#include "engine/scripting/Compiler.h"
#include "engine/scripting/VirtualMachine.h"
#include <cassert>
#include <iostream>

namespace Voidfall {

void RunScriptingVMTests() {
    std::cout << "[TEST] Running Scripting Bytecode VM Tests...\n";

    using namespace Scripting;

    // 1. Basic Arithmetic & Expression Compilation
    std::string script1 =
        "var a = 15;\n"
        "var b = 25;\n"
        "var c = a + b * 2;\n";

    Compiler compiler;
    Chunk chunk1;
    bool compiled = compiler.Compile(script1, chunk1);
    assert(compiled);

    VirtualMachine vm;
    VMResult res = vm.Interpret(&chunk1);
    assert(res == VMResult::OK);
    assert(vm.GetGlobal("c").AsNumber() == 65.0);

    // 2. Control Flow (If / Else & While Loops)
    std::string script2 =
        "var count = 0;\n"
        "var total = 0;\n"
        "while (count < 5) {\n"
        "    count = count + 1;\n"
        "    total = total + count;\n"
        "}\n";

    Chunk chunk2;
    compiled = compiler.Compile(script2, chunk2);
    assert(compiled);

    vm.Reset();
    res = vm.Interpret(&chunk2);
    assert(res == VMResult::OK);
    assert(vm.GetGlobal("count").AsNumber() == 5.0);
    assert(vm.GetGlobal("total").AsNumber() == 15.0); // 1+2+3+4+5

    // 3. Native Function Binding & Calling
    std::string script3 =
        "var x = 16;\n"
        "var root = sqrt(x);\n"
        "var text = \"Value: \" + root;\n";

    Chunk chunk3;
    compiled = compiler.Compile(script3, chunk3);
    assert(compiled);

    vm.Reset();
    res = vm.Interpret(&chunk3);
    assert(res == VMResult::OK);
    assert(vm.GetGlobal("root").AsNumber() == 4.0);
    assert(vm.GetGlobal("text").AsString() == "Value: 4");

    // 4. Yield & Coroutine Resume
    std::string script4 =
        "var step = 1;\n"
        "yield;\n"
        "step = 2;\n";

    Chunk chunk4;
    compiled = compiler.Compile(script4, chunk4);
    assert(compiled);

    vm.Reset();
    res = vm.Interpret(&chunk4);
    assert(res == VMResult::YIELDED);
    assert(vm.GetGlobal("step").AsNumber() == 1.0);

    res = vm.Resume();
    assert(res == VMResult::OK);
    assert(vm.GetGlobal("step").AsNumber() == 2.0);

    std::cout << "[PASS] Scripting Bytecode VM Tests passed!\n";
}

} // namespace Voidfall
