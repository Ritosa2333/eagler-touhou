
// block 0048bcff
0048bcff  mov        edi, edi
0048bd01  push       ebp
0048bd02  mov        ebp, esp
0048bd04  cmp        dword ptr [0x4b40dc], 0
0048bd0b  jne        0x48bd21

// block 0048bd0d
0048bd0d  mov        eax, dword ptr [ebp + 8]
0048bd10  mov        ecx, dword ptr [0x4adaa8]
0048bd16  movzx      eax, word ptr [ecx + eax*2]
0048bd1a  and        eax, 0x157
0048bd1f  pop        ebp
0048bd20  ret        

// block 0048bd21
0048bd21  push       0
0048bd23  push       dword ptr [ebp + 8]
0048bd26  call       0x48bca9

// block 0048bd2b
0048bd2b  pop        ecx
0048bd2c  pop        ecx
0048bd2d  pop        ebp
0048bd2e  ret        
