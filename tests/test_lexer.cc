#include <vector>

#include <gtest/gtest.h>

#include "cool-lex.h"
#include "utilities.h"

#define CODE(s) std::string((s), sizeof(s) - 1)

struct ExpectedToken {
  int type;
  unsigned int line;
};

class LexerTest : public ::testing::TestWithParam<std::tuple<std::string, std::vector<ExpectedToken>>> {};

TEST_P(LexerTest, VerifiesEachTokenIndividually) {
  auto [input, expected_tokens] = GetParam();
  std::istringstream iss(input);
  LexState lexer(iss);

  LexState::Value yylval;
  LexState::Location yylloc;

  for (size_t i = 0; i < expected_tokens.size(); ++i) {
    const auto& expected = expected_tokens[i];

    int token_type = lexer.lex(&yylval, &yylloc);

    EXPECT_EQ(token_type, expected.type)
      << "Token #" << i << " type mismatch\n"
      << "Input: " << input << "\n"
      << "Position: line " << yylloc.line;

    EXPECT_EQ(yylloc.line, expected.line)
      << "Token #" << i << " line number mismatch\n"
      << "Input: " << input << "\n";
  }

  EXPECT_EQ(lexer.lex(&yylval, &yylloc), TOKID(END))
    << "Input did not end with EOF: " << input;
}

INSTANTIATE_TEST_SUITE_P(
  SingleTokensTests,
  LexerTest,
  ::testing::Values(
    std::make_tuple(CODE("x"), std::vector<ExpectedToken>{
      { TOKID(OBJECTID), 1 },
    }),
    std::make_tuple(CODE("42"), std::vector<ExpectedToken>{
      { TOKID(NUMBER), 1 },
    }),
    std::make_tuple(CODE("\"hello\""), std::vector<ExpectedToken>{
      { TOKID(STRING), 1 },
    }),
    std::make_tuple(CODE("\"s\\\n\""), std::vector<ExpectedToken>{
      { TOKID(STRING), 2 },
    })
  )
);

INSTANTIATE_TEST_SUITE_P(
  MultiTokenTests,
  LexerTest,
  ::testing::Values(
    std::make_tuple(CODE("x + 42"), std::vector<ExpectedToken>{
      { TOKID(OBJECTID), 1 },
      { int('+'), 1 },
      { TOKID(NUMBER), 1 },
    }),
    std::make_tuple(CODE("if x\n  then 1"), std::vector<ExpectedToken>{
      { TOKID(IF), 1 },
      { TOKID(OBJECTID), 1 },
      { TOKID(THEN), 2 },
      { TOKID(NUMBER), 2 },
    })
  )
);

INSTANTIATE_TEST_SUITE_P(
  EdgeCases,
  LexerTest,
  ::testing::Values(
    std::make_tuple(CODE(""), std::vector<ExpectedToken>{}),
    std::make_tuple(CODE("  \t\n  "), std::vector<ExpectedToken>{}),
    std::make_tuple(CODE("x -- comment\ny"), std::vector<ExpectedToken>{
      { TOKID(OBJECTID), 1 },
      { TOKID(OBJECTID), 2 },
    })
  )
);

INSTANTIATE_TEST_SUITE_P(
  ErrorHandling,
  LexerTest,
  ::testing::Values(
    std::make_tuple(CODE("\"unterminated"), std::vector<ExpectedToken>{
      { TOKID(ERROR), 1 },
    }),
    std::make_tuple(CODE("\"null\0\\\n\n"), std::vector<ExpectedToken>{
      { TOKID(ERROR), 3 },
    }),
    std::make_tuple(CODE("\"\0\na;\nb"), std::vector<ExpectedToken>{
      { TOKID(ERROR), 2 },
      { TOKID(OBJECTID), 2 },
      { int(';'), 2 },
      { TOKID(OBJECTID), 3 },
    })
  )
);
