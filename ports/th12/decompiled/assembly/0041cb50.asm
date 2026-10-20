
// block 0041cb50
0041cb50  push       esi
0041cb51  mov        esi, dword ptr [0x4b43e0]
0041cb57  test       esi, esi
0041cb59  je         0x41cb6b

// block 0041cb5b
0041cb5b  mov        eax, esi
0041cb5d  call       0x41ca70

// block 0041cb62
0041cb62  push       esi
0041cb63  call       0x46ca4f

// block 0041cb68
0041cb68  add        esp, 4

// block 0041cb6b
0041cb6b  pop        esi
0041cb6c  ret        
