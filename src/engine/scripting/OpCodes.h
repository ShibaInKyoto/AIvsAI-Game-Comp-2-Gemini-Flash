#pragma once

#include <cstdint>

namespace Voidfall::Scripting {

enum class OpCode : uint8_t {
    NOP = 0,
    CONSTANT,
    NIL,
    TRUE,
    FALSE,
    POP,
    DUP,
    GET_LOCAL,
    SET_LOCAL,
    GET_GLOBAL,
    SET_GLOBAL,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    NEGATE,
    NOT,
    EQUAL,
    GREATER,
    LESS,
    JUMP,
    JUMP_IF_FALSE,
    LOOP,
    CALL,
    RETURN,
    YIELD,
    NATIVE_CALL,
    HALT
};

enum class ValueType : uint8_t {
    NIL,
    BOOL,
    NUMBER,
    STRING
};

} // namespace Voidfall::Scripting
