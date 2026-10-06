
// block 0040e940
0040e940  push       ecx
0040e941  push       ebp
0040e942  push       esi
0040e943  push       edi
0040e944  mov        edi, dword ptr [0x4ce8cc]
0040e94a  lea        esi, [edi + 0x4b50e0]
0040e950  mov        dword ptr [esp + 0xc], 4
0040e958  mov        ebp, 0x10000000
0040e95d  lea        ecx, [ecx]

// block 0040e960
0040e960  mov        eax, dword ptr [edi + 0x8856b8]
0040e966  mov        edx, dword ptr [esi]
0040e968  test       eax, eax
0040e96a  je         0x40e989

// block 0040e96c
0040e96c  lea        esp, [esp]

// block 0040e970
0040e970  mov        ecx, dword ptr [eax + 4]
0040e973  mov        eax, dword ptr [eax]
0040e975  cmp        dword ptr [eax + 0x3f8], edx
0040e97b  jne        0x40e983

// block 0040e97d
0040e97d  or         dword ptr [eax + 0x47c], ebp

// block 0040e983
0040e983  mov        eax, ecx
0040e985  test       ecx, ecx
0040e987  jne        0x40e970

// block 0040e989
0040e989  mov        eax, dword ptr [edi + 0x8856c0]
0040e98f  test       eax, eax
0040e991  je         0x40e9ac

// block 0040e993
0040e993  mov        ecx, dword ptr [eax + 4]
0040e996  mov        eax, dword ptr [eax]
0040e998  cmp        dword ptr [eax + 0x3f8], edx
0040e99e  jne        0x40e9a6

// block 0040e9a0
0040e9a0  or         dword ptr [eax + 0x47c], ebp

// block 0040e9a6
0040e9a6  mov        eax, ecx
0040e9a8  test       ecx, ecx
0040e9aa  jne        0x40e993

// block 0040e9ac
0040e9ac  add        esi, 4
0040e9af  sub        dword ptr [esp + 0xc], 1
0040e9b4  jne        0x40e960

// block 0040e9b6
0040e9b6  mov        edi, dword ptr [esp + 0x14]
0040e9ba  mov        eax, dword ptr [edi + 0x68]
0040e9bd  test       eax, eax
0040e9bf  je         0x40e9db

// block 0040e9c1
0040e9c1  mov        esi, dword ptr [eax + 4]
0040e9c4  mov        eax, dword ptr [eax]
0040e9c6  test       eax, eax
0040e9c8  je         0x40e9d5

// block 0040e9ca
0040e9ca  mov        edx, dword ptr [eax]
0040e9cc  mov        ecx, eax
0040e9ce  mov        eax, dword ptr [edx + 0x14]
0040e9d1  push       1
0040e9d3  call       eax

// block 0040e9d5
0040e9d5  mov        eax, esi
0040e9d7  test       esi, esi
0040e9d9  jne        0x40e9c1

// block 0040e9db
0040e9db  mov        dword ptr [edi + 0x74], 0
0040e9e2  mov        dword ptr [edi + 0x1c], 0
0040e9e9  mov        eax, dword ptr [edi + 8]
0040e9ec  mov        ecx, 0xfffffffd
0040e9f1  test       eax, eax
0040e9f3  je         0x40e9f8

// block 0040e9f5
0040e9f5  and        dword ptr [eax + 4], ecx

// block 0040e9f8
0040e9f8  mov        eax, dword ptr [edi + 0xc]
0040e9fb  pop        edi
0040e9fc  pop        esi
0040e9fd  pop        ebp
0040e9fe  test       eax, eax
0040ea00  je         0x40ea05

// block 0040ea02
0040ea02  and        dword ptr [eax + 4], ecx

// block 0040ea05
0040ea05  pop        ecx
0040ea06  ret        4
