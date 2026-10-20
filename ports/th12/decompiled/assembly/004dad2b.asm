
// block 004dad2b
004dad2b  push       ebp
004dad2c  sbb        eax, 0x1287695c
004dad31  push       edx

// block 004dad32
004dad32  repe scasb al, byte ptr es:[edi]
