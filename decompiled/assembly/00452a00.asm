
// block 00452a00
00452a00  test       esi, esi
00452a02  je         0x452a12

// block 00452a04
00452a04  mov        eax, esi
00452a06  call       0x452f00

// block 00452a0b
00452a0b  push       esi
00452a0c  call       0x46ca4f

// block 00452a11
00452a11  pop        ecx

// block 00452a12
00452a12  ret        
