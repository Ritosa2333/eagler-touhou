
// block 00468d90
00468d90  push       ecx
00468d91  push       ebx
00468d92  push       ebp
00468d93  push       esi
00468d94  lea        esi, [edi + 0x1030]
00468d9a  mov        ebp, 1
00468d9f  test       esi, esi
00468da1  je         0x468e05

// block 00468da3
00468da3  mov        eax, dword ptr [esi]
00468da5  fld        dword ptr [esp + 0x14]
00468da9  mov        ebx, dword ptr [esi + 4]
00468dac  push       ecx
00468dad  fstp       dword ptr [esp]
00468db0  mov        dword ptr [edi + 4], eax
00468db3  call       0x467170

// block 00468db8
00468db8  test       ebp, ebp
00468dba  je         0x468dc4

// block 00468dbc
00468dbc  test       eax, eax
00468dbe  jne        0x468e14

// block 00468dc0
00468dc0  xor        ebp, ebp
00468dc2  jmp        0x468dff

// block 00468dc4
00468dc4  test       eax, eax
00468dc6  je         0x468dff

// block 00468dc8
00468dc8  mov        eax, dword ptr [edi + 4]
00468dcb  push       eax
00468dcc  call       0x46ca4f

// block 00468dd1
00468dd1  mov        eax, dword ptr [esi + 4]
00468dd4  xor        ecx, ecx
00468dd6  add        esp, 4
00468dd9  cmp        eax, ecx
00468ddb  je         0x468de3

// block 00468ddd
00468ddd  mov        edx, dword ptr [esi + 8]
00468de0  mov        dword ptr [eax + 8], edx

// block 00468de3
00468de3  mov        eax, dword ptr [esi + 8]
00468de6  cmp        eax, ecx
00468de8  je         0x468df0

// block 00468dea
00468dea  mov        edx, dword ptr [esi + 4]
00468ded  mov        dword ptr [eax + 4], edx

// block 00468df0
00468df0  push       esi
00468df1  mov        dword ptr [esi + 4], ecx
00468df4  mov        dword ptr [esi + 8], ecx
00468df7  call       0x46ca4f

// block 00468dfc
00468dfc  add        esp, 4

// block 00468dff
00468dff  mov        esi, ebx
00468e01  test       ebx, ebx
00468e03  jne        0x468da3

// block 00468e05
00468e05  lea        eax, [edi + 8]
00468e08  mov        dword ptr [edi + 4], eax
00468e0b  xor        eax, eax
00468e0d  pop        esi
00468e0e  pop        ebp
00468e0f  pop        ebx
00468e10  pop        ecx
00468e11  ret        4

// block 00468e14
00468e14  pop        esi
00468e15  pop        ebp
00468e16  or         eax, 0xffffffff
00468e19  pop        ebx
00468e1a  pop        ecx
00468e1b  ret        4
