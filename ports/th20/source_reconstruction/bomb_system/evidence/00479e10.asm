00479e10 55                       push       ebp
00479e11 8bec                     mov        ebp, esp
00479e13 83ec28                   sub        esp, 0x28
00479e16 894dfc                   mov        dword ptr [ebp - 4], ecx
00479e19 8b45fc                   mov        eax, dword ptr [ebp - 4]
00479e1c 83b8d000000000           cmp        dword ptr [eax + 0xd0], 0
00479e23 0f854d010000             jne        0x479f76
00479e29 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479e2c 83b9a000000000           cmp        dword ptr [ecx + 0xa0], 0
00479e33 0f84f4000000             je         0x479f2d
00479e39 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479e3c 83c104                   add        ecx, 4
00479e3f e85c1ff9ff               call       0x40bda0
00479e44 51                       push       ecx
00479e45 f30f1000                 movss      xmm0, dword ptr [eax]
00479e49 f30f110424               movss      dword ptr [esp], xmm0
00479e4e 6a1b                     push       0x1b
00479e50 b930a85b00               mov        ecx, 0x5ba830
00479e55 e856d0faff               call       0x426eb0
00479e5a 6a00                     push       0
00479e5c e81ff0ffff               call       0x478e80
00479e61 83c404                   add        esp, 4
00479e64 8945f4                   mov        dword ptr [ebp - 0xc], eax
00479e67 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479e6a 83c104                   add        ecx, 4
00479e6d e82e1ff9ff               call       0x40bda0
00479e72 8945f8                   mov        dword ptr [ebp - 8], eax
00479e75 6a00                     push       0
00479e77 689f860100               push       0x1869f
00479e7c 6a00                     push       0
00479e7e 51                       push       ecx
00479e7f f30f100598cd5600         movss      xmm0, dword ptr [0x56cd98]
00479e87 f30f110424               movss      dword ptr [esp], xmm0
00479e8c 8b55f8                   mov        edx, dword ptr [ebp - 8]
00479e8f 52                       push       edx
00479e90 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00479e93 e8c8300000               call       0x47cf60
00479e98 6a00                     push       0
00479e9a e841f0ffff               call       0x478ee0
00479e9f 83c404                   add        esp, 4
00479ea2 8945ec                   mov        dword ptr [ebp - 0x14], eax
00479ea5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479ea8 83c104                   add        ecx, 4
00479eab e8f01ef9ff               call       0x40bda0
00479eb0 8945f0                   mov        dword ptr [ebp - 0x10], eax
00479eb3 6a01                     push       1
00479eb5 6a01                     push       1
00479eb7 51                       push       ecx
00479eb8 f30f100598cd5600         movss      xmm0, dword ptr [0x56cd98]
00479ec0 f30f110424               movss      dword ptr [esp], xmm0
00479ec5 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00479ec8 50                       push       eax
00479ec9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00479ecc e8ff0b0500               call       0x4caad0
00479ed1 90                       nop        
00479ed2 6a00                     push       0
00479ed4 e8e7efffff               call       0x478ec0
00479ed9 83c404                   add        esp, 4
00479edc 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00479edf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479ee2 83c104                   add        ecx, 4
00479ee5 e8b61ef9ff               call       0x40bda0
00479eea 8945e8                   mov        dword ptr [ebp - 0x18], eax
00479eed 6a64                     push       0x64
00479eef 6a0b                     push       0xb
00479ef1 51                       push       ecx
00479ef2 f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
00479efa f30f110424               movss      dword ptr [esp], xmm0
00479eff 51                       push       ecx
00479f00 f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
00479f08 f30f110424               movss      dword ptr [esp], xmm0
00479f0d 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00479f10 51                       push       ecx
00479f11 8d55dc                   lea        edx, [ebp - 0x24]
00479f14 52                       push       edx
00479f15 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00479f18 e8637c0400               call       0x4c1b80
00479f1d 8945e0                   mov        dword ptr [ebp - 0x20], eax
00479f20 8d45d8                   lea        eax, [ebp - 0x28]
00479f23 50                       push       eax
00479f24 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00479f27 e8e47c0400               call       0x4c1c10
00479f2c 90                       nop        
00479f2d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479f30 e80bf1ffff               call       0x479040
00479f35 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479f38 c781a000000000000000     mov        dword ptr [ecx + 0xa0], 0
00479f42 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479f45 81c1cc000000             add        ecx, 0xcc
00479f4b e8b023f9ff               call       0x40c300
00479f50 85c0                     test       eax, eax
00479f52 740f                     je         0x479f63
00479f54 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479f57 81c1cc000000             add        ecx, 0xcc
00479f5d e89e7f0400               call       0x4c1f00
00479f62 90                       nop        
00479f63 6a00                     push       0
00479f65 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479f68 81c1cc000000             add        ecx, 0xcc
00479f6e e82d78f9ff               call       0x4117a0
00479f73 90                       nop        
00479f74 eb09                     jmp        0x479f7f
00479f76 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479f79 e8525dfdff               call       0x44fcd0
00479f7e 90                       nop        
00479f7f 8be5                     mov        esp, ebp
00479f81 5d                       pop        ebp
00479f82 c3                       ret        
00479f83 cc                       int3       
00479f84 cc                       int3       
00479f85 cc                       int3       
00479f86 cc                       int3       
00479f87 cc                       int3       
00479f88 cc                       int3       
00479f89 cc                       int3       
00479f8a cc                       int3       
00479f8b cc                       int3       
00479f8c cc                       int3       
00479f8d cc                       int3       
00479f8e cc                       int3       
00479f8f cc                       int3       
