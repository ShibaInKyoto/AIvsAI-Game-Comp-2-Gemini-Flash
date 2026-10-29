#pragma once

#include <vector>
#include <string>
#include "VMTypes.h"
#include "OpCodes.h"

namespace Voidfall::Scripting {

class Chunk {
public:
    std::vector<uint8_t> code;
    std::vector<Value> constants;
    std::vector<int> lines;

    void Write(uint8_t byte, int line) {
        code.push_back(byte);
        lines.push_back(line);
    }

    void WriteOp(OpCode op, int line) {
        Write(static_cast<uint8_t>(op), line);
    }

    int AddConstant(const Value& val) {
        constants.push_back(val);
        return static_cast<int>(constants.size()) - 1;
    }

    void Clear() {
        code.clear();
        constants.clear();
        lines.clear();
    }
};

} // namespace Voidfall::Scripting
