
// block 0048ba71
0048ba71  mov        edi, edi
0048ba73  push       ebp
0048ba74  mov        ebp, esp
0048ba76  cmp        dword ptr [0x4b40dc], 0
0048ba7d  jne        0x48ba91

// block 0048ba7f
0048ba7f  mov        eax, dword ptr [ebp + 8]
0048ba82  mov        ecx, dword ptr [0x4adaa8]
0048ba88  movzx      eax, word ptr [ecx + eax*2]
0048ba8c  and        eax, 4
0048ba8f  pop        ebp
0048ba90  ret        

// block 0048ba91
0048ba91  push       0
0048ba93  push       dword ptr [ebp + 8]
0048ba96  call       0x48ba20

// block 0048ba9b
0048ba9b  pop        ecx
0048ba9c  pop        ecx
0048ba9d  pop        ebp
0048ba9e  ret        
