
// block 00410595
00410595  push       esi
00410596  mov        esi, dword ptr [ecx + 0x478]
0041059c  push       ecx
0041059d  call       0x459a60

// block 004105a2
004105a2  mov        eax, dword ptr [esi + 0xc8]
004105a8  lea        edx, [esi + 0x80]
004105ae  push       eax
004105af  mov        ecx, esi
004105b1  call       0x45d5d0

// block 004105b6
004105b6  xor        eax, eax
004105b8  pop        esi
004105b9  ret        
