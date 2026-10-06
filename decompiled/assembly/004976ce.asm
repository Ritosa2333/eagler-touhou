
// block 004976ce
004976ce  mov        edx, dword ptr [esp + 8]
004976d2  lea        eax, [edx - 0x314]
004976d8  mov        ecx, dword ptr [edx - 0x318]
004976de  xor        ecx, eax
004976e0  call       0x46c932

// block 004976e5
004976e5  add        eax, 0xc
004976e8  mov        ecx, dword ptr [edx - 8]
004976eb  xor        ecx, eax
004976ed  call       0x46c932

// block 004976f2
004976f2  mov        eax, 0x4abd2c
004976f7  jmp        0x491aab
