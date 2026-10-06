
// block 004da042
004da042  xor        eax, 0xd18951cd
004da047  aad        0x3d
004da049  jg         0x4da05a

// block 004da05c
004da05c  js         0x4da042

// block 004da05e
004da05e  fst        dword ptr gs:[ebx*4 + 0x5272e8d1]
004da066  jmp        0x4da052
