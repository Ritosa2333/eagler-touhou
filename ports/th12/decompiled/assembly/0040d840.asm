
// block 0040d840
0040d840  xor        eax, eax
0040d842  xor        ecx, ecx
0040d844  push       esi

// block 0040d845
0040d845  movsx      esi, byte ptr [ecx + 0x4af208]
0040d84c  cmp        esi, edx
0040d84e  jne        0x40d851

// block 0040d850
0040d850  inc        eax

// block 0040d851
0040d851  inc        ecx
0040d852  cmp        ecx, 0x71
0040d855  jl         0x40d845

// block 0040d857
0040d857  pop        esi
0040d858  ret        
