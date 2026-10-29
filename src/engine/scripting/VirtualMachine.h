#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
#include "Bytecode.h"

namespace Voidfall::Scripting {

enum class VMResult {
    OK,
    YIELDED,
    RUNTIME_ERROR
};

using NativeFn = std::function<Value(const std::vector<Value>& args)>;

class VirtualMachine {
public:
    VirtualMachine();

    void RegisterNative(const std::string& name, NativeFn fn);
    VMResult Interpret(Chunk* chunk);
    VMResult Resume();

    void SetGlobal(const std::string& name, const Value& val);
    Value GetGlobal(const std::string& name) const;
    bool HasGlobal(const std::string& name) const;

    const std::string& GetLastError() const { return m_lastError; }
    void Reset();

private:
    VMResult Run();
    void Push(const Value& val);
    Value Pop();
    Value Peek(int distance = 0) const;
    void RuntimeError(const std::string& message);

    Chunk* m_chunk{nullptr};
    size_t m_ip{0};
    std::vector<Value> m_stack;
    std::unordered_map<std::string, Value> m_globals;
    std::unordered_map<std::string, NativeFn> m_natives;
    std::string m_lastError;
    bool m_isYielded{false};
};

} // namespace Voidfall::Scripting
