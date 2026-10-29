#include "Compiler.h"
#include <cctype>
#include <cstdlib>

namespace Voidfall::Scripting {

// --- LEXER ---

Lexer::Lexer(const std::string& source) : m_source(source), m_start(0), m_current(0), m_line(1) {}

bool Lexer::IsAtEnd() const {
    return m_current >= static_cast<int>(m_source.size());
}

char Lexer::Advance() {
    return m_source[m_current++];
}

char Lexer::Peek() const {
    if (IsAtEnd()) return '\0';
    return m_source[m_current];
}

char Lexer::PeekNext() const {
    if (m_current + 1 >= static_cast<int>(m_source.size())) return '\0';
    return m_source[m_current + 1];
}

bool Lexer::Match(char expected) {
    if (IsAtEnd()) return false;
    if (m_source[m_current] != expected) return false;
    m_current++;
    return true;
}

void Lexer::SkipWhitespace() {
    while (true) {
        char c = Peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                Advance();
                break;
            case '\n':
                m_line++;
                Advance();
                break;
            case '/':
                if (PeekNext() == '/') {
                    while (Peek() != '\n' && !IsAtEnd()) Advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::MakeToken(TokenType type) {
    Token token;
    token.type = type;
    token.lexeme = m_source.substr(m_start, m_current - m_start);
    token.line = m_line;
    return token;
}

Token Lexer::ErrorToken(const std::string& message) {
    Token token;
    token.type = TokenType::TOKEN_ERROR;
    token.lexeme = message;
    token.line = m_line;
    return token;
}

Token Lexer::ScanIdentifier() {
    while (std::isalnum(Peek()) || Peek() == '_') Advance();

    std::string text = m_source.substr(m_start, m_current - m_start);
    TokenType type = TokenType::IDENTIFIER;

    if (text == "var") type = TokenType::VAR;
    else if (text == "if") type = TokenType::IF;
    else if (text == "else") type = TokenType::ELSE;
    else if (text == "while") type = TokenType::WHILE;
    else if (text == "yield") type = TokenType::YIELD;
    else if (text == "print") type = TokenType::PRINT;
    else if (text == "true") type = TokenType::TRUE;
    else if (text == "false") type = TokenType::FALSE;
    else if (text == "nil") type = TokenType::NIL;

    return MakeToken(type);
}

Token Lexer::ScanNumber() {
    while (std::isdigit(Peek())) Advance();

    if (Peek() == '.' && std::isdigit(PeekNext())) {
        Advance(); // Consume '.'
        while (std::isdigit(Peek())) Advance();
    }

    return MakeToken(TokenType::NUMBER);
}

Token Lexer::ScanString() {
    while (Peek() != '"' && !IsAtEnd()) {
        if (Peek() == '\n') m_line++;
        Advance();
    }

    if (IsAtEnd()) return ErrorToken("Unterminated string literal.");

    Advance(); // Consume closing quote
    Token token;
    token.type = TokenType::STRING;
    // Extract without quotes
    token.lexeme = m_source.substr(m_start + 1, m_current - m_start - 2);
    token.line = m_line;
    return token;
}

Token Lexer::ScanToken() {
    SkipWhitespace();
    m_start = m_current;

    if (IsAtEnd()) return MakeToken(TokenType::TOKEN_EOF);

    char c = Advance();

    if (std::isalpha(c) || c == '_') return ScanIdentifier();
    if (std::isdigit(c)) return ScanNumber();

    switch (c) {
        case '(': return MakeToken(TokenType::LEFT_PAREN);
        case ')': return MakeToken(TokenType::RIGHT_PAREN);
        case '{': return MakeToken(TokenType::LEFT_BRACE);
        case '}': return MakeToken(TokenType::RIGHT_BRACE);
        case ';': return MakeToken(TokenType::SEMICOLON);
        case ',': return MakeToken(TokenType::COMMA);
        case '+': return MakeToken(TokenType::PLUS);
        case '-': return MakeToken(TokenType::MINUS);
        case '*': return MakeToken(TokenType::STAR);
        case '/': return MakeToken(TokenType::SLASH);
        case '%': return MakeToken(TokenType::PERCENT);
        case '!': return MakeToken(Match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=': return MakeToken(Match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<': return MakeToken(Match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>': return MakeToken(Match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '"': return ScanString();
    }

    return ErrorToken("Unexpected character.");
}

// --- COMPILER ---

bool Compiler::Compile(const std::string& source, Chunk& chunk) {
    Lexer lexer(source);
    m_lexer = &lexer;
    m_chunk = &chunk;
    m_hadError = false;
    m_errorMessage.clear();
    m_locals.clear();
    m_scopeDepth = 0;

    Advance();

    while (!Match(TokenType::TOKEN_EOF)) {
        Declaration();
        if (m_hadError) break;
    }

    EmitOp(OpCode::HALT);
    return !m_hadError;
}

void Compiler::Advance() {
    m_previous = m_current;

    while (true) {
        m_current = m_lexer->ScanToken();
        if (m_current.type != TokenType::TOKEN_ERROR) break;

        m_hadError = true;
        m_errorMessage = "Lex error on line " + std::to_string(m_current.line) + ": " + m_current.lexeme;
    }
}

void Compiler::Consume(TokenType type, const std::string& message) {
    if (m_current.type == type) {
        Advance();
        return;
    }
    m_hadError = true;
    m_errorMessage = "Syntax error on line " + std::to_string(m_current.line) + ": " + message;
}

bool Compiler::Check(TokenType type) {
    return m_current.type == type;
}

bool Compiler::Match(TokenType type) {
    if (!Check(type)) return false;
    Advance();
    return true;
}

void Compiler::EmitByte(uint8_t byte) {
    m_chunk->Write(byte, m_previous.line);
}

void Compiler::EmitOp(OpCode op) {
    m_chunk->WriteOp(op, m_previous.line);
}

void Compiler::EmitBytes(uint8_t byte1, uint8_t byte2) {
    EmitByte(byte1);
    EmitByte(byte2);
}

void Compiler::EmitConstant(const Value& val) {
    int constIdx = m_chunk->AddConstant(val);
    EmitOp(OpCode::CONSTANT);
    EmitByte(static_cast<uint8_t>(constIdx));
}

int Compiler::EmitJump(OpCode instruction) {
    EmitOp(instruction);
    EmitByte(0xFF);
    EmitByte(0xFF);
    return static_cast<int>(m_chunk->code.size()) - 2;
}

void Compiler::PatchJump(int offset) {
    int jump = static_cast<int>(m_chunk->code.size()) - offset - 2;
    m_chunk->code[offset] = static_cast<uint8_t>((jump >> 8) & 0xFF);
    m_chunk->code[offset + 1] = static_cast<uint8_t>(jump & 0xFF);
}

void Compiler::EmitLoop(int loopStart) {
    EmitOp(OpCode::LOOP);
    int jump = static_cast<int>(m_chunk->code.size()) - loopStart + 2;
    EmitByte(static_cast<uint8_t>((jump >> 8) & 0xFF));
    EmitByte(static_cast<uint8_t>(jump & 0xFF));
}

void Compiler::Declaration() {
    if (Match(TokenType::VAR)) {
        VarDeclaration();
    } else {
        Statement();
    }
}

void Compiler::VarDeclaration() {
    Consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::string varName = m_previous.lexeme;

    if (Match(TokenType::EQUAL)) {
        Expression();
    } else {
        EmitOp(OpCode::NIL);
    }

    Consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");

    int constIdx = m_chunk->AddConstant(Value(varName));
    EmitOp(OpCode::SET_GLOBAL);
    EmitByte(static_cast<uint8_t>(constIdx));
    EmitOp(OpCode::POP); // statement pops result
}

void Compiler::Statement() {
    if (Match(TokenType::PRINT)) {
        PrintStatement();
    } else if (Match(TokenType::IF)) {
        IfStatement();
    } else if (Match(TokenType::WHILE)) {
        WhileStatement();
    } else if (Match(TokenType::YIELD)) {
        YieldStatement();
    } else if (Match(TokenType::LEFT_BRACE)) {
        while (!Check(TokenType::RIGHT_BRACE) && !Check(TokenType::TOKEN_EOF)) {
            Declaration();
        }
        Consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    } else {
        ExpressionStatement();
    }
}

void Compiler::PrintStatement() {
    Expression();
    Consume(TokenType::SEMICOLON, "Expect ';' after print statement.");
    // In our VM, print pops and displays
    int constIdx = m_chunk->AddConstant(Value("print"));
    EmitOp(OpCode::NATIVE_CALL);
    EmitByte(static_cast<uint8_t>(constIdx));
    EmitByte(1); // 1 arg
}

void Compiler::YieldStatement() {
    Consume(TokenType::SEMICOLON, "Expect ';' after yield.");
    EmitOp(OpCode::YIELD);
}

void Compiler::IfStatement() {
    Consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    Expression();
    Consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");

    int thenJump = EmitJump(OpCode::JUMP_IF_FALSE);
    EmitOp(OpCode::POP); // pop condition
    Statement();

    int elseJump = EmitJump(OpCode::JUMP);
    PatchJump(thenJump);
    EmitOp(OpCode::POP); // pop condition on false path

    if (Match(TokenType::ELSE)) {
        Statement();
    }
    PatchJump(elseJump);
}

void Compiler::WhileStatement() {
    int loopStart = static_cast<int>(m_chunk->code.size());
    Consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    Expression();
    Consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");

    int exitJump = EmitJump(OpCode::JUMP_IF_FALSE);
    EmitOp(OpCode::POP);
    Statement();
    EmitLoop(loopStart);

    PatchJump(exitJump);
    EmitOp(OpCode::POP);
}

void Compiler::ExpressionStatement() {
    Expression();
    Consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    EmitOp(OpCode::POP);
}

void Compiler::Expression() {
    Assignment();
}

void Compiler::Assignment() {
    Equality();

    if (Match(TokenType::EQUAL)) {
        // Handle assignment to identifier
        if (m_previous.type == TokenType::IDENTIFIER || true) {
            std::string varName = m_previous.lexeme;
            Expression();
            int constIdx = m_chunk->AddConstant(Value(varName));
            EmitOp(OpCode::SET_GLOBAL);
            EmitByte(static_cast<uint8_t>(constIdx));
        }
    }
}

void Compiler::Equality() {
    Comparison();
    while (Match(TokenType::BANG_EQUAL) || Match(TokenType::EQUAL_EQUAL)) {
        TokenType op = m_previous.type;
        Comparison();
        if (op == TokenType::EQUAL_EQUAL) {
            EmitOp(OpCode::EQUAL);
        } else {
            EmitOp(OpCode::EQUAL);
            EmitOp(OpCode::NOT);
        }
    }
}

void Compiler::Comparison() {
    Term();
    while (Match(TokenType::GREATER) || Match(TokenType::GREATER_EQUAL) ||
           Match(TokenType::LESS) || Match(TokenType::LESS_EQUAL)) {
        TokenType op = m_previous.type;
        Term();
        if (op == TokenType::GREATER) EmitOp(OpCode::GREATER);
        else if (op == TokenType::LESS) EmitOp(OpCode::LESS);
        else if (op == TokenType::GREATER_EQUAL) {
            EmitOp(OpCode::LESS);
            EmitOp(OpCode::NOT);
        } else if (op == TokenType::LESS_EQUAL) {
            EmitOp(OpCode::GREATER);
            EmitOp(OpCode::NOT);
        }
    }
}

void Compiler::Term() {
    Factor();
    while (Match(TokenType::PLUS) || Match(TokenType::MINUS)) {
        TokenType op = m_previous.type;
        Factor();
        if (op == TokenType::PLUS) EmitOp(OpCode::ADD);
        else EmitOp(OpCode::SUB);
    }
}

void Compiler::Factor() {
    Unary();
    while (Match(TokenType::STAR) || Match(TokenType::SLASH) || Match(TokenType::PERCENT)) {
        TokenType op = m_previous.type;
        Unary();
        if (op == TokenType::STAR) EmitOp(OpCode::MUL);
        else if (op == TokenType::SLASH) EmitOp(OpCode::DIV);
        else EmitOp(OpCode::MOD);
    }
}

void Compiler::Unary() {
    if (Match(TokenType::BANG)) {
        Unary();
        EmitOp(OpCode::NOT);
    } else if (Match(TokenType::MINUS)) {
        Unary();
        EmitOp(OpCode::NEGATE);
    } else {
        Primary();
    }
}

void Compiler::Primary() {
    if (Match(TokenType::FALSE)) {
        EmitOp(OpCode::FALSE);
    } else if (Match(TokenType::TRUE)) {
        EmitOp(OpCode::TRUE);
    } else if (Match(TokenType::NIL)) {
        EmitOp(OpCode::NIL);
    } else if (Match(TokenType::NUMBER)) {
        double val = std::strtod(m_previous.lexeme.c_str(), nullptr);
        EmitConstant(Value(val));
    } else if (Match(TokenType::STRING)) {
        EmitConstant(Value(m_previous.lexeme));
    } else if (Match(TokenType::IDENTIFIER)) {
        std::string name = m_previous.lexeme;
        if (Match(TokenType::LEFT_PAREN)) {
            // Function call
            uint8_t argCount = 0;
            if (!Check(TokenType::RIGHT_PAREN)) {
                do {
                    Expression();
                    argCount++;
                } while (Match(TokenType::COMMA));
            }
            Consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
            int constIdx = m_chunk->AddConstant(Value(name));
            EmitOp(OpCode::NATIVE_CALL);
            EmitByte(static_cast<uint8_t>(constIdx));
            EmitByte(argCount);
        } else {
            // Variable read
            int constIdx = m_chunk->AddConstant(Value(name));
            EmitOp(OpCode::GET_GLOBAL);
            EmitByte(static_cast<uint8_t>(constIdx));
        }
    } else if (Match(TokenType::LEFT_PAREN)) {
        Expression();
        Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    } else {
        m_hadError = true;
        m_errorMessage = "Expect expression on line " + std::to_string(m_current.line);
    }
}

} // namespace Voidfall::Scripting
