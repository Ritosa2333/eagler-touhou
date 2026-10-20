
// block 004386d3
004386d3  push       0xd
004386d5  lea        eax, [esp + 0x3c]
004386d9  jmp        0x4386f1

// block 004386f1
004386f1  mov        edi, dword ptr [ebp + 0x10]
004386f4  push       eax
004386f5  mov        eax, 0xc
004386fa  call       0x41c6a0

// block 004386ff
004386ff  mov        ecx, dword ptr [eax]
00438701  mov        dword ptr [ebx], ecx
