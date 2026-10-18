00510b60 55                       push       ebp
00510b61 8bec                     mov        ebp, esp
00510b63 83ec38                   sub        esp, 0x38
00510b66 56                       push       esi
00510b67 57                       push       edi
00510b68 894dfc                   mov        dword ptr [ebp - 4], ecx
00510b6b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510b6e 8b4824                   mov        ecx, dword ptr [eax + 0x24]
00510b71 894df8                   mov        dword ptr [ebp - 8], ecx
00510b74 8b55f8                   mov        edx, dword ptr [ebp - 8]
00510b77 83ea01                   sub        edx, 1
00510b7a 8955f8                   mov        dword ptr [ebp - 8], edx
00510b7d 837df805                 cmp        dword ptr [ebp - 8], 5
00510b81 0f87b6020000             ja         0x510e3d
00510b87 8b45f8                   mov        eax, dword ptr [ebp - 8]
00510b8a ff24855c0e5100           jmp        dword ptr [eax*4 + 0x510e5c]
00510b91 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
00510b97 e8f4090000               call       0x511590
00510b9c 90                       nop        
00510b9d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510ba0 8b5110                   mov        edx, dword ptr [ecx + 0x10]
00510ba3 8955f0                   mov        dword ptr [ebp - 0x10], edx
00510ba6 6a00                     push       0
00510ba8 6aff                     push       -1
00510baa 6a6f                     push       0x6f
00510bac 6838065700               push       0x570638
00510bb1 8d45c8                   lea        eax, [ebp - 0x38]
00510bb4 50                       push       eax
00510bb5 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00510bb8 e8b300f4ff               call       0x450c70
00510bbd 8b08                     mov        ecx, dword ptr [eax]
00510bbf 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510bc2 894a14                   mov        dword ptr [edx + 0x14], ecx
00510bc5 b968a55b00               mov        ecx, 0x5ba568
00510bca e89141f6ff               call       0x474d60
00510bcf 69c0a0860100             imul       eax, eax, 0x186a0
00510bd5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510bd8 894128                   mov        dword ptr [ecx + 0x28], eax
00510bdb 6a00                     push       0
00510bdd e89e34f5ff               call       0x464080
00510be2 83c404                   add        esp, 4
00510be5 8bc8                     mov        ecx, eax
00510be7 e8d476faff               call       0x4b82c0
00510bec 8bf0                     mov        esi, eax
00510bee 6a00                     push       0
00510bf0 e88b34f5ff               call       0x464080
00510bf5 83c404                   add        esp, 4
00510bf8 8bc8                     mov        ecx, eax
00510bfa e8c1070000               call       0x5113c0
00510bff 8bf8                     mov        edi, eax
00510c01 6a00                     push       0
00510c03 e87834f5ff               call       0x464080
00510c08 83c404                   add        esp, 4
00510c0b 8bc8                     mov        ecx, eax
00510c0d e80e070000               call       0x511320
00510c12 03f8                     add        edi, eax
00510c14 6a00                     push       0
00510c16 e86534f5ff               call       0x464080
00510c1b 83c404                   add        esp, 4
00510c1e 8bc8                     mov        ecx, eax
00510c20 e84b070000               call       0x511370
00510c25 03f8                     add        edi, eax
00510c27 6a00                     push       0
00510c29 e85234f5ff               call       0x464080
00510c2e 83c404                   add        esp, 4
00510c31 8bc8                     mov        ecx, eax
00510c33 e8d8070000               call       0x511410
00510c38 03f8                     add        edi, eax
00510c3a 0faff7                   imul       esi, edi
00510c3d 8975e8                   mov        dword ptr [ebp - 0x18], esi
00510c40 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510c43 8b4228                   mov        eax, dword ptr [edx + 0x28]
00510c46 8945ec                   mov        dword ptr [ebp - 0x14], eax
00510c49 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00510c4c 034de8                   add        ecx, dword ptr [ebp - 0x18]
00510c4f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510c52 894a28                   mov        dword ptr [edx + 0x28], ecx
00510c55 6a00                     push       0
00510c57 e82434f5ff               call       0x464080
00510c5c 83c404                   add        esp, 4
00510c5f 8bc8                     mov        ecx, eax
00510c61 e88a080000               call       0x5114f0
00510c66 8bf0                     mov        esi, eax
00510c68 6a00                     push       0
00510c6a e81134f5ff               call       0x464080
00510c6f 83c404                   add        esp, 4
00510c72 8bc8                     mov        ecx, eax
00510c74 e8e7070000               call       0x511460
00510c79 03f0                     add        esi, eax
00510c7b 6a00                     push       0
00510c7d e8fe33f5ff               call       0x464080
00510c82 83c404                   add        esp, 4
00510c85 8bc8                     mov        ecx, eax
00510c87 e814080000               call       0x5114a0
00510c8c 03f0                     add        esi, eax
00510c8e 6a00                     push       0
00510c90 e8eb33f5ff               call       0x464080
00510c95 83c404                   add        esp, 4
00510c98 8bc8                     mov        ecx, eax
00510c9a e891080000               call       0x511530
00510c9f 03f0                     add        esi, eax
00510ca1 69c6a0860100             imul       eax, esi, 0x186a0
00510ca7 8945e0                   mov        dword ptr [ebp - 0x20], eax
00510caa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510cad 8b5128                   mov        edx, dword ptr [ecx + 0x28]
00510cb0 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00510cb3 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
00510cb6 0345e0                   add        eax, dword ptr [ebp - 0x20]
00510cb9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510cbc 894128                   mov        dword ptr [ecx + 0x28], eax
00510cbf 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510cc2 8b4228                   mov        eax, dword ptr [edx + 0x28]
00510cc5 99                       cdq        
00510cc6 b90a000000               mov        ecx, 0xa
00510ccb f7f9                     idiv       ecx
00510ccd 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510cd0 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00510cd3 2bca                     sub        ecx, edx
00510cd5 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510cd8 894a28                   mov        dword ptr [edx + 0x28], ecx
00510cdb 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510cde 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00510ce1 51                       push       ecx
00510ce2 b968a55b00               mov        ecx, 0x5ba568
00510ce7 e86478f7ff               call       0x488550
00510cec 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510cef c7422402000000           mov        dword ptr [edx + 0x24], 2
00510cf6 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510cf9 c7401c00000000           mov        dword ptr [eax + 0x1c], 0
00510d00 c70518a55b0000000000     mov        dword ptr [0x5ba518], 0
00510d0a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510d0d c7412404000000           mov        dword ptr [ecx + 0x24], 4
00510d14 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510d17 83c22c                   add        edx, 0x2c
00510d1a 8955dc                   mov        dword ptr [ebp - 0x24], edx
00510d1d 6a78                     push       0x78
00510d1f 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00510d22 e89928f1ff               call       0x4235c0
00510d27 0fb6c0                   movzx      eax, al
00510d2a 85c0                     test       eax, eax
00510d2c 0f84c7000000             je         0x510df9
00510d32 33c9                     xor        ecx, ecx
00510d34 7534                     jne        0x510d6a
00510d36 ba04000000               mov        edx, 4
00510d3b 6bc200                   imul       eax, edx, 0
00510d3e 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00510d45 7423                     je         0x510d6a
00510d47 b904000000               mov        ecx, 4
00510d4c 6bd100                   imul       edx, ecx, 0
00510d4f 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00510d55 8945d8                   mov        dword ptr [ebp - 0x28], eax
00510d58 6801000800               push       0x80001
00510d5d 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00510d60 e89b8ef0ff               call       0x419c00
00510d65 8945f4                   mov        dword ptr [ebp - 0xc], eax
00510d68 eb07                     jmp        0x510d71
00510d6a c745f400000000           mov        dword ptr [ebp - 0xc], 0
00510d71 837df400                 cmp        dword ptr [ebp - 0xc], 0
00510d75 751d                     jne        0x510d94
00510d77 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510d7a 83c12c                   add        ecx, 0x2c
00510d7d 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00510d80 682c010000               push       0x12c
00510d85 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00510d88 e83328f1ff               call       0x4235c0
00510d8d 0fb6d0                   movzx      edx, al
00510d90 85d2                     test       edx, edx
00510d92 7465                     je         0x510df9
00510d94 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
00510d9b 741d                     je         0x510dba
00510d9d a128a85b00               mov        eax, dword ptr [0x5ba828]
00510da2 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
00510da8 81e1fffffffb             and        ecx, 0xfbffffff
00510dae 8b1528a85b00             mov        edx, dword ptr [0x5ba828]
00510db4 898ae8000000             mov        dword ptr [edx + 0xe8], ecx
00510dba 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510dbd 83c114                   add        ecx, 0x14
00510dc0 e80beff3ff               call       0x44fcd0
00510dc5 90                       nop        
00510dc6 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510dc9 c7402406000000           mov        dword ptr [eax + 0x24], 6
00510dd0 6a00                     push       0
00510dd2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510dd5 83c12c                   add        ecx, 0x2c
00510dd8 e84327f1ff               call       0x423520
00510ddd 90                       nop        
00510dde 51                       push       ecx
00510ddf f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
00510de7 f30f110424               movss      dword ptr [esp], xmm0
00510dec b9404d5c00               mov        ecx, 0x5c4d40
00510df1 e8da8bfcff               call       0x4d99d0
00510df6 90                       nop        
00510df7 eb44                     jmp        0x510e3d
00510df9 eb42                     jmp        0x510e3d
00510dfb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510dfe 83c12c                   add        ecx, 0x2c
00510e01 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00510e04 6a0a                     push       0xa
00510e06 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00510e09 e8d277f6ff               call       0x4785e0
00510e0e 0fb6d0                   movzx      edx, al
00510e11 85d2                     test       edx, edx
00510e13 7428                     je         0x510e3d
00510e15 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
00510e1c 740c                     je         0x510e2a
00510e1e 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
00510e24 e847b7faff               call       0x4bc570
00510e29 90                       nop        
00510e2a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510e2d 50                       push       eax
00510e2e e88d09f1ff               call       0x4217c0
00510e33 83c404                   add        esp, 4
00510e36 b801000000               mov        eax, 1
00510e3b eb18                     jmp        0x510e55
00510e3d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510e40 83c12c                   add        ecx, 0x2c
00510e43 894dcc                   mov        dword ptr [ebp - 0x34], ecx
00510e46 6a00                     push       0
00510e48 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00510e4b e8f026f1ff               call       0x423540
00510e50 b801000000               mov        eax, 1
00510e55 5f                       pop        edi
00510e56 5e                       pop        esi
00510e57 8be5                     mov        esp, ebp
00510e59 5d                       pop        ebp
00510e5a c3                       ret        
00510e5b 90                       nop        
00510e5c 91                       xchg       ecx, eax
00510e5d 0b5100                   or         edx, dword ptr [ecx]
00510e60 0a0d51003d0e             or         cl, byte ptr [0xe3d0051]
00510e66 51                       push       ecx
00510e67 00140d51003d0e           add        byte ptr [ecx + 0xe3d0051], dl
00510e6e 51                       push       ecx
00510e6f 00fb                     add        bl, bh
00510e71 0d5100cccc               or         eax, 0xcccc0051
00510e76 cc                       int3       
00510e77 cc                       int3       
00510e78 cc                       int3       
00510e79 cc                       int3       
00510e7a cc                       int3       
00510e7b cc                       int3       
00510e7c cc                       int3       
00510e7d cc                       int3       
00510e7e cc                       int3       
00510e7f cc                       int3       
