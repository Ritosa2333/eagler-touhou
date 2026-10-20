
// block 004386b9
004386b9  push       0x16
004386bb  lea        ecx, [esp + 0x38]

// block 004386bf
004386bf  mov        edi, dword ptr [ebp + 0x10]
004386c2  push       ecx
004386c3  mov        eax, 0xc
004386c8  call       0x41c6a0

// block 004386cd
004386cd  mov        edx, dword ptr [eax]
004386cf  mov        dword ptr [ebx], edx
004386d1  jmp        0x438703
