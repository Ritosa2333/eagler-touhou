
// block 0048b9f2
0048b9f2  mov        edi, edi
0048b9f4  push       ebp
0048b9f5  mov        ebp, esp
0048b9f7  cmp        dword ptr [0x4b40dc], 0
0048b9fe  jne        0x48ba12

// block 0048ba00
0048ba00  mov        eax, dword ptr [ebp + 8]
0048ba03  mov        ecx, dword ptr [0x4adaa8]
0048ba09  movzx      eax, word ptr [ecx + eax*2]
0048ba0d  and        eax, 2
0048ba10  pop        ebp
0048ba11  ret        

// block 0048ba12
0048ba12  push       0
0048ba14  push       dword ptr [ebp + 8]
0048ba17  call       0x48b9a1

// block 0048ba1c
0048ba1c  pop        ecx
0048ba1d  pop        ecx
0048ba1e  pop        ebp
0048ba1f  ret        
