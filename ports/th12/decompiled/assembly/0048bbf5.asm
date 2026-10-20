
// block 0048bbf5
0048bbf5  mov        edi, edi
0048bbf7  push       ebp
0048bbf8  mov        ebp, esp
0048bbfa  cmp        dword ptr [0x4b40dc], 0
0048bc01  jne        0x48bc15

// block 0048bc03
0048bc03  mov        eax, dword ptr [ebp + 8]
0048bc06  mov        ecx, dword ptr [0x4adaa8]
0048bc0c  movzx      eax, word ptr [ecx + eax*2]
0048bc10  and        eax, 0x10
0048bc13  pop        ebp
0048bc14  ret        

// block 0048bc15
0048bc15  push       0
0048bc17  push       dword ptr [ebp + 8]
0048bc1a  call       0x48bba4

// block 0048bc1f
0048bc1f  pop        ecx
0048bc20  pop        ecx
0048bc21  pop        ebp
0048bc22  ret        
