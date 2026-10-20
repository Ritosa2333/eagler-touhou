
// block 0048b8f2
0048b8f2  mov        edi, edi
0048b8f4  push       ebp
0048b8f5  mov        ebp, esp
0048b8f7  cmp        dword ptr [0x4b40dc], 0
0048b8fe  jne        0x48b914

// block 0048b900
0048b900  mov        eax, dword ptr [ebp + 8]
0048b903  mov        ecx, dword ptr [0x4adaa8]
0048b909  movzx      eax, word ptr [ecx + eax*2]
0048b90d  and        eax, 0x103
0048b912  pop        ebp
0048b913  ret        

// block 0048b914
0048b914  push       0
0048b916  push       dword ptr [ebp + 8]
0048b919  call       0x48b89c

// block 0048b91e
0048b91e  pop        ecx
0048b91f  pop        ecx
0048b920  pop        ebp
0048b921  ret        
