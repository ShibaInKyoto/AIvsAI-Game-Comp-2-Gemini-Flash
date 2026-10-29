#pragma once

#include <string>
#include <iostream>
#include "OpCodes.h"

namespace Voidfall::Scripting {

struct Value {
    ValueType type{ValueType::NIL};
    union {
        bool boolean;
        double number;
    } as;
    std::string stringVal;

    Value() : type(ValueType::NIL) { as.number = 0.0; }
    Value(bool b) : type(ValueType::BOOL) { as.boolean = b; }
    Value(double n) : type(ValueType::NUMBER) { as.number = n; }
    Value(float f) : type(ValueType::NUMBER) { as.number = static_cast<double>(f); }
    Value(int i) : type(ValueType::NUMBER) { as.number = static_cast<double>(i); }
    Value(const std::string& s) : type(ValueType::STRING), stringVal(s) { as.number = 0.0; }
    Value(const char* s) : type(ValueType::STRING), stringVal(s ? s : "") { as.number = 0.0; }

    bool IsNil() const noexcept { return type == ValueType::NIL; }
    bool IsBool() const noexcept { return type == ValueType::BOOL; }
    bool IsNumber() const noexcept { return type == ValueType::NUMBER; }
    bool IsString() const noexcept { return type == ValueType::STRING; }

    bool AsBool() const noexcept { return (type == ValueType::BOOL) ? as.boolean : (type == ValueType::NUMBER ? as.number != 0.0 : !IsNil()); }
    double AsNumber() const noexcept { return (type == ValueType::NUMBER) ? as.number : 0.0; }
    const std::string& AsString() const noexcept { return stringVal; }

    std::string ToString() const {
        switch (type) {
            case ValueType::NIL: return "nil";
            case ValueType::BOOL: return as.boolean ? "true" : "false";
            case ValueType::NUMBER: {
                std::string s = std::to_string(as.number);
                s.erase(s.find_last_not_of('0') + 1, std::string::npos);
                if (!s.empty() && s.back() == '.') s.pop_back();
                return s;
            }
            case ValueType::STRING: return stringVal;
        }
        return "";
    }

    bool operator==(const Value& o) const {
        if (type != o.type) return false;
        switch (type) {
            case ValueType::NIL: return true;
            case ValueType::BOOL: return as.boolean == o.as.boolean;
            case ValueType::NUMBER: return as.number == o.as.number;
            case ValueType::STRING: return stringVal == o.stringVal;
        }
        return false;
    }
};

} // namespace Voidfall::Scripting
