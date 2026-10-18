00531e80 55                       push       ebp
00531e81 8bec                     mov        ebp, esp
00531e83 83ec38                   sub        esp, 0x38
00531e86 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00531e8b 33c5                     xor        eax, ebp
00531e8d 8945fc                   mov        dword ptr [ebp - 4], eax
00531e90 56                       push       esi
00531e91 894df4                   mov        dword ptr [ebp - 0xc], ecx
00531e94 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00531e97 83c014                   add        eax, 0x14
00531e9a 8945f0                   mov        dword ptr [ebp - 0x10], eax
00531e9d 6a00                     push       0
00531e9f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00531ea2 e8e916efff               call       0x423590
00531ea7 0fb6c8                   movzx      ecx, al
00531eaa 85c9                     test       ecx, ecx
00531eac 741a                     je         0x531ec8
00531eae 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00531eb1 83c138                   add        ecx, 0x38
00531eb4 e817def1ff               call       0x44fcd0
00531eb9 b801000000               mov        eax, 1
00531ebe e951010000               jmp        0x532014
00531ec3 e937010000               jmp        0x531fff
00531ec8 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00531ecb 83c138                   add        ecx, 0x38
00531ece e85dd2f1ff               call       0x44f130
00531ed3 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00531ed6 83c138                   add        ecx, 0x38
00531ed9 e852dbf1ff               call       0x44fa30
00531ede 6a00                     push       0
00531ee0 e89b21f3ff               call       0x464080
00531ee5 83c404                   add        esp, 4
00531ee8 8945e8                   mov        dword ptr [ebp - 0x18], eax
00531eeb 6a00                     push       0
00531eed e88e21f3ff               call       0x464080
00531ef2 83c404                   add        esp, 4
00531ef5 8bc8                     mov        ecx, eax
00531ef7 e8249afeff               call       0x51b920
00531efc 8bf0                     mov        esi, eax
00531efe 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00531f01 83c114                   add        ecx, 0x14
00531f04 e887e0edff               call       0x40ff90
00531f09 0faff0                   imul       esi, eax
00531f0c e84f230000               call       0x534260
00531f11 8bc8                     mov        ecx, eax
00531f13 8bc6                     mov        eax, esi
00531f15 99                       cdq        
00531f16 f7f9                     idiv       ecx
00531f18 8945ec                   mov        dword ptr [ebp - 0x14], eax
00531f1b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00531f1e 52                       push       edx
00531f1f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00531f22 e819caf8ff               call       0x4be940
00531f27 6a00                     push       0
00531f29 e8526ff4ff               call       0x478e80
00531f2e 83c404                   add        esp, 4
00531f31 8945e0                   mov        dword ptr [ebp - 0x20], eax
00531f34 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00531f37 f30f10405c               movss      xmm0, dword ptr [eax + 0x5c]
00531f3c f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00531f41 6a00                     push       0
00531f43 689f860100               push       0x1869f
00531f48 6a00                     push       0
00531f4a 51                       push       ecx
00531f4b f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00531f50 f30f110424               movss      dword ptr [esp], xmm0
00531f55 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00531f58 81c18c000000             add        ecx, 0x8c
00531f5e 51                       push       ecx
00531f5f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00531f62 e8f9aff4ff               call       0x47cf60
00531f67 6a00                     push       0
00531f69 e8726ff4ff               call       0x478ee0
00531f6e 83c404                   add        esp, 4
00531f71 8945d8                   mov        dword ptr [ebp - 0x28], eax
00531f74 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00531f77 f30f10425c               movss      xmm0, dword ptr [edx + 0x5c]
00531f7c f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00531f81 6a01                     push       1
00531f83 6a00                     push       0
00531f85 51                       push       ecx
00531f86 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00531f8b f30f110424               movss      dword ptr [esp], xmm0
00531f90 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00531f93 058c000000               add        eax, 0x8c
00531f98 50                       push       eax
00531f99 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00531f9c e82f8bf9ff               call       0x4caad0
00531fa1 6a00                     push       0
00531fa3 e8186ff4ff               call       0x478ec0
00531fa8 83c404                   add        esp, 4
00531fab 8945d0                   mov        dword ptr [ebp - 0x30], eax
00531fae 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00531fb1 f30f10415c               movss      xmm0, dword ptr [ecx + 0x5c]
00531fb6 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
00531fbb 6a14                     push       0x14
00531fbd 6a02                     push       2
00531fbf 51                       push       ecx
00531fc0 0f57c0                   xorps      xmm0, xmm0
00531fc3 f30f110424               movss      dword ptr [esp], xmm0
00531fc8 51                       push       ecx
00531fc9 f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
00531fce f30f110424               movss      dword ptr [esp], xmm0
00531fd3 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00531fd6 81c28c000000             add        edx, 0x8c
00531fdc 52                       push       edx
00531fdd 8d45f8                   lea        eax, [ebp - 8]
00531fe0 50                       push       eax
00531fe1 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00531fe4 e897fbf8ff               call       0x4c1b80
00531fe9 8d4df8                   lea        ecx, [ebp - 8]
00531fec e81feef8ff               call       0x4c0e10
00531ff1 8945cc                   mov        dword ptr [ebp - 0x34], eax
00531ff4 6a01                     push       1
00531ff6 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00531ff9 e8f24cfdff               call       0x506cf0
00531ffe 90                       nop        
00531fff 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00532002 83c114                   add        ecx, 0x14
00532005 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00532008 6a00                     push       0
0053200a 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0053200d e80e74efff               call       0x429420
00532012 33c0                     xor        eax, eax
00532014 5e                       pop        esi
00532015 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532018 33cd                     xor        ecx, ebp
0053201a e8cf090100               call       0x5429ee
0053201f 8be5                     mov        esp, ebp
00532021 5d                       pop        ebp
00532022 c3                       ret        
00532023 cc                       int3       
00532024 cc                       int3       
00532025 cc                       int3       
00532026 cc                       int3       
00532027 cc                       int3       
00532028 cc                       int3       
00532029 cc                       int3       
0053202a cc                       int3       
0053202b cc                       int3       
0053202c cc                       int3       
0053202d cc                       int3       
0053202e cc                       int3       
0053202f cc                       int3       
