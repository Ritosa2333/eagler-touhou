
// block 0048bd85
0048bd85  mov        edi, edi
0048bd87  push       ebp
0048bd88  mov        ebp, esp
0048bd8a  cmp        dword ptr [0x4b40dc], 0
0048bd91  jne        0x48bda7

// block 0048bd93
0048bd93  mov        eax, dword ptr [ebp + 8]
0048bd96  mov        ecx, dword ptr [0x4adaa8]
0048bd9c  movzx      eax, word ptr [ecx + eax*2]
0048bda0  and        eax, 0x117
0048bda5  pop        ebp
0048bda6  ret        

// block 0048bda7
0048bda7  push       0
0048bda9  push       dword ptr [ebp + 8]
0048bdac  call       0x48bd2f

// block 0048bdb1
0048bdb1  pop        ecx
0048bdb2  pop        ecx
0048bdb3  pop        ebp
0048bdb4  ret        
