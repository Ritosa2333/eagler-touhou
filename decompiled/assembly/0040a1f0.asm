
// block 0040a1f0
0040a1f0  mov        eax, dword ptr [0x4b44e8]
0040a1f5  test       eax, eax
0040a1f7  je         0x40a20e

// block 0040a1f9
0040a1f9  mov        eax, dword ptr [eax + 0x60]
0040a1fc  mov        edx, eax
0040a1fe  shr        edx, 2
0040a201  or         edx, eax
0040a203  test       dl, 1
0040a206  je         0x40a20e

// block 0040a208
0040a208  mov        eax, 1
0040a20d  ret        
