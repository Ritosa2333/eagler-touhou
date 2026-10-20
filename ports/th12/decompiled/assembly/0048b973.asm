
// block 0048b973
0048b973  mov        edi, edi
0048b975  push       ebp
0048b976  mov        ebp, esp
0048b978  cmp        dword ptr [0x4b40dc], 0
0048b97f  jne        0x48b993

// block 0048b981
0048b981  mov        eax, dword ptr [ebp + 8]
0048b984  mov        ecx, dword ptr [0x4adaa8]
0048b98a  movzx      eax, word ptr [ecx + eax*2]
0048b98e  and        eax, 1
0048b991  pop        ebp
0048b992  ret        

// block 0048b993
0048b993  push       0
0048b995  push       dword ptr [ebp + 8]
0048b998  call       0x48b922

// block 0048b99d
0048b99d  pop        ecx
0048b99e  pop        ecx
0048b99f  pop        ebp
0048b9a0  ret        
