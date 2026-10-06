
// block 0042f800
0042f800  mov        eax, dword ptr [esi + 0x478]
0042f806  test       eax, eax
0042f808  je         0x42f813

// block 0042f80a
0042f80a  push       eax
0042f80b  call       0x46c941

// block 0042f810
0042f810  add        esp, 4

// block 0042f813
0042f813  push       esi
0042f814  mov        dword ptr [esi + 0x478], 0
0042f81e  call       0x46ca4f

// block 0042f823
0042f823  add        esp, 4
0042f826  mov        eax, esi
0042f828  ret        
