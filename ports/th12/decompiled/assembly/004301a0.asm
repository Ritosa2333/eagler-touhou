
// block 004301a0
004301a0  mov        al, byte ptr [0x4ceacb]
004301a5  cmp        al, 2
004301a7  je         0x4301ad

// block 004301a9
004301a9  cmp        al, 1
004301ab  jne        0x4301b0

// block 004301ad
004301ad  xor        eax, eax
004301af  ret        

// block 004301b0
004301b0  or         eax, 0xffffffff
004301b3  ret        
