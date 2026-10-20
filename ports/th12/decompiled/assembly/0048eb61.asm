
// block 0048eb61
0048eb61  mov        edi, edi
0048eb63  push       ebp
0048eb64  mov        ebp, esp
0048eb66  mov        eax, dword ptr [ebp + 8]

// block 0048eb69
0048eb69  mov        cx, word ptr [eax]
0048eb6c  inc        eax
0048eb6d  inc        eax
0048eb6e  test       cx, cx
0048eb71  jne        0x48eb69

// block 0048eb73
0048eb73  sub        eax, dword ptr [ebp + 8]
0048eb76  sar        eax, 1
0048eb78  dec        eax
0048eb79  pop        ebp
0048eb7a  ret        
