
// block 0040e8b0
0040e8b0  push       esi
0040e8b1  mov        esi, dword ptr [0x4b44f0]
0040e8b7  push       0x666fc0
0040e8bc  lea        eax, [esi + 0x14]
0040e8bf  push       0
0040e8c1  push       eax
0040e8c2  call       0x477420

// block 0040e8c7
0040e8c7  add        esp, 0xc
0040e8ca  mov        dword ptr [esi + 0x666fd8], 0
0040e8d4  mov        dword ptr [esi + 0x666fe0], 0
0040e8de  pop        esi
0040e8df  ret        
