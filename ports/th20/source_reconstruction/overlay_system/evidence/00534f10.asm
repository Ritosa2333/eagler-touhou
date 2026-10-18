00534f10 55                       push       ebp
00534f11 8bec                     mov        ebp, esp
00534f13 83ec0c                   sub        esp, 0xc
00534f16 894df4                   mov        dword ptr [ebp - 0xc], ecx
00534f19 8b4508                   mov        eax, dword ptr [ebp + 8]
00534f1c 8b88c8050000             mov        ecx, dword ptr [eax + 0x5c8]
00534f22 894dfc                   mov        dword ptr [ebp - 4], ecx
00534f25 51                       push       ecx
00534f26 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00534f2e f30f110424               movss      dword ptr [esp], xmm0
00534f33 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534f36 e865bef6ff               call       0x4a0da0
00534f3b 8bc8                     mov        ecx, eax
00534f3d e84eb0edff               call       0x40ff90
00534f42 f30f2ac0                 cvtsi2ss   xmm0, eax
00534f46 f30f59057cfe5600         mulss      xmm0, dword ptr [0x56fe7c]
00534f4e f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00534f53 e808f3ffff               call       0x534260
00534f58 f30f2ac0                 cvtsi2ss   xmm0, eax
00534f5c f30f104df8               movss      xmm1, dword ptr [ebp - 8]
00534f61 f30f5ec8                 divss      xmm1, xmm0
00534f65 f30f580d30645700         addss      xmm1, dword ptr [0x576430]
00534f6d 51                       push       ecx
00534f6e f30f110c24               movss      dword ptr [esp], xmm1
00534f73 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00534f76 e8d5bef3ff               call       0x470e50
00534f7b 6a00                     push       0
00534f7d e8aeb8f2ff               call       0x460830
00534f82 83c404                   add        esp, 4
00534f85 8bc8                     mov        ecx, eax
00534f87 e8c4b8f2ff               call       0x460850
00534f8c 50                       push       eax
00534f8d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00534f90 e8cb8df2ff               call       0x45dd60
00534f95 33c0                     xor        eax, eax
00534f97 8be5                     mov        esp, ebp
00534f99 5d                       pop        ebp
00534f9a c20400                   ret        4
00534f9d cc                       int3       
00534f9e cc                       int3       
00534f9f cc                       int3       
