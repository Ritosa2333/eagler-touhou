
// block 0047c890
0047c890  push       esi
0047c891  mov        eax, dword ptr [esp + 0x14]
0047c895  or         eax, eax
0047c897  jne        0x47c8c1

// block 0047c899
0047c899  mov        ecx, dword ptr [esp + 0x10]
0047c89d  mov        eax, dword ptr [esp + 0xc]
0047c8a1  xor        edx, edx
0047c8a3  div        ecx
0047c8a5  mov        ebx, eax
0047c8a7  mov        eax, dword ptr [esp + 8]
0047c8ab  div        ecx
0047c8ad  mov        esi, eax
0047c8af  mov        eax, ebx
0047c8b1  mul        dword ptr [esp + 0x10]
0047c8b5  mov        ecx, eax
0047c8b7  mov        eax, esi
0047c8b9  mul        dword ptr [esp + 0x10]
0047c8bd  add        edx, ecx
0047c8bf  jmp        0x47c908

// block 0047c8c1
0047c8c1  mov        ecx, eax
0047c8c3  mov        ebx, dword ptr [esp + 0x10]
0047c8c7  mov        edx, dword ptr [esp + 0xc]
0047c8cb  mov        eax, dword ptr [esp + 8]

// block 0047c8cf
0047c8cf  shr        ecx, 1
0047c8d1  rcr        ebx, 1
0047c8d3  shr        edx, 1
0047c8d5  rcr        eax, 1
0047c8d7  or         ecx, ecx
0047c8d9  jne        0x47c8cf

// block 0047c8db
0047c8db  div        ebx
0047c8dd  mov        esi, eax
0047c8df  mul        dword ptr [esp + 0x14]
0047c8e3  mov        ecx, eax
0047c8e5  mov        eax, dword ptr [esp + 0x10]
0047c8e9  mul        esi
0047c8eb  add        edx, ecx
0047c8ed  jb         0x47c8fd

// block 0047c8ef
0047c8ef  cmp        edx, dword ptr [esp + 0xc]
0047c8f3  ja         0x47c8fd

// block 0047c8f5
0047c8f5  jb         0x47c906

// block 0047c8f7
0047c8f7  cmp        eax, dword ptr [esp + 8]
0047c8fb  jbe        0x47c906

// block 0047c8fd
0047c8fd  dec        esi
0047c8fe  sub        eax, dword ptr [esp + 0x10]
0047c902  sbb        edx, dword ptr [esp + 0x14]

// block 0047c906
0047c906  xor        ebx, ebx

// block 0047c908
0047c908  sub        eax, dword ptr [esp + 8]
0047c90c  sbb        edx, dword ptr [esp + 0xc]
0047c910  neg        edx
0047c912  neg        eax
0047c914  sbb        edx, 0
0047c917  mov        ecx, edx
0047c919  mov        edx, ebx
0047c91b  mov        ebx, ecx
0047c91d  mov        ecx, eax
0047c91f  mov        eax, esi
0047c921  pop        esi
0047c922  ret        0x10
