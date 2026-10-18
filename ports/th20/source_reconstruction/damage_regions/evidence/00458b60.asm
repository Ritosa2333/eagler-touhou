00458b60 55                       push       ebp
00458b61 8bec                     mov        ebp, esp
00458b63 83ec44                   sub        esp, 0x44
00458b66 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00458b6b 33c5                     xor        eax, ebp
00458b6d 8945fc                   mov        dword ptr [ebp - 4], eax
00458b70 68a0984300               push       0x4398a0
00458b75 6a04                     push       4
00458b77 6a08                     push       8
00458b79 8d45dc                   lea        eax, [ebp - 0x24]
00458b7c 50                       push       eax
00458b7d e89e30fbff               call       0x40bc20
00458b82 b908000000               mov        ecx, 8
00458b87 6bd100                   imul       edx, ecx, 0
00458b8a 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
00458b8d f30f100410               movss      xmm0, dword ptr [eax + edx]
00458b92 f30f5c4508               subss      xmm0, dword ptr [ebp + 8]
00458b97 b908000000               mov        ecx, 8
00458b9c 6bd100                   imul       edx, ecx, 0
00458b9f f30f114415dc             movss      dword ptr [ebp + edx - 0x24], xmm0
00458ba5 b808000000               mov        eax, 8
00458baa 6bc800                   imul       ecx, eax, 0
00458bad 8b551c                   mov        edx, dword ptr [ebp + 0x1c]
00458bb0 f30f10440a04             movss      xmm0, dword ptr [edx + ecx + 4]
00458bb6 f30f5c450c               subss      xmm0, dword ptr [ebp + 0xc]
00458bbb b808000000               mov        eax, 8
00458bc0 6bc800                   imul       ecx, eax, 0
00458bc3 f30f11440de0             movss      dword ptr [ebp + ecx - 0x20], xmm0
00458bc9 ba08000000               mov        edx, 8
00458bce c1e200                   shl        edx, 0
00458bd1 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
00458bd4 f30f100410               movss      xmm0, dword ptr [eax + edx]
00458bd9 f30f5c4508               subss      xmm0, dword ptr [ebp + 8]
00458bde b908000000               mov        ecx, 8
00458be3 c1e100                   shl        ecx, 0
00458be6 f30f11440ddc             movss      dword ptr [ebp + ecx - 0x24], xmm0
00458bec ba08000000               mov        edx, 8
00458bf1 c1e200                   shl        edx, 0
00458bf4 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
00458bf7 f30f10441004             movss      xmm0, dword ptr [eax + edx + 4]
00458bfd f30f5c450c               subss      xmm0, dword ptr [ebp + 0xc]
00458c02 b908000000               mov        ecx, 8
00458c07 c1e100                   shl        ecx, 0
00458c0a f30f11440de0             movss      dword ptr [ebp + ecx - 0x20], xmm0
00458c10 ba08000000               mov        edx, 8
00458c15 d1e2                     shl        edx, 1
00458c17 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
00458c1a f30f100410               movss      xmm0, dword ptr [eax + edx]
00458c1f f30f5c4508               subss      xmm0, dword ptr [ebp + 8]
00458c24 b908000000               mov        ecx, 8
00458c29 d1e1                     shl        ecx, 1
00458c2b f30f11440ddc             movss      dword ptr [ebp + ecx - 0x24], xmm0
00458c31 ba08000000               mov        edx, 8
00458c36 d1e2                     shl        edx, 1
00458c38 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
00458c3b f30f10441004             movss      xmm0, dword ptr [eax + edx + 4]
00458c41 f30f5c450c               subss      xmm0, dword ptr [ebp + 0xc]
00458c46 b908000000               mov        ecx, 8
00458c4b d1e1                     shl        ecx, 1
00458c4d f30f11440de0             movss      dword ptr [ebp + ecx - 0x20], xmm0
00458c53 ba08000000               mov        edx, 8
00458c58 6bc203                   imul       eax, edx, 3
00458c5b 8b4d1c                   mov        ecx, dword ptr [ebp + 0x1c]
00458c5e f30f100401               movss      xmm0, dword ptr [ecx + eax]
00458c63 f30f5c4508               subss      xmm0, dword ptr [ebp + 8]
00458c68 ba08000000               mov        edx, 8
00458c6d 6bc203                   imul       eax, edx, 3
00458c70 f30f114405dc             movss      dword ptr [ebp + eax - 0x24], xmm0
00458c76 b908000000               mov        ecx, 8
00458c7b 6bd103                   imul       edx, ecx, 3
00458c7e 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
00458c81 f30f10441004             movss      xmm0, dword ptr [eax + edx + 4]
00458c87 f30f5c450c               subss      xmm0, dword ptr [ebp + 0xc]
00458c8c b908000000               mov        ecx, 8
00458c91 6bd103                   imul       edx, ecx, 3
00458c94 f30f114415e0             movss      dword ptr [ebp + edx - 0x20], xmm0
00458c9a f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00458c9f 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
00458ca6 9f                       lahf       
00458ca7 f6c444                   test       ah, 0x44
00458caa 7b24                     jnp        0x458cd0
00458cac 6a04                     push       4
00458cae f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00458cb3 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00458cba 51                       push       ecx
00458cbb f30f110424               movss      dword ptr [esp], xmm0
00458cc0 8d45dc                   lea        eax, [ebp - 0x24]
00458cc3 50                       push       eax
00458cc4 8d4ddc                   lea        ecx, [ebp - 0x24]
00458cc7 51                       push       ecx
00458cc8 e863030000               call       0x459030
00458ccd 83c410                   add        esp, 0x10
00458cd0 ba08000000               mov        edx, 8
00458cd5 6bc200                   imul       eax, edx, 0
00458cd8 51                       push       ecx
00458cd9 f30f104405dc             movss      xmm0, dword ptr [ebp + eax - 0x24]
00458cdf f30f110424               movss      dword ptr [esp], xmm0
00458ce4 e897c9feff               call       0x445680
00458ce9 83c404                   add        esp, 4
00458cec d95dd8                   fstp       dword ptr [ebp - 0x28]
00458cef f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458cf4 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458cfc 0f2f45d8                 comiss     xmm0, dword ptr [ebp - 0x28]
00458d00 7239                     jb         0x458d3b
00458d02 b908000000               mov        ecx, 8
00458d07 6bd100                   imul       edx, ecx, 0
00458d0a 51                       push       ecx
00458d0b f30f104415e0             movss      xmm0, dword ptr [ebp + edx - 0x20]
00458d11 f30f110424               movss      dword ptr [esp], xmm0
00458d16 e865c9feff               call       0x445680
00458d1b 83c404                   add        esp, 4
00458d1e d95dd4                   fstp       dword ptr [ebp - 0x2c]
00458d21 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00458d26 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458d2e 0f2f45d4                 comiss     xmm0, dword ptr [ebp - 0x2c]
00458d32 7207                     jb         0x458d3b
00458d34 b001                     mov        al, 1
00458d36 e93b010000               jmp        0x458e76
00458d3b b808000000               mov        eax, 8
00458d40 c1e000                   shl        eax, 0
00458d43 51                       push       ecx
00458d44 f30f104405dc             movss      xmm0, dword ptr [ebp + eax - 0x24]
00458d4a f30f110424               movss      dword ptr [esp], xmm0
00458d4f e82cc9feff               call       0x445680
00458d54 83c404                   add        esp, 4
00458d57 d95dd0                   fstp       dword ptr [ebp - 0x30]
00458d5a f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458d5f f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458d67 0f2f45d0                 comiss     xmm0, dword ptr [ebp - 0x30]
00458d6b 7239                     jb         0x458da6
00458d6d b908000000               mov        ecx, 8
00458d72 c1e100                   shl        ecx, 0
00458d75 51                       push       ecx
00458d76 f30f10440de0             movss      xmm0, dword ptr [ebp + ecx - 0x20]
00458d7c f30f110424               movss      dword ptr [esp], xmm0
00458d81 e8fac8feff               call       0x445680
00458d86 83c404                   add        esp, 4
00458d89 d95dcc                   fstp       dword ptr [ebp - 0x34]
00458d8c f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00458d91 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458d99 0f2f45cc                 comiss     xmm0, dword ptr [ebp - 0x34]
00458d9d 7207                     jb         0x458da6
00458d9f b001                     mov        al, 1
00458da1 e9d0000000               jmp        0x458e76
00458da6 ba08000000               mov        edx, 8
00458dab d1e2                     shl        edx, 1
00458dad 51                       push       ecx
00458dae f30f104415dc             movss      xmm0, dword ptr [ebp + edx - 0x24]
00458db4 f30f110424               movss      dword ptr [esp], xmm0
00458db9 e8c2c8feff               call       0x445680
00458dbe 83c404                   add        esp, 4
00458dc1 d95dc8                   fstp       dword ptr [ebp - 0x38]
00458dc4 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458dc9 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458dd1 0f2f45c8                 comiss     xmm0, dword ptr [ebp - 0x38]
00458dd5 7235                     jb         0x458e0c
00458dd7 b808000000               mov        eax, 8
00458ddc d1e0                     shl        eax, 1
00458dde 51                       push       ecx
00458ddf f30f104405e0             movss      xmm0, dword ptr [ebp + eax - 0x20]
00458de5 f30f110424               movss      dword ptr [esp], xmm0
00458dea e891c8feff               call       0x445680
00458def 83c404                   add        esp, 4
00458df2 d95dc4                   fstp       dword ptr [ebp - 0x3c]
00458df5 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00458dfa f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458e02 0f2f45c4                 comiss     xmm0, dword ptr [ebp - 0x3c]
00458e06 7204                     jb         0x458e0c
00458e08 b001                     mov        al, 1
00458e0a eb6a                     jmp        0x458e76
00458e0c b908000000               mov        ecx, 8
00458e11 6bd103                   imul       edx, ecx, 3
00458e14 51                       push       ecx
00458e15 f30f104415dc             movss      xmm0, dword ptr [ebp + edx - 0x24]
00458e1b f30f110424               movss      dword ptr [esp], xmm0
00458e20 e85bc8feff               call       0x445680
00458e25 83c404                   add        esp, 4
00458e28 d95dc0                   fstp       dword ptr [ebp - 0x40]
00458e2b f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458e30 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458e38 0f2f45c0                 comiss     xmm0, dword ptr [ebp - 0x40]
00458e3c 7236                     jb         0x458e74
00458e3e b808000000               mov        eax, 8
00458e43 6bc803                   imul       ecx, eax, 3
00458e46 51                       push       ecx
00458e47 f30f10440de0             movss      xmm0, dword ptr [ebp + ecx - 0x20]
00458e4d f30f110424               movss      dword ptr [esp], xmm0
00458e52 e829c8feff               call       0x445680
00458e57 83c404                   add        esp, 4
00458e5a d95dbc                   fstp       dword ptr [ebp - 0x44]
00458e5d f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00458e62 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458e6a 0f2f45bc                 comiss     xmm0, dword ptr [ebp - 0x44]
00458e6e 7204                     jb         0x458e74
00458e70 b001                     mov        al, 1
00458e72 eb02                     jmp        0x458e76
00458e74 32c0                     xor        al, al
00458e76 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00458e79 33cd                     xor        ecx, ebp
00458e7b e86e9b0e00               call       0x5429ee
00458e80 8be5                     mov        esp, ebp
00458e82 5d                       pop        ebp
00458e83 c3                       ret        
00458e84 cc                       int3       
00458e85 cc                       int3       
00458e86 cc                       int3       
00458e87 cc                       int3       
00458e88 cc                       int3       
00458e89 cc                       int3       
00458e8a cc                       int3       
00458e8b cc                       int3       
00458e8c cc                       int3       
00458e8d cc                       int3       
00458e8e cc                       int3       
00458e8f cc                       int3       
