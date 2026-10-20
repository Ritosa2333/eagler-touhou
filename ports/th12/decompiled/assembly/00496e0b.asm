
// block 00496e0b
00496e0b  mov        edx, dword ptr [esp + 8]
00496e0f  lea        eax, [edx - 0x5c]
00496e12  mov        ecx, dword ptr [edx - 0x60]
00496e15  xor        ecx, eax
00496e17  call       0x46c932

// block 00496e1c
00496e1c  add        eax, 0x10
00496e1f  mov        ecx, dword ptr [edx - 4]
00496e22  xor        ecx, eax
00496e24  call       0x46c932

// block 00496e29
00496e29  mov        eax, 0x4ab614
00496e2e  jmp        0x491aab
