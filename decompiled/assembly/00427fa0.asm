
// block 00427fa0
00427fa0  push       esi
00427fa1  lea        esi, [edi + 0x10]
00427fa4  call       0x427ec0

// block 00427fa9
00427fa9  push       0x490
00427fae  push       0
00427fb0  push       edi
00427fb1  call       0x477420

// block 00427fb6
00427fb6  add        esp, 0xc
00427fb9  mov        dword ptr [edi + 0x46c], 0x10000
00427fc3  mov        dword ptr [0x4b44f4], edi
00427fc9  mov        eax, edi
00427fcb  pop        esi
00427fcc  ret        
