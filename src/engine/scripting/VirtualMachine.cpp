#include "VirtualMachine.h"
#include <iostream>
#include <cmath>

namespace Voidfall::Scripting {

VirtualMachine::VirtualMachine() {
    Reset();

    // Register built-in default natives
    RegisterNative("print", [](const std::vector<Value>& args) -> Value {
        for (size_t i = 0; i < args.size(); ++i) {
            std::cout << args[i].ToString() << (i + 1 < args.size() ? " " : "");
        }
        std::cout << std::endl;
        return Value();
    });

    RegisterNative("sin", [](const std::vector<Value>& args) -> Value {
        if (args.empty()) return Value(0.0);
        return Value(std::sin(args[0].AsNumber()));
    });

    RegisterNative("cos", [](const std::vector<Value>& args) -> Value {
        if (args.empty()) return Value(0.0);
        return Value(std::cos(args[0].AsNumber()));
    });

    RegisterNative("sqrt", [](const std::vector<Value>& args) -> Value {
        if (args.empty()) return Value(0.0);
        return Value(std::sqrt(std::max(0.0, args[0].AsNumber())));
    });
}

void VirtualMachine::Reset() {
    m_chunk = nullptr;
    m_ip = 0;
    m_stack.clear();
    m_lastError.clear();
    m_isYielded = false;
}

void VirtualMachine::RegisterNative(const std::string& name, NativeFn fn) {
    m_natives[name] = fn;
}

void VirtualMachine::SetGlobal(const std::string& name, const Value& val) {
    m_globals[name] = val;
}

Value VirtualMachine::GetGlobal(const std::string& name) const {
    auto it = m_globals.find(name);
    return (it != m_globals.end()) ? it->second : Value();
}

bool VirtualMachine::HasGlobal(const std::string& name) const {
    return m_globals.find(name) != m_globals.end();
}

void VirtualMachine::Push(const Value& val) {
    m_stack.push_back(val);
}

Value VirtualMachine::Pop() {
    if (m_stack.empty()) return Value();
    Value v = m_stack.back();
    m_stack.pop_back();
    return v;
}

Value VirtualMachine::Peek(int distance) const {
    if (m_stack.size() <= static_cast<size_t>(distance)) return Value();
    return m_stack[m_stack.size() - 1 - distance];
}

void VirtualMachine::RuntimeError(const std::string& message) {
    m_lastError = message;
    if (m_chunk && m_ip < m_chunk->lines.size()) {
        m_lastError += " [line " + std::to_string(m_chunk->lines[m_ip]) + "]";
    }
}

VMResult VirtualMachine::Interpret(Chunk* chunk) {
    m_chunk = chunk;
    m_ip = 0;
    m_stack.clear();
    m_isYielded = false;
    return Run();
}

VMResult VirtualMachine::Resume() {
    if (!m_isYielded || !m_chunk) {
        return VMResult::OK;
    }
    m_isYielded = false;
    return Run();
}

VMResult VirtualMachine::Run() {
    if (!m_chunk) return VMResult::RUNTIME_ERROR;

    auto readByte = [this]() -> uint8_t {
        return m_chunk->code[m_ip++];
    };

    auto readShort = [this]() -> uint16_t {
        m_ip += 2;
        return static_cast<uint16_t>((m_chunk->code[m_ip - 2] << 8) | m_chunk->code[m_ip - 1]);
    };

    while (m_ip < m_chunk->code.size()) {
        OpCode op = static_cast<OpCode>(readByte());

        switch (op) {
            case OpCode::NOP:
                break;

            case OpCode::HALT:
                return VMResult::OK;

            case OpCode::CONSTANT: {
                uint8_t constIdx = readByte();
                Push(m_chunk->constants[constIdx]);
                break;
            }

            case OpCode::NIL:
                Push(Value());
                break;

            case OpCode::TRUE:
                Push(Value(true));
                break;

            case OpCode::FALSE:
                Push(Value(false));
                break;

            case OpCode::POP:
                Pop();
                break;

            case OpCode::DUP:
                Push(Peek(0));
                break;

            case OpCode::GET_GLOBAL: {
                uint8_t constIdx = readByte();
                std::string name = m_chunk->constants[constIdx].AsString();
                auto it = m_globals.find(name);
                if (it == m_globals.end()) {
                    Push(Value()); // Undefined variables default to nil
                } else {
                    Push(it->second);
                }
                break;
            }

            case OpCode::SET_GLOBAL: {
                uint8_t constIdx = readByte();
                std::string name = m_chunk->constants[constIdx].AsString();
                m_globals[name] = Peek(0);
                break;
            }

            case OpCode::ADD: {
                Value b = Pop();
                Value a = Pop();
                if (a.IsString() || b.IsString()) {
                    Push(Value(a.ToString() + b.ToString()));
                } else {
                    Push(Value(a.AsNumber() + b.AsNumber()));
                }
                break;
            }

            case OpCode::SUB: {
                Value b = Pop();
                Value a = Pop();
                Push(Value(a.AsNumber() - b.AsNumber()));
                break;
            }

            case OpCode::MUL: {
                Value b = Pop();
                Value a = Pop();
                Push(Value(a.AsNumber() * b.AsNumber()));
                break;
            }

            case OpCode::DIV: {
                Value b = Pop();
                Value a = Pop();
                double denom = b.AsNumber();
                Push(Value((denom != 0.0) ? (a.AsNumber() / denom) : 0.0));
                break;
            }

            case OpCode::MOD: {
                Value b = Pop();
                Value a = Pop();
                double denom = b.AsNumber();
                Push(Value((denom != 0.0) ? std::fmod(a.AsNumber(), denom) : 0.0));
                break;
            }

            case OpCode::NEGATE: {
                Value v = Pop();
                Push(Value(-v.AsNumber()));
                break;
            }

            case OpCode::NOT: {
                Value v = Pop();
                Push(Value(!v.AsBool()));
                break;
            }

            case OpCode::EQUAL: {
                Value b = Pop();
                Value a = Pop();
                Push(Value(a == b));
                break;
            }

            case OpCode::GREATER: {
                Value b = Pop();
                Value a = Pop();
                Push(Value(a.AsNumber() > b.AsNumber()));
                break;
            }

            case OpCode::LESS: {
                Value b = Pop();
                Value a = Pop();
                Push(Value(a.AsNumber() < b.AsNumber()));
                break;
            }

            case OpCode::JUMP: {
                uint16_t offset = readShort();
                m_ip += offset;
                break;
            }

            case OpCode::JUMP_IF_FALSE: {
                uint16_t offset = readShort();
                if (!Peek(0).AsBool()) {
                    m_ip += offset;
                }
                break;
            }

            case OpCode::LOOP: {
                uint16_t offset = readShort();
                m_ip -= offset;
                break;
            }

            case OpCode::YIELD: {
                m_isYielded = true;
                return VMResult::YIELDED;
            }

            case OpCode::NATIVE_CALL: {
                uint8_t constIdx = readByte();
                uint8_t argCount = readByte();
                std::string fnName = m_chunk->constants[constIdx].AsString();

                std::vector<Value> args(argCount);
                for (int i = argCount - 1; i >= 0; --i) {
                    args[i] = Pop();
                }

                auto it = m_natives.find(fnName);
                if (it != m_natives.end()) {
                    Value ret = it->second(args);
                    Push(ret);
                } else {
                    Push(Value()); // Return nil for unhandled natives
                }
                break;
            }

            default:
                RuntimeError("Unknown opcode instruction.");
                return VMResult::RUNTIME_ERROR;
        }
    }

    return VMResult::OK;
}

} // namespace Voidfall::Scripting
