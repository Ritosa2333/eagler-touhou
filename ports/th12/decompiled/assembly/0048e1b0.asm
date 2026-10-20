
// block 0048e1b0
0048e1b0  cmp        cl, 0x40
0048e1b3  jae        0x48e1ca

// block 0048e1b5
0048e1b5  cmp        cl, 0x20
0048e1b8  jae        0x48e1c0

// block 0048e1ba
0048e1ba  shrd       eax, edx, cl
0048e1bd  shr        edx, cl
0048e1bf  ret        

// block 0048e1c0
0048e1c0  mov        eax, edx
0048e1c2  xor        edx, edx
0048e1c4  and        cl, 0x1f
0048e1c7  shr        eax, cl
0048e1c9  ret        

// block 0048e1ca
0048e1ca  xor        eax, eax
0048e1cc  xor        edx, edx
0048e1ce  ret        
