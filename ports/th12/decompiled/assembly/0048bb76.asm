
// block 0048bb76
0048bb76  mov        edi, edi
0048bb78  push       ebp
0048bb79  mov        ebp, esp
0048bb7b  cmp        dword ptr [0x4b40dc], 0
0048bb82  jne        0x48bb96

// block 0048bb84
0048bb84  mov        eax, dword ptr [ebp + 8]
0048bb87  mov        ecx, dword ptr [0x4adaa8]
0048bb8d  movzx      eax, word ptr [ecx + eax*2]
0048bb91  and        eax, 8
0048bb94  pop        ebp
0048bb95  ret        

// block 0048bb96
0048bb96  push       0
0048bb98  push       dword ptr [ebp + 8]
0048bb9b  call       0x48bb25

// block 0048bba0
0048bba0  pop        ecx
0048bba1  pop        ecx
0048bba2  pop        ebp
0048bba3  ret        
