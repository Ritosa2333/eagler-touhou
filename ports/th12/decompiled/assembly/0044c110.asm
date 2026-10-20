
// block 0044c110
0044c110  push       0x2f
0044c112  push       esi
0044c113  call       0x46d1a0

// block 0044c118
0044c118  add        esp, 8
0044c11b  test       eax, eax
0044c11d  je         0x44c121

// block 0044c11f
0044c11f  inc        eax
0044c120  ret        

// block 0044c121
0044c121  mov        eax, esi
0044c123  ret        
