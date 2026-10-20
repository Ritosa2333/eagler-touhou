
// block 00431d00
00431d00  mov        eax, 0xfffffffe
00431d05  and        dword ptr [esi + 0x20], eax
00431d08  and        dword ptr [esi + 0x34], eax
00431d0b  xor        eax, eax
00431d0d  mov        dword ptr [esi + 0xc4], eax
00431d13  mov        dword ptr [esi + 0x38], eax
00431d16  mov        dword ptr [esi + 0x10c], eax
00431d1c  push       0x2e0
00431d21  mov        edx, 1
00431d26  mov        dword ptr [esi + 0x108], edx
00431d2c  mov        ecx, 0x3e7
00431d31  mov        dword ptr [esi + 0x40], ecx
00431d34  push       eax
00431d35  push       esi
00431d36  mov        dword ptr [esi + 0x19c], eax
00431d3c  mov        dword ptr [esi + 0x110], eax
00431d42  mov        dword ptr [esi + 0x1e4], eax
00431d48  mov        dword ptr [esi + 0x1e0], edx
00431d4e  mov        dword ptr [esi + 0x118], ecx
00431d54  call       0x477420

// block 00431d59
00431d59  or         dword ptr [esi], 2
00431d5c  add        esp, 0xc
00431d5f  mov        dword ptr [0x4b4510], esi
00431d65  mov        eax, esi
00431d67  ret        
