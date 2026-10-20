
// block 0041cad0
0041cad0  push       esi
0041cad1  push       0x8c
0041cad6  call       0x46c9ea

// block 0041cadb
0041cadb  mov        esi, eax
0041cadd  add        esp, 4
0041cae0  test       esi, esi
0041cae2  je         0x41caff

// block 0041cae4
0041cae4  push       0x8c
0041cae9  push       0
0041caeb  push       esi
0041caec  call       0x477420

// block 0041caf1
0041caf1  add        esp, 0xc
0041caf4  or         dword ptr [esi], 2
0041caf7  mov        dword ptr [0x4b43e0], esi
0041cafd  jmp        0x41cb01

// block 0041caff
0041caff  xor        esi, esi
