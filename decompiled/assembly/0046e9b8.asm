
// block 0046e9b8
0046e9b8  mov        edi, edi
0046e9ba  push       ebp
0046e9bb  mov        ebp, esp
0046e9bd  call       0x4753ee

// block 0046e9c2
0046e9c2  test       eax, eax
0046e9c4  jne        0x46e9cb

// block 0046e9c6
0046e9c6  push       0xc
0046e9c8  pop        eax
0046e9c9  pop        ebp
0046e9ca  ret        

// block 0046e9cb
0046e9cb  call       0x46e96f

// block 0046e9d0
0046e9d0  mov        ecx, dword ptr [ebp + 8]
0046e9d3  mov        dword ptr [eax], ecx
0046e9d5  xor        eax, eax
0046e9d7  pop        ebp
0046e9d8  ret        
