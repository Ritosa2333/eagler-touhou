
// block 00423340
00423340  push       0x88
00423345  push       0
00423347  push       esi
00423348  call       0x477420

// block 0042334d
0042334d  fld1       
0042334f  mov        dword ptr [esi + 0xc], esi
00423352  mov        dword ptr [esi + 0x10], 0
00423359  mov        dword ptr [esi + 0x14], 0
00423360  fstp       dword ptr [esi + 0x7c]
00423363  or         eax, 0xffffffff
00423366  mov        dword ptr [esi + 0x84], eax
0042336c  mov        dword ptr [esi + 0x70], eax
0042336f  add        esp, 0xc
00423372  mov        dword ptr [esi + 0x6c], 0x12c
00423379  mov        eax, esi
0042337b  ret        
