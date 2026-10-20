
// block 0040db20
0040db20  push       esi
0040db21  mov        esi, dword ptr [0x4b43cc]
0040db27  test       esi, esi
0040db29  je         0x40db3b

// block 0040db2b
0040db2b  mov        eax, esi
0040db2d  call       0x40d9c0

// block 0040db32
0040db32  push       esi
0040db33  call       0x46ca4f

// block 0040db38
0040db38  add        esp, 4

// block 0040db3b
0040db3b  pop        esi
0040db3c  ret        
