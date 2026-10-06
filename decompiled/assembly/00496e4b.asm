
// block 00496e4b
00496e4b  mov        edx, dword ptr [esp + 8]
00496e4f  lea        eax, [edx - 0x5c]
00496e52  mov        ecx, dword ptr [edx - 0x60]
00496e55  xor        ecx, eax
00496e57  call       0x46c932

// block 00496e5c
00496e5c  add        eax, 0x10
00496e5f  mov        ecx, dword ptr [edx - 4]
00496e62  xor        ecx, eax
00496e64  call       0x46c932

// block 00496e69
00496e69  mov        eax, 0x4ab640
00496e6e  jmp        0x491aab
