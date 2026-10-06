
// block 00478dec
00478dec  mov        edi, edi
00478dee  push       ebp
00478def  mov        ebp, esp
00478df1  push       ebx

// block 00478df2
00478df2  mov        edx, dword ptr [ebp + 8]
00478df5  inc        dword ptr [esi]
00478df7  call       0x478dc3

// block 00478dfc
00478dfc  mov        ebx, eax
00478dfe  cmp        ebx, -1
00478e01  je         0x478e11

// block 00478e03
00478e03  movzx      eax, bl
00478e06  push       eax
00478e07  call       0x48bb76

// block 00478e0c
00478e0c  pop        ecx
00478e0d  test       eax, eax
00478e0f  jne        0x478df2

// block 00478e11
00478e11  mov        eax, ebx
00478e13  pop        ebx
00478e14  pop        ebp
00478e15  ret        
