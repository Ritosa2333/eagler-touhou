00466e20 55                       push       ebp
00466e21 8bec                     mov        ebp, esp
00466e23 83ec38                   sub        esp, 0x38
00466e26 894df0                   mov        dword ptr [ebp - 0x10], ecx
00466e29 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00466e2e f30f5e0580f25600         divss      xmm0, dword ptr [0x56f280]
00466e36 f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
00466e3b c745f800000000           mov        dword ptr [ebp - 8], 0
00466e42 c745fc00000000           mov        dword ptr [ebp - 4], 0
00466e49 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00466e4c 05f0000000               add        eax, 0xf0
00466e51 8945e8                   mov        dword ptr [ebp - 0x18], eax
00466e54 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00466e57 e8444ffaff               call       0x40bda0
00466e5c 8945f4                   mov        dword ptr [ebp - 0xc], eax
00466e5f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00466e62 e849010000               call       0x466fb0
00466e67 8945e0                   mov        dword ptr [ebp - 0x20], eax
00466e6a eb09                     jmp        0x466e75
00466e6c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00466e6f 83c104                   add        ecx, 4
00466e72 894df4                   mov        dword ptr [ebp - 0xc], ecx
00466e75 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00466e78 3b55e0                   cmp        edx, dword ptr [ebp - 0x20]
00466e7b 747d                     je         0x466efa
00466e7d 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00466e80 8945dc                   mov        dword ptr [ebp - 0x24], eax
00466e83 8d4d14                   lea        ecx, [ebp + 0x14]
00466e86 e87554faff               call       0x40c300
00466e8b 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00466e8e 8901                     mov        dword ptr [ecx], eax
00466e90 837df808                 cmp        dword ptr [ebp - 8], 8
00466e94 7d29                     jge        0x466ebf
00466e96 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00466e99 81c2f0000000             add        edx, 0xf0
00466e9f 8955d8                   mov        dword ptr [ebp - 0x28], edx
00466ea2 8b45f8                   mov        eax, dword ptr [ebp - 8]
00466ea5 50                       push       eax
00466ea6 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00466ea9 e8d2d6faff               call       0x414580
00466eae 8a4dfc                   mov        cl, byte ptr [ebp - 4]
00466eb1 884803                   mov        byte ptr [eax + 3], cl
00466eb4 8b55fc                   mov        edx, dword ptr [ebp - 4]
00466eb7 83c220                   add        edx, 0x20
00466eba 8955fc                   mov        dword ptr [ebp - 4], edx
00466ebd eb2d                     jmp        0x466eec
00466ebf 837df80e                 cmp        dword ptr [ebp - 8], 0xe
00466ec3 7c27                     jl         0x466eec
00466ec5 8b45fc                   mov        eax, dword ptr [ebp - 4]
00466ec8 83e808                   sub        eax, 8
00466ecb 8945fc                   mov        dword ptr [ebp - 4], eax
00466ece 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00466ed1 81c1f0000000             add        ecx, 0xf0
00466ed7 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00466eda 8b55f8                   mov        edx, dword ptr [ebp - 8]
00466edd 52                       push       edx
00466ede 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00466ee1 e89ad6faff               call       0x414580
00466ee6 8a4dfc                   mov        cl, byte ptr [ebp - 4]
00466ee9 884803                   mov        byte ptr [eax + 3], cl
00466eec 8b55f8                   mov        edx, dword ptr [ebp - 8]
00466eef 83c201                   add        edx, 1
00466ef2 8955f8                   mov        dword ptr [ebp - 8], edx
00466ef5 e972ffffff               jmp        0x466e6c
00466efa 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00466efd 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00466f00 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00466f03 e8984efaff               call       0x40bda0
00466f08 8945ec                   mov        dword ptr [ebp - 0x14], eax
00466f0b 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00466f0e e88d6dffff               call       0x45dca0
00466f13 8945d0                   mov        dword ptr [ebp - 0x30], eax
00466f16 eb09                     jmp        0x466f21
00466f18 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00466f1b 83c108                   add        ecx, 8
00466f1e 894dec                   mov        dword ptr [ebp - 0x14], ecx
00466f21 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00466f24 3b55d0                   cmp        edx, dword ptr [ebp - 0x30]
00466f27 7448                     je         0x466f71
00466f29 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00466f2c 8945cc                   mov        dword ptr [ebp - 0x34], eax
00466f2f 51                       push       ecx
00466f30 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00466f35 f30f110424               movss      dword ptr [esp], xmm0
00466f3a 51                       push       ecx
00466f3b f30f104508               movss      xmm0, dword ptr [ebp + 8]
00466f40 f30f110424               movss      dword ptr [esp], xmm0
00466f45 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00466f48 51                       push       ecx
00466f49 e8e223fdff               call       0x439330
00466f4e 83c40c                   add        esp, 0xc
00466f51 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00466f56 f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
00466f5b f30f114508               movss      dword ptr [ebp + 8], xmm0
00466f60 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00466f65 f30f5c45c8               subss      xmm0, dword ptr [ebp - 0x38]
00466f6a f30f114510               movss      dword ptr [ebp + 0x10], xmm0
00466f6f eba7                     jmp        0x466f18
00466f71 8be5                     mov        esp, ebp
00466f73 5d                       pop        ebp
00466f74 c21000                   ret        0x10
00466f77 cc                       int3       
00466f78 cc                       int3       
00466f79 cc                       int3       
00466f7a cc                       int3       
00466f7b cc                       int3       
00466f7c cc                       int3       
00466f7d cc                       int3       
00466f7e cc                       int3       
00466f7f cc                       int3       
