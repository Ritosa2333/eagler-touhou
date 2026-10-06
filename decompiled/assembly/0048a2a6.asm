
// block 0048a2a6
0048a2a6  mov        edi, edi
0048a2a8  push       esi
0048a2a9  mov        esi, eax
0048a2ab  test       edi, edi
0048a2ad  je         0x48a2c3

// block 0048a2af
0048a2af  push       esi
0048a2b0  call       0x475860

// block 0048a2b5
0048a2b5  inc        eax
0048a2b6  push       eax
0048a2b7  push       esi
0048a2b8  add        esi, edi
0048a2ba  push       esi
0048a2bb  call       0x47a6a0

// block 0048a2c0
0048a2c0  add        esp, 0x10

// block 0048a2c3
0048a2c3  pop        esi
0048a2c4  ret        
