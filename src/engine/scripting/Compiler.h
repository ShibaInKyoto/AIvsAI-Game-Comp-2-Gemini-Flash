#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Bytecode.h"

namespace Voidfall::Scripting {

enum class TokenType {
    // Single-character
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, SEMICOLON, PLUS, MINUS, STAR, SLASH, PERCENT,
    // One or two character
    BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,
    // Literals
    IDENTIFIER, STRING, NUMBER,
    // Keywords
    VAR, IF, ELSE, WHILE, YIELD, PRINT, TRUE, FALSE, NIL,
    // Special
    TOKEN_ERROR, TOKEN_EOF
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line{1};
};

class Lexer {
public:
    explicit Lexer(const std::string& source);
    Token ScanToken();

private:
    bool IsAtEnd() const;
    char Advance();
    char Peek() const;
    char PeekNext() const;
    bool Match(char expected);
    void SkipWhitespace();

    Token MakeToken(TokenType type);
    Token ErrorToken(const std::string& message);
    Token ScanIdentifier();
    Token ScanNumber();
    Token ScanString();

    std::string m_source;
    int m_start{0};
    int m_current{0};
    int m_line{1};
};

class Compiler {
public:
    Compiler() = default;
    bool Compile(const std::string& source, Chunk& chunk);
    const std::string& GetErrorMessage() const { return m_errorMessage; }

private:
    void Advance();
    void Consume(TokenType type, const std::string& message);
    bool Check(TokenType type);
    bool Match(TokenType type);

    void Declaration();
    void VarDeclaration();
    void Statement();
    void IfStatement();
    void WhileStatement();
    void YieldStatement();
    void PrintStatement();
    void ExpressionStatement();

    void Expression();
    void Assignment();
    void LogicOr();
    void LogicAnd();
    void Equality();
    void Comparison();
    void Term();
    void Factor();
    void Unary();
    void Primary();

    void EmitByte(uint8_t byte);
    void EmitOp(OpCode op);
    void EmitBytes(uint8_t byte1, uint8_t byte2);
    void EmitConstant(const Value& val);
    int EmitJump(OpCode instruction);
    void PatchJump(int offset);
    void EmitLoop(int loopStart);

    Lexer* m_lexer{nullptr};
    Token m_current;
    Token m_previous;
    Chunk* m_chunk{nullptr};
    bool m_hadError{false};
    std::string m_errorMessage;

    std::unordered_map<std::string, int> m_locals;
    int m_scopeDepth{0};
};

} // namespace Voidfall::Scripting
