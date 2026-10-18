00478c00 55                       push       ebp
00478c01 8bec                     mov        ebp, esp
00478c03 83ec3c                   sub        esp, 0x3c
00478c06 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00478c0b 33c5                     xor        eax, ebp
00478c0d 8945fc                   mov        dword ptr [ebp - 4], eax
00478c10 894dec                   mov        dword ptr [ebp - 0x14], ecx
00478c13 8d4df0                   lea        ecx, [ebp - 0x10]
00478c16 e8f5a1faff               call       0x422e10
00478c1b 6a00                     push       0
00478c1d e80e7cfeff               call       0x460830
00478c22 83c404                   add        esp, 4
00478c25 8bc8                     mov        ecx, eax
00478c27 e8247cfeff               call       0x460850
00478c2c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00478c2f 81c1b8000000             add        ecx, 0xb8
00478c35 8b10                     mov        edx, dword ptr [eax]
00478c37 8911                     mov        dword ptr [ecx], edx
00478c39 8b5004                   mov        edx, dword ptr [eax + 4]
00478c3c 895104                   mov        dword ptr [ecx + 4], edx
00478c3f 8b4008                   mov        eax, dword ptr [eax + 8]
00478c42 894108                   mov        dword ptr [ecx + 8], eax
00478c45 f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
00478c4d f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00478c55 51                       push       ecx
00478c56 f30f110424               movss      dword ptr [esp], xmm0
00478c5b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00478c5e 81c1c4000000             add        ecx, 0xc4
00478c64 e8b7a2fdff               call       0x452f20
00478c69 6a00                     push       0
00478c6b 6a31                     push       0x31
00478c6d b930a85b00               mov        ecx, 0x5ba830
00478c72 e8f9e0faff               call       0x426d70
00478c77 90                       nop        
00478c78 6a00                     push       0
00478c7a e8b17bfeff               call       0x460830
00478c7f 83c404                   add        esp, 4
00478c82 8bc8                     mov        ecx, eax
00478c84 e8878bfaff               call       0x421810
00478c89 8945e8                   mov        dword ptr [ebp - 0x18], eax
00478c8c 6a00                     push       0
00478c8e 6aff                     push       -1
00478c90 51                       push       ecx
00478c91 0f57c0                   xorps      xmm0, xmm0
00478c94 f30f110424               movss      dword ptr [esp], xmm0
00478c99 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00478c9c 81c1b8000000             add        ecx, 0xb8
00478ca2 51                       push       ecx
00478ca3 6a33                     push       0x33
00478ca5 6824fb5600               push       0x56fb24
00478caa 8d55c8                   lea        edx, [ebp - 0x38]
00478cad 52                       push       edx
00478cae 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00478cb1 e82a040000               call       0x4790e0
00478cb6 8b00                     mov        eax, dword ptr [eax]
00478cb8 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00478cbb 8981c8000000             mov        dword ptr [ecx + 0xc8], eax
00478cc1 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00478cc4 81c2c8000000             add        edx, 0xc8
00478cca 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00478ccd 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00478cd0 50                       push       eax
00478cd1 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00478cd4 e8777bfdff               call       0x450850
00478cd9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00478cdc 81c1c8000000             add        ecx, 0xc8
00478ce2 894de0                   mov        dword ptr [ebp - 0x20], ecx
00478ce5 6880834700               push       0x478380
00478cea 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00478ced e80e64fdff               call       0x44f100
00478cf2 90                       nop        
00478cf3 6a00                     push       0
00478cf5 e8367bfeff               call       0x460830
00478cfa 83c404                   add        esp, 4
00478cfd 8bc8                     mov        ecx, eax
00478cff e80c8bfaff               call       0x421810
00478d04 8945dc                   mov        dword ptr [ebp - 0x24], eax
00478d07 6a00                     push       0
00478d09 6aff                     push       -1
00478d0b 51                       push       ecx
00478d0c 0f57c0                   xorps      xmm0, xmm0
00478d0f f30f110424               movss      dword ptr [esp], xmm0
00478d14 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00478d17 81c2b8000000             add        edx, 0xb8
00478d1d 52                       push       edx
00478d1e 6a41                     push       0x41
00478d20 6824fb5600               push       0x56fb24
00478d25 8d45c4                   lea        eax, [ebp - 0x3c]
00478d28 50                       push       eax
00478d29 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00478d2c e8af030000               call       0x4790e0
00478d31 8b08                     mov        ecx, dword ptr [eax]
00478d33 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00478d36 894a74                   mov        dword ptr [edx + 0x74], ecx
00478d39 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00478d3c 83c074                   add        eax, 0x74
00478d3f 8945d8                   mov        dword ptr [ebp - 0x28], eax
00478d42 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00478d45 51                       push       ecx
00478d46 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00478d49 e8027bfdff               call       0x450850
00478d4e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00478d51 83c274                   add        edx, 0x74
00478d54 8955d4                   mov        dword ptr [ebp - 0x2c], edx
00478d57 6880834700               push       0x478380
00478d5c 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00478d5f e89c63fdff               call       0x44f100
00478d64 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00478d67 05b8000000               add        eax, 0xb8
00478d6c 8b08                     mov        ecx, dword ptr [eax]
00478d6e 894df0                   mov        dword ptr [ebp - 0x10], ecx
00478d71 8b5004                   mov        edx, dword ptr [eax + 4]
00478d74 8955f4                   mov        dword ptr [ebp - 0xc], edx
00478d77 8b4008                   mov        eax, dword ptr [eax + 8]
00478d7a 8945f8                   mov        dword ptr [ebp - 8], eax
00478d7d 6a00                     push       0
00478d7f e81c010000               call       0x478ea0
00478d84 83c404                   add        esp, 4
00478d87 8bc8                     mov        ecx, eax
00478d89 e822ee0000               call       0x487bb0
00478d8e 6a00                     push       0
00478d90 e89b7afeff               call       0x460830
00478d95 83c404                   add        esp, 4
00478d98 8945d0                   mov        dword ptr [ebp - 0x30], eax
00478d9b 6a78                     push       0x78
00478d9d 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478da0 e8bb020000               call       0x479060
00478da5 6a00                     push       0
00478da7 e8b4f2ffff               call       0x478060
00478dac 83c404                   add        esp, 4
00478daf 8945cc                   mov        dword ptr [ebp - 0x34], eax
00478db2 6a01                     push       1
00478db4 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00478db7 e884f1ffff               call       0x477f40
00478dbc 6a1e                     push       0x1e
00478dbe 68f0000000               push       0xf0
00478dc3 6a3c                     push       0x3c
00478dc5 6a03                     push       3
00478dc7 6a08                     push       8
00478dc9 e8c2c2faff               call       0x425090
00478dce 83c414                   add        esp, 0x14
00478dd1 6a00                     push       0
00478dd3 e8587afeff               call       0x460830
00478dd8 83c404                   add        esp, 4
00478ddb 8bc8                     mov        ecx, eax
00478ddd e8be020000               call       0x4790a0
00478de2 33c0                     xor        eax, eax
00478de4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478de7 33cd                     xor        ecx, ebp
00478de9 e8009c0c00               call       0x5429ee
00478dee 8be5                     mov        esp, ebp
00478df0 5d                       pop        ebp
00478df1 c20400                   ret        4
00478df4 cc                       int3       
00478df5 cc                       int3       
00478df6 cc                       int3       
00478df7 cc                       int3       
00478df8 cc                       int3       
00478df9 cc                       int3       
00478dfa cc                       int3       
00478dfb cc                       int3       
00478dfc cc                       int3       
00478dfd cc                       int3       
00478dfe cc                       int3       
00478dff cc                       int3       
