
// block 0044b5c0
0044b5c0  cmp        dword ptr [eax + 0x10], 3
0044b5c4  jne        0x44b5d2

// block 0044b5c6
0044b5c6  cmp        dword ptr [eax + 0x14], 0
0044b5ca  jl         0x44b5d2

// block 0044b5cc
0044b5cc  mov        eax, 1
0044b5d1  ret        

// block 0044b5d2
0044b5d2  xor        eax, eax
0044b5d4  ret        
