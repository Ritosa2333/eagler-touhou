
// block 0048ac71
0048ac71  mov        edi, edi
0048ac73  push       esi
0048ac74  push       0x30000
0048ac79  push       0x10000
0048ac7e  xor        esi, esi
0048ac80  push       esi
0048ac81  call       0x48e1cf

// block 0048ac86
0048ac86  add        esp, 0xc
0048ac89  test       eax, eax
0048ac8b  je         0x48ac9a

// block 0048ac8d
0048ac8d  push       esi
0048ac8e  push       esi
0048ac8f  push       esi
0048ac90  push       esi
0048ac91  push       esi
0048ac92  call       0x470dc6

// block 0048ac97
0048ac97  add        esp, 0x14

// block 0048ac9a
0048ac9a  pop        esi
0048ac9b  ret        
