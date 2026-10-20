
// block 0041fca0
0041fca0  push       esi
0041fca1  mov        esi, eax
0041fca3  push       esi
0041fca4  call       0x41fcc0

// block 0041fca9
0041fca9  test       eax, eax
0041fcab  je         0x41fcb4

// block 0041fcad
0041fcad  mov        eax, 1
0041fcb2  pop        esi
0041fcb3  ret        

// block 0041fcb4
0041fcb4  add        esi, 4
0041fcb7  call       0x464a80

// block 0041fcbc
0041fcbc  xor        eax, eax
0041fcbe  pop        esi
0041fcbf  ret        
