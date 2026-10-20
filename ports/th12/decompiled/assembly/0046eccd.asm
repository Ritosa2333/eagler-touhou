
// block 0046eccd
0046eccd  cmp        dword ptr [0x4b3c04], 0
0046ecd4  jne        0x46ecd9

// block 0046ecd6
0046ecd6  xor        eax, eax
0046ecd8  ret        

// block 0046ecd9
0046ecd9  mov        eax, dword ptr [0x4d6458]
0046ecde  sub        eax, 3
0046ece1  neg        eax
0046ece3  sbb        eax, eax
0046ece5  not        eax
0046ece7  and        eax, dword ptr [0x4d6448]
0046eced  ret        
