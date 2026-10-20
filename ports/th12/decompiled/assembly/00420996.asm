
// block 00420996
00420996  mov        edx, dword ptr [0x4b43dc]
0042099c  mov        eax, dword ptr [edx + 0x48]
0042099f  push       0
004209a1  push       0xe
004209a3  lea        ecx, [esp + 0x40]
004209a7  push       ecx
004209a8  push       eax
004209a9  mov        eax, 0x17
004209ae  xor        ecx, ecx
004209b0  call       0x4615a0

// block 004209b5
004209b5  mov        ecx, dword ptr [esp + 0x38]
004209b9  mov        dword ptr [ebp + 0x5c], ecx
004209bc  jmp        0x420ced
