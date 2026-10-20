
// block 0040a130
0040a130  push       esi
0040a131  xor        esi, esi

// block 0040a133
0040a133  mov        ecx, esi
0040a135  mov        eax, edi
0040a137  call       0x40a150

// block 0040a13c
0040a13c  inc        esi
0040a13d  cmp        esi, 6
0040a140  jl         0x40a133

// block 0040a142
0040a142  mov        eax, 1
0040a147  pop        esi
0040a148  ret        
