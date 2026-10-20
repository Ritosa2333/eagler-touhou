
// block 0044bf40
0044bf40  mov        eax, dword ptr [ecx + 4]
0044bf43  cmp        eax, -1
0044bf46  jne        0x44bf4b

// block 0044bf48
0044bf48  xor        eax, eax
0044bf4a  ret        

// block 0044bf4b
0044bf4b  push       1
0044bf4d  push       0
0044bf4f  push       0
0044bf51  push       eax
0044bf52  call       dword ptr [0x4980a0]

// block 0044bf58
0044bf58  ret        
