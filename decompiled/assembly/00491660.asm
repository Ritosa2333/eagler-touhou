
// block 00491660
00491660  mov        edi, edi
00491662  push       ebp
00491663  mov        ebp, esp
00491665  push       esi
00491666  mov        esi, ecx
00491668  mov        dword ptr [esi], 0x49f3a8
0049166e  call       0x49155a

// block 00491673
00491673  test       byte ptr [ebp + 8], 1
00491677  je         0x491680

// block 00491679
00491679  push       esi
0049167a  call       0x46ca4f

// block 0049167f
0049167f  pop        ecx

// block 00491680
00491680  mov        eax, esi
00491682  pop        esi
00491683  pop        ebp
00491684  ret        4
