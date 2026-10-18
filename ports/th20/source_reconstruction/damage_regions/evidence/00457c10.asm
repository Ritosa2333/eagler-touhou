00457c10 55                       push       ebp
00457c11 8bec                     mov        ebp, esp
00457c13 83ec44                   sub        esp, 0x44
00457c16 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00457c1b 33c5                     xor        eax, ebp
00457c1d 8945fc                   mov        dword ptr [ebp - 4], eax
00457c20 51                       push       ecx
00457c21 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457c26 f30f110424               movss      dword ptr [esp], xmm0
00457c2b 51                       push       ecx
00457c2c f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457c31 f30f110424               movss      dword ptr [esp], xmm0
00457c36 51                       push       ecx
00457c37 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00457c3c f30f110424               movss      dword ptr [esp], xmm0
00457c41 51                       push       ecx
00457c42 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00457c47 f30f110424               movss      dword ptr [esp], xmm0
00457c4c 51                       push       ecx
00457c4d f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00457c52 f30f110424               movss      dword ptr [esp], xmm0
00457c57 51                       push       ecx
00457c58 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00457c5d f30f110424               movss      dword ptr [esp], xmm0
00457c62 e899f6ffff               call       0x457300
00457c67 83c418                   add        esp, 0x18
00457c6a 0fb6c0                   movzx      eax, al
00457c6d 85c0                     test       eax, eax
00457c6f 7407                     je         0x457c78
00457c71 b001                     mov        al, 1
00457c73 e935040000               jmp        0x4580ad
00457c78 8b4d2c                   mov        ecx, dword ptr [ebp + 0x2c]
00457c7b 51                       push       ecx
00457c7c 51                       push       ecx
00457c7d f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00457c82 f30f110424               movss      dword ptr [esp], xmm0
00457c87 51                       push       ecx
00457c88 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00457c8d f30f110424               movss      dword ptr [esp], xmm0
00457c92 51                       push       ecx
00457c93 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00457c98 f30f110424               movss      dword ptr [esp], xmm0
00457c9d 51                       push       ecx
00457c9e f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00457ca3 f30f110424               movss      dword ptr [esp], xmm0
00457ca8 51                       push       ecx
00457ca9 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00457cae f30f110424               movss      dword ptr [esp], xmm0
00457cb3 51                       push       ecx
00457cb4 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00457cb9 f30f110424               movss      dword ptr [esp], xmm0
00457cbe e82df4ffff               call       0x4570f0
00457cc3 83c41c                   add        esp, 0x1c
00457cc6 0fb6d0                   movzx      edx, al
00457cc9 85d2                     test       edx, edx
00457ccb 7407                     je         0x457cd4
00457ccd b001                     mov        al, 1
00457ccf e9d9030000               jmp        0x4580ad
00457cd4 68102e4200               push       0x422e10
00457cd9 6a04                     push       4
00457cdb 6a0c                     push       0xc
00457cdd 8d45cc                   lea        eax, [ebp - 0x34]
00457ce0 50                       push       eax
00457ce1 e83a3ffbff               call       0x40bc20
00457ce6 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457ceb 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00457cf2 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457cfa b90c000000               mov        ecx, 0xc
00457cff 6bd100                   imul       edx, ecx, 0
00457d02 f30f114415cc             movss      dword ptr [ebp + edx - 0x34], xmm0
00457d08 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457d0d 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00457d14 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457d1c b80c000000               mov        eax, 0xc
00457d21 6bc800                   imul       ecx, eax, 0
00457d24 f30f11440dd0             movss      dword ptr [ebp + ecx - 0x30], xmm0
00457d2a f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457d2f 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00457d36 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457d3e ba0c000000               mov        edx, 0xc
00457d43 c1e200                   shl        edx, 0
00457d46 f30f114415cc             movss      dword ptr [ebp + edx - 0x34], xmm0
00457d4c f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457d51 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457d59 b80c000000               mov        eax, 0xc
00457d5e c1e000                   shl        eax, 0
00457d61 f30f114405d0             movss      dword ptr [ebp + eax - 0x30], xmm0
00457d67 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457d6c f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457d74 b90c000000               mov        ecx, 0xc
00457d79 d1e1                     shl        ecx, 1
00457d7b f30f11440dcc             movss      dword ptr [ebp + ecx - 0x34], xmm0
00457d81 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457d86 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457d8e ba0c000000               mov        edx, 0xc
00457d93 d1e2                     shl        edx, 1
00457d95 f30f114415d0             movss      dword ptr [ebp + edx - 0x30], xmm0
00457d9b f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457da0 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457da8 b80c000000               mov        eax, 0xc
00457dad 6bc803                   imul       ecx, eax, 3
00457db0 f30f11440dcc             movss      dword ptr [ebp + ecx - 0x34], xmm0
00457db6 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457dbb 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00457dc2 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457dca ba0c000000               mov        edx, 0xc
00457dcf 6bc203                   imul       eax, edx, 3
00457dd2 f30f114405d0             movss      dword ptr [ebp + eax - 0x30], xmm0
00457dd8 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00457ddd 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
00457de4 9f                       lahf       
00457de5 f6c444                   test       ah, 0x44
00457de8 7b1d                     jnp        0x457e07
00457dea 6a04                     push       4
00457dec 51                       push       ecx
00457ded f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00457df2 f30f110424               movss      dword ptr [esp], xmm0
00457df7 8d4dcc                   lea        ecx, [ebp - 0x34]
00457dfa 51                       push       ecx
00457dfb 8d55cc                   lea        edx, [ebp - 0x34]
00457dfe 52                       push       edx
00457dff e8dc120000               call       0x4590e0
00457e04 83c410                   add        esp, 0x10
00457e07 c745c800000000           mov        dword ptr [ebp - 0x38], 0
00457e0e eb09                     jmp        0x457e19
00457e10 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
00457e13 83c001                   add        eax, 1
00457e16 8945c8                   mov        dword ptr [ebp - 0x38], eax
00457e19 837dc804                 cmp        dword ptr [ebp - 0x38], 4
00457e1d 7d3b                     jge        0x457e5a
00457e1f 51                       push       ecx
00457e20 0f57c0                   xorps      xmm0, xmm0
00457e23 f30f110424               movss      dword ptr [esp], xmm0
00457e28 51                       push       ecx
00457e29 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00457e2e f30f110424               movss      dword ptr [esp], xmm0
00457e33 51                       push       ecx
00457e34 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00457e39 f30f110424               movss      dword ptr [esp], xmm0
00457e3e 8d4dbc                   lea        ecx, [ebp - 0x44]
00457e41 e88aaffcff               call       0x422dd0
00457e46 8d4dbc                   lea        ecx, [ebp - 0x44]
00457e49 51                       push       ecx
00457e4a 6b55c80c                 imul       edx, dword ptr [ebp - 0x38], 0xc
00457e4e 8d4c15cc                 lea        ecx, [ebp + edx - 0x34]
00457e52 e88918fdff               call       0x4296e0
00457e57 90                       nop        
00457e58 ebb6                     jmp        0x457e10
00457e5a 8b452c                   mov        eax, dword ptr [ebp + 0x2c]
00457e5d 50                       push       eax
00457e5e 51                       push       ecx
00457e5f f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00457e64 f30f110424               movss      dword ptr [esp], xmm0
00457e69 51                       push       ecx
00457e6a f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00457e6f f30f110424               movss      dword ptr [esp], xmm0
00457e74 51                       push       ecx
00457e75 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00457e7a f30f110424               movss      dword ptr [esp], xmm0
00457e7f 51                       push       ecx
00457e80 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00457e85 f30f110424               movss      dword ptr [esp], xmm0
00457e8a b90c000000               mov        ecx, 0xc
00457e8f c1e100                   shl        ecx, 0
00457e92 51                       push       ecx
00457e93 f30f10440dd0             movss      xmm0, dword ptr [ebp + ecx - 0x30]
00457e99 f30f110424               movss      dword ptr [esp], xmm0
00457e9e ba0c000000               mov        edx, 0xc
00457ea3 c1e200                   shl        edx, 0
00457ea6 51                       push       ecx
00457ea7 f30f104415cc             movss      xmm0, dword ptr [ebp + edx - 0x34]
00457ead f30f110424               movss      dword ptr [esp], xmm0
00457eb2 b80c000000               mov        eax, 0xc
00457eb7 6bc800                   imul       ecx, eax, 0
00457eba 51                       push       ecx
00457ebb f30f10440dd0             movss      xmm0, dword ptr [ebp + ecx - 0x30]
00457ec1 f30f110424               movss      dword ptr [esp], xmm0
00457ec6 ba0c000000               mov        edx, 0xc
00457ecb 6bc200                   imul       eax, edx, 0
00457ece 51                       push       ecx
00457ecf f30f104405cc             movss      xmm0, dword ptr [ebp + eax - 0x34]
00457ed5 f30f110424               movss      dword ptr [esp], xmm0
00457eda e861ecffff               call       0x456b40
00457edf 83c424                   add        esp, 0x24
00457ee2 0fb6c8                   movzx      ecx, al
00457ee5 85c9                     test       ecx, ecx
00457ee7 7407                     je         0x457ef0
00457ee9 b001                     mov        al, 1
00457eeb e9bd010000               jmp        0x4580ad
00457ef0 8b552c                   mov        edx, dword ptr [ebp + 0x2c]
00457ef3 52                       push       edx
00457ef4 51                       push       ecx
00457ef5 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00457efa f30f110424               movss      dword ptr [esp], xmm0
00457eff 51                       push       ecx
00457f00 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00457f05 f30f110424               movss      dword ptr [esp], xmm0
00457f0a 51                       push       ecx
00457f0b f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00457f10 f30f110424               movss      dword ptr [esp], xmm0
00457f15 51                       push       ecx
00457f16 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00457f1b f30f110424               movss      dword ptr [esp], xmm0
00457f20 b80c000000               mov        eax, 0xc
00457f25 d1e0                     shl        eax, 1
00457f27 51                       push       ecx
00457f28 f30f104405d0             movss      xmm0, dword ptr [ebp + eax - 0x30]
00457f2e f30f110424               movss      dword ptr [esp], xmm0
00457f33 b90c000000               mov        ecx, 0xc
00457f38 d1e1                     shl        ecx, 1
00457f3a 51                       push       ecx
00457f3b f30f10440dcc             movss      xmm0, dword ptr [ebp + ecx - 0x34]
00457f41 f30f110424               movss      dword ptr [esp], xmm0
00457f46 ba0c000000               mov        edx, 0xc
00457f4b c1e200                   shl        edx, 0
00457f4e 51                       push       ecx
00457f4f f30f104415d0             movss      xmm0, dword ptr [ebp + edx - 0x30]
00457f55 f30f110424               movss      dword ptr [esp], xmm0
00457f5a b80c000000               mov        eax, 0xc
00457f5f c1e000                   shl        eax, 0
00457f62 51                       push       ecx
00457f63 f30f104405cc             movss      xmm0, dword ptr [ebp + eax - 0x34]
00457f69 f30f110424               movss      dword ptr [esp], xmm0
00457f6e e8cdebffff               call       0x456b40
00457f73 83c424                   add        esp, 0x24
00457f76 0fb6c8                   movzx      ecx, al
00457f79 85c9                     test       ecx, ecx
00457f7b 7407                     je         0x457f84
00457f7d b001                     mov        al, 1
00457f7f e929010000               jmp        0x4580ad
00457f84 8b552c                   mov        edx, dword ptr [ebp + 0x2c]
00457f87 52                       push       edx
00457f88 51                       push       ecx
00457f89 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00457f8e f30f110424               movss      dword ptr [esp], xmm0
00457f93 51                       push       ecx
00457f94 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00457f99 f30f110424               movss      dword ptr [esp], xmm0
00457f9e 51                       push       ecx
00457f9f f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00457fa4 f30f110424               movss      dword ptr [esp], xmm0
00457fa9 51                       push       ecx
00457faa f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00457faf f30f110424               movss      dword ptr [esp], xmm0
00457fb4 b80c000000               mov        eax, 0xc
00457fb9 6bc803                   imul       ecx, eax, 3
00457fbc 51                       push       ecx
00457fbd f30f10440dd0             movss      xmm0, dword ptr [ebp + ecx - 0x30]
00457fc3 f30f110424               movss      dword ptr [esp], xmm0
00457fc8 ba0c000000               mov        edx, 0xc
00457fcd 6bc203                   imul       eax, edx, 3
00457fd0 51                       push       ecx
00457fd1 f30f104405cc             movss      xmm0, dword ptr [ebp + eax - 0x34]
00457fd7 f30f110424               movss      dword ptr [esp], xmm0
00457fdc b90c000000               mov        ecx, 0xc
00457fe1 d1e1                     shl        ecx, 1
00457fe3 51                       push       ecx
00457fe4 f30f10440dd0             movss      xmm0, dword ptr [ebp + ecx - 0x30]
00457fea f30f110424               movss      dword ptr [esp], xmm0
00457fef ba0c000000               mov        edx, 0xc
00457ff4 d1e2                     shl        edx, 1
00457ff6 51                       push       ecx
00457ff7 f30f104415cc             movss      xmm0, dword ptr [ebp + edx - 0x34]
00457ffd f30f110424               movss      dword ptr [esp], xmm0
00458002 e839ebffff               call       0x456b40
00458007 83c424                   add        esp, 0x24
0045800a 0fb6c0                   movzx      eax, al
0045800d 85c0                     test       eax, eax
0045800f 7407                     je         0x458018
00458011 b001                     mov        al, 1
00458013 e995000000               jmp        0x4580ad
00458018 8b4d2c                   mov        ecx, dword ptr [ebp + 0x2c]
0045801b 51                       push       ecx
0045801c 51                       push       ecx
0045801d f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00458022 f30f110424               movss      dword ptr [esp], xmm0
00458027 51                       push       ecx
00458028 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
0045802d f30f110424               movss      dword ptr [esp], xmm0
00458032 51                       push       ecx
00458033 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00458038 f30f110424               movss      dword ptr [esp], xmm0
0045803d 51                       push       ecx
0045803e f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00458043 f30f110424               movss      dword ptr [esp], xmm0
00458048 ba0c000000               mov        edx, 0xc
0045804d 6bc200                   imul       eax, edx, 0
00458050 51                       push       ecx
00458051 f30f104405d0             movss      xmm0, dword ptr [ebp + eax - 0x30]
00458057 f30f110424               movss      dword ptr [esp], xmm0
0045805c b90c000000               mov        ecx, 0xc
00458061 6bd100                   imul       edx, ecx, 0
00458064 51                       push       ecx
00458065 f30f104415cc             movss      xmm0, dword ptr [ebp + edx - 0x34]
0045806b f30f110424               movss      dword ptr [esp], xmm0
00458070 b80c000000               mov        eax, 0xc
00458075 6bc803                   imul       ecx, eax, 3
00458078 51                       push       ecx
00458079 f30f10440dd0             movss      xmm0, dword ptr [ebp + ecx - 0x30]
0045807f f30f110424               movss      dword ptr [esp], xmm0
00458084 ba0c000000               mov        edx, 0xc
00458089 6bc203                   imul       eax, edx, 3
0045808c 51                       push       ecx
0045808d f30f104405cc             movss      xmm0, dword ptr [ebp + eax - 0x34]
00458093 f30f110424               movss      dword ptr [esp], xmm0
00458098 e8a3eaffff               call       0x456b40
0045809d 83c424                   add        esp, 0x24
004580a0 0fb6c8                   movzx      ecx, al
004580a3 85c9                     test       ecx, ecx
004580a5 7404                     je         0x4580ab
004580a7 b001                     mov        al, 1
004580a9 eb02                     jmp        0x4580ad
004580ab 32c0                     xor        al, al
004580ad 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004580b0 33cd                     xor        ecx, ebp
004580b2 e837a90e00               call       0x5429ee
004580b7 8be5                     mov        esp, ebp
004580b9 5d                       pop        ebp
004580ba c3                       ret        
004580bb cc                       int3       
004580bc cc                       int3       
004580bd cc                       int3       
004580be cc                       int3       
004580bf cc                       int3       
