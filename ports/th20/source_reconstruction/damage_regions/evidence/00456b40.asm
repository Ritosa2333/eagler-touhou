00456b40 55                       push       ebp
00456b41 8bec                     mov        ebp, esp
00456b43 83ec44                   sub        esp, 0x44
00456b46 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00456b4b 33c5                     xor        eax, ebp
00456b4d 8945fc                   mov        dword ptr [ebp - 4], eax
00456b50 68102e4200               push       0x422e10
00456b55 6a02                     push       2
00456b57 6a0c                     push       0xc
00456b59 8d45e4                   lea        eax, [ebp - 0x1c]
00456b5c 50                       push       eax
00456b5d e8be50fbff               call       0x40bc20
00456b62 51                       push       ecx
00456b63 0f57c0                   xorps      xmm0, xmm0
00456b66 f30f110424               movss      dword ptr [esp], xmm0
00456b6b 51                       push       ecx
00456b6c 0f57c0                   xorps      xmm0, xmm0
00456b6f f30f110424               movss      dword ptr [esp], xmm0
00456b74 51                       push       ecx
00456b75 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00456b7a f30f110424               movss      dword ptr [esp], xmm0
00456b7f 8d4dd8                   lea        ecx, [ebp - 0x28]
00456b82 e849c2fcff               call       0x422dd0
00456b87 90                       nop        
00456b88 c745d400000000           mov        dword ptr [ebp - 0x2c], 0
00456b8f eb09                     jmp        0x456b9a
00456b91 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00456b94 83c101                   add        ecx, 1
00456b97 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00456b9a 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00456b9d 3b5528                   cmp        edx, dword ptr [ebp + 0x28]
00456ba0 0f8d8f010000             jge        0x456d35
00456ba6 51                       push       ecx
00456ba7 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00456bac f30f110424               movss      dword ptr [esp], xmm0
00456bb1 8d45d8                   lea        eax, [ebp - 0x28]
00456bb4 50                       push       eax
00456bb5 b90c000000               mov        ecx, 0xc
00456bba 6bd100                   imul       edx, ecx, 0
00456bbd 8d4415e4                 lea        eax, [ebp + edx - 0x1c]
00456bc1 50                       push       eax
00456bc2 e8d9230000               call       0x458fa0
00456bc7 83c40c                   add        esp, 0xc
00456bca f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00456bd2 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00456bda f30f2a4d28               cvtsi2ss   xmm1, dword ptr [ebp + 0x28]
00456bdf f30f5ec1                 divss      xmm0, xmm1
00456be3 f30f584524               addss      xmm0, dword ptr [ebp + 0x24]
00456be8 f30f114524               movss      dword ptr [ebp + 0x24], xmm0
00456bed 51                       push       ecx
00456bee f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00456bf3 f30f110424               movss      dword ptr [esp], xmm0
00456bf8 e84319feff               call       0x438540
00456bfd 83c404                   add        esp, 4
00456c00 d95d24                   fstp       dword ptr [ebp + 0x24]
00456c03 51                       push       ecx
00456c04 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00456c09 f30f110424               movss      dword ptr [esp], xmm0
00456c0e 8d4dd8                   lea        ecx, [ebp - 0x28]
00456c11 51                       push       ecx
00456c12 ba0c000000               mov        edx, 0xc
00456c17 c1e200                   shl        edx, 0
00456c1a 8d4415e4                 lea        eax, [ebp + edx - 0x1c]
00456c1e 50                       push       eax
00456c1f e87c230000               call       0x458fa0
00456c24 83c40c                   add        esp, 0xc
00456c27 51                       push       ecx
00456c28 0f57c0                   xorps      xmm0, xmm0
00456c2b f30f110424               movss      dword ptr [esp], xmm0
00456c30 51                       push       ecx
00456c31 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00456c36 f30f110424               movss      dword ptr [esp], xmm0
00456c3b 51                       push       ecx
00456c3c f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456c41 f30f110424               movss      dword ptr [esp], xmm0
00456c46 8d4dc8                   lea        ecx, [ebp - 0x38]
00456c49 e882c1fcff               call       0x422dd0
00456c4e 8d4dc8                   lea        ecx, [ebp - 0x38]
00456c51 51                       push       ecx
00456c52 ba0c000000               mov        edx, 0xc
00456c57 6bc200                   imul       eax, edx, 0
00456c5a 8d4c05e4                 lea        ecx, [ebp + eax - 0x1c]
00456c5e e87d2afdff               call       0x4296e0
00456c63 90                       nop        
00456c64 51                       push       ecx
00456c65 0f57c0                   xorps      xmm0, xmm0
00456c68 f30f110424               movss      dword ptr [esp], xmm0
00456c6d 51                       push       ecx
00456c6e f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00456c73 f30f110424               movss      dword ptr [esp], xmm0
00456c78 51                       push       ecx
00456c79 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456c7e f30f110424               movss      dword ptr [esp], xmm0
00456c83 8d4dbc                   lea        ecx, [ebp - 0x44]
00456c86 e845c1fcff               call       0x422dd0
00456c8b 8d4dbc                   lea        ecx, [ebp - 0x44]
00456c8e 51                       push       ecx
00456c8f ba0c000000               mov        edx, 0xc
00456c94 c1e200                   shl        edx, 0
00456c97 8d4c15e4                 lea        ecx, [ebp + edx - 0x1c]
00456c9b e8402afdff               call       0x4296e0
00456ca0 90                       nop        
00456ca1 51                       push       ecx
00456ca2 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00456ca7 f30f110424               movss      dword ptr [esp], xmm0
00456cac 51                       push       ecx
00456cad f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00456cb2 f30f110424               movss      dword ptr [esp], xmm0
00456cb7 51                       push       ecx
00456cb8 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00456cbd f30f110424               movss      dword ptr [esp], xmm0
00456cc2 51                       push       ecx
00456cc3 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00456cc8 f30f110424               movss      dword ptr [esp], xmm0
00456ccd b80c000000               mov        eax, 0xc
00456cd2 c1e000                   shl        eax, 0
00456cd5 51                       push       ecx
00456cd6 f30f104405e8             movss      xmm0, dword ptr [ebp + eax - 0x18]
00456cdc f30f110424               movss      dword ptr [esp], xmm0
00456ce1 b90c000000               mov        ecx, 0xc
00456ce6 c1e100                   shl        ecx, 0
00456ce9 51                       push       ecx
00456cea f30f10440de4             movss      xmm0, dword ptr [ebp + ecx - 0x1c]
00456cf0 f30f110424               movss      dword ptr [esp], xmm0
00456cf5 ba0c000000               mov        edx, 0xc
00456cfa 6bc200                   imul       eax, edx, 0
00456cfd 51                       push       ecx
00456cfe f30f104405e8             movss      xmm0, dword ptr [ebp + eax - 0x18]
00456d04 f30f110424               movss      dword ptr [esp], xmm0
00456d09 b90c000000               mov        ecx, 0xc
00456d0e 6bd100                   imul       edx, ecx, 0
00456d11 51                       push       ecx
00456d12 f30f104415e4             movss      xmm0, dword ptr [ebp + edx - 0x1c]
00456d18 f30f110424               movss      dword ptr [esp], xmm0
00456d1d e8fefbffff               call       0x456920
00456d22 83c420                   add        esp, 0x20
00456d25 0fb6c0                   movzx      eax, al
00456d28 85c0                     test       eax, eax
00456d2a 7404                     je         0x456d30
00456d2c b001                     mov        al, 1
00456d2e eb07                     jmp        0x456d37
00456d30 e95cfeffff               jmp        0x456b91
00456d35 32c0                     xor        al, al
00456d37 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00456d3a 33cd                     xor        ecx, ebp
00456d3c e8adbc0e00               call       0x5429ee
00456d41 8be5                     mov        esp, ebp
00456d43 5d                       pop        ebp
00456d44 c3                       ret        
00456d45 cc                       int3       
00456d46 cc                       int3       
00456d47 cc                       int3       
00456d48 cc                       int3       
00456d49 cc                       int3       
00456d4a cc                       int3       
00456d4b cc                       int3       
00456d4c cc                       int3       
00456d4d cc                       int3       
00456d4e cc                       int3       
00456d4f cc                       int3       
