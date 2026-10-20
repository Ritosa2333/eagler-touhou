
// block 0040daa0
0040daa0  push       esi
0040daa1  push       0xc0
0040daa6  call       0x46c9ea

// block 0040daab
0040daab  mov        esi, eax
0040daad  add        esp, 4
0040dab0  test       esi, esi
0040dab2  je         0x40dad3

// block 0040dab4
0040dab4  and        dword ptr [esi + 0x34], 0xfffffffe
0040dab8  push       0xc0
0040dabd  push       0
0040dabf  push       esi
0040dac0  call       0x477420

// block 0040dac5
0040dac5  add        esp, 0xc
0040dac8  or         dword ptr [esi], 2
0040dacb  mov        dword ptr [0x4b43cc], esi
0040dad1  jmp        0x40dad5

// block 0040dad3
0040dad3  xor        esi, esi
