
// block 0044d2b0
0044d2b0  push       esi
0044d2b1  mov        esi, ecx
0044d2b3  mov        dword ptr [esi], 0x49cd00
0044d2b9  call       0x46e086

// block 0044d2be
0044d2be  test       byte ptr [esp + 8], 1
0044d2c3  je         0x44d2ce

// block 0044d2c5
0044d2c5  push       esi
0044d2c6  call       0x46ca4f

// block 0044d2cb
0044d2cb  add        esp, 4

// block 0044d2ce
0044d2ce  mov        eax, esi
0044d2d0  pop        esi
0044d2d1  ret        4
