
// block 00425c00
00425c00  push       esi
00425c01  mov        esi, dword ptr [0x4b44f0]
00425c07  test       esi, esi
00425c09  je         0x425c1a

// block 00425c0b
00425c0b  push       esi
00425c0c  call       0x425a00

// block 00425c11
00425c11  push       esi
00425c12  call       0x46ca4f

// block 00425c17
00425c17  add        esp, 4

// block 00425c1a
00425c1a  pop        esi
00425c1b  ret        
