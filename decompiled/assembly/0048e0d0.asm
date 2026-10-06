
// block 0048e0d0
0048e0d0  push       edi
0048e0d1  push       esi
0048e0d2  push       ebp
0048e0d3  xor        edi, edi
0048e0d5  xor        ebp, ebp
0048e0d7  mov        eax, dword ptr [esp + 0x14]
0048e0db  or         eax, eax
0048e0dd  jge        0x48e0f4

// block 0048e0df
0048e0df  inc        edi
0048e0e0  inc        ebp
0048e0e1  mov        edx, dword ptr [esp + 0x10]
0048e0e5  neg        eax
0048e0e7  neg        edx
0048e0e9  sbb        eax, 0
0048e0ec  mov        dword ptr [esp + 0x14], eax
0048e0f0  mov        dword ptr [esp + 0x10], edx

// block 0048e0f4
0048e0f4  mov        eax, dword ptr [esp + 0x1c]
0048e0f8  or         eax, eax
0048e0fa  jge        0x48e110

// block 0048e0fc
0048e0fc  inc        edi
0048e0fd  mov        edx, dword ptr [esp + 0x18]
0048e101  neg        eax
0048e103  neg        edx
0048e105  sbb        eax, 0
0048e108  mov        dword ptr [esp + 0x1c], eax
0048e10c  mov        dword ptr [esp + 0x18], edx

// block 0048e110
0048e110  or         eax, eax
0048e112  jne        0x48e13c

// block 0048e114
0048e114  mov        ecx, dword ptr [esp + 0x18]
0048e118  mov        eax, dword ptr [esp + 0x14]
0048e11c  xor        edx, edx
0048e11e  div        ecx
0048e120  mov        ebx, eax
0048e122  mov        eax, dword ptr [esp + 0x10]
0048e126  div        ecx
0048e128  mov        esi, eax
0048e12a  mov        eax, ebx
0048e12c  mul        dword ptr [esp + 0x18]
0048e130  mov        ecx, eax
0048e132  mov        eax, esi
0048e134  mul        dword ptr [esp + 0x18]
0048e138  add        edx, ecx
0048e13a  jmp        0x48e183

// block 0048e13c
0048e13c  mov        ebx, eax
0048e13e  mov        ecx, dword ptr [esp + 0x18]
0048e142  mov        edx, dword ptr [esp + 0x14]
0048e146  mov        eax, dword ptr [esp + 0x10]

// block 0048e14a
0048e14a  shr        ebx, 1
0048e14c  rcr        ecx, 1
0048e14e  shr        edx, 1
0048e150  rcr        eax, 1
0048e152  or         ebx, ebx
0048e154  jne        0x48e14a

// block 0048e156
0048e156  div        ecx
0048e158  mov        esi, eax
0048e15a  mul        dword ptr [esp + 0x1c]
0048e15e  mov        ecx, eax
0048e160  mov        eax, dword ptr [esp + 0x18]
0048e164  mul        esi
0048e166  add        edx, ecx
0048e168  jb         0x48e178

// block 0048e16a
0048e16a  cmp        edx, dword ptr [esp + 0x14]
0048e16e  ja         0x48e178

// block 0048e170
0048e170  jb         0x48e181

// block 0048e172
0048e172  cmp        eax, dword ptr [esp + 0x10]
0048e176  jbe        0x48e181

// block 0048e178
0048e178  dec        esi
0048e179  sub        eax, dword ptr [esp + 0x18]
0048e17d  sbb        edx, dword ptr [esp + 0x1c]

// block 0048e181
0048e181  xor        ebx, ebx

// block 0048e183
0048e183  sub        eax, dword ptr [esp + 0x10]
0048e187  sbb        edx, dword ptr [esp + 0x14]
0048e18b  dec        ebp
0048e18c  jns        0x48e195

// block 0048e18e
0048e18e  neg        edx
0048e190  neg        eax
0048e192  sbb        edx, 0

// block 0048e195
0048e195  mov        ecx, edx
0048e197  mov        edx, ebx
0048e199  mov        ebx, ecx
0048e19b  mov        ecx, eax
0048e19d  mov        eax, esi
0048e19f  dec        edi
0048e1a0  jne        0x48e1a9

// block 0048e1a2
0048e1a2  neg        edx
0048e1a4  neg        eax
0048e1a6  sbb        edx, 0

// block 0048e1a9
0048e1a9  pop        ebp
0048e1aa  pop        esi
0048e1ab  pop        edi
0048e1ac  ret        0x10
