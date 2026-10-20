
// block 0040fb80
0040fb80  push       esi
0040fb81  mov        esi, dword ptr [0x4b43d4]
0040fb87  test       esi, esi
0040fb89  je         0x40fb9b

// block 0040fb8b
0040fb8b  mov        eax, esi
0040fb8d  call       0x40fa30

// block 0040fb92
0040fb92  push       esi
0040fb93  call       0x46ca4f

// block 0040fb98
0040fb98  add        esp, 4

// block 0040fb9b
0040fb9b  pop        esi
0040fb9c  ret        
