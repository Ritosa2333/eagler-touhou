
// block 0043869f
0043869f  push       0x15
004386a1  lea        edx, [esp + 0x34]

// block 004386a5
004386a5  mov        edi, dword ptr [ebp + 0x10]
004386a8  push       edx
004386a9  mov        eax, 0xc
004386ae  call       0x41c6a0

// block 004386b3
004386b3  mov        eax, dword ptr [eax]
004386b5  mov        dword ptr [ebx], eax
004386b7  jmp        0x438703
