
// block 0048baf5
0048baf5  mov        edi, edi
0048baf7  push       ebp
0048baf8  mov        ebp, esp
0048bafa  cmp        dword ptr [0x4b40dc], 0
0048bb01  jne        0x48bb17

// block 0048bb03
0048bb03  mov        eax, dword ptr [ebp + 8]
0048bb06  mov        ecx, dword ptr [0x4adaa8]
0048bb0c  movzx      eax, word ptr [ecx + eax*2]
0048bb10  and        eax, 0x80
0048bb15  pop        ebp
0048bb16  ret        

// block 0048bb17
0048bb17  push       0
0048bb19  push       dword ptr [ebp + 8]
0048bb1c  call       0x48ba9f

// block 0048bb21
0048bb21  pop        ecx
0048bb22  pop        ecx
0048bb23  pop        ebp
0048bb24  ret        
