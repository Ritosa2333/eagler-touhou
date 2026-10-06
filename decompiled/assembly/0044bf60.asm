
// block 0044bf60
0044bf60  mov        eax, dword ptr [ecx + 4]
0044bf63  cmp        eax, -1
0044bf66  jne        0x44bf6b

// block 0044bf68
0044bf68  xor        eax, eax
0044bf6a  ret        

// block 0044bf6b
0044bf6b  push       0
0044bf6d  push       eax
0044bf6e  call       dword ptr [0x4980b0]

// block 0044bf74
0044bf74  ret        
