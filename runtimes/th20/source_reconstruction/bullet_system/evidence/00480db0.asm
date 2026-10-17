00480db0 55                       push       ebp
00480db1 8bec                     mov        ebp, esp
00480db3 83ec68                   sub        esp, 0x68
00480db6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00480dbb 33c5                     xor        eax, ebp
00480dbd 8945fc                   mov        dword ptr [ebp - 4], eax
00480dc0 894dec                   mov        dword ptr [ebp - 0x14], ecx
00480dc3 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00480dc6 05a0000000               add        eax, 0xa0
00480dcb 8945e8                   mov        dword ptr [ebp - 0x18], eax
00480dce 6a07                     push       7
00480dd0 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00480dd3 e8a8b8ffff               call       0x47c680
00480dd8 8945dc                   mov        dword ptr [ebp - 0x24], eax
00480ddb 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00480dde 81c1a0000000             add        ecx, 0xa0
00480de4 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00480de7 6a07                     push       7
00480de9 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00480dec e88fb8ffff               call       0x47c680
00480df1 8b5030                   mov        edx, dword ptr [eax + 0x30]
00480df4 8955e0                   mov        dword ptr [ebp - 0x20], edx
00480df7 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00480dfa 50                       push       eax
00480dfb 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00480dfe e8bd27faff               call       0x4235c0
00480e03 0fb6c8                   movzx      ecx, al
00480e06 85c9                     test       ecx, ecx
00480e08 7426                     je         0x480e30
00480e0a 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00480e0d 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00480e13 25ffffff7f               and        eax, 0x7fffffff
00480e18 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00480e1b 898190000000             mov        dword ptr [ecx + 0x90], eax
00480e21 b801000000               mov        eax, 1
00480e26 e964010000               jmp        0x480f8f
00480e2b e93b010000               jmp        0x480f6b
00480e30 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00480e33 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
00480e39 e852f1f8ff               call       0x40ff90
00480e3e 8945d8                   mov        dword ptr [ebp - 0x28], eax
00480e41 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00480e44 83c064                   add        eax, 0x64
00480e47 50                       push       eax
00480e48 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00480e4b e800e50700               call       0x4ff350
00480e50 d95dd0                   fstp       dword ptr [ebp - 0x30]
00480e53 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00480e56 81c1a0000000             add        ecx, 0xa0
00480e5c 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00480e5f 6a07                     push       7
00480e61 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00480e64 e817b8ffff               call       0x47c680
00480e69 f30f104014               movss      xmm0, dword ptr [eax + 0x14]
00480e6e f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
00480e73 51                       push       ecx
00480e74 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00480e79 f30f110424               movss      dword ptr [esp], xmm0
00480e7e 51                       push       ecx
00480e7f f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00480e84 f30f110424               movss      dword ptr [esp], xmm0
00480e89 e87277fbff               call       0x438600
00480e8e 83c408                   add        esp, 8
00480e91 d95dc4                   fstp       dword ptr [ebp - 0x3c]
00480e94 8d4df0                   lea        ecx, [ebp - 0x10]
00480e97 e8741ffaff               call       0x422e10
00480e9c 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00480e9f 81c2a0000000             add        edx, 0xa0
00480ea5 8955c8                   mov        dword ptr [ebp - 0x38], edx
00480ea8 6a07                     push       7
00480eaa 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00480ead e8ceb7ffff               call       0x47c680
00480eb2 51                       push       ecx
00480eb3 f30f104010               movss      xmm0, dword ptr [eax + 0x10]
00480eb8 f30f110424               movss      dword ptr [esp], xmm0
00480ebd 51                       push       ecx
00480ebe f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
00480ec3 f30f110424               movss      dword ptr [esp], xmm0
00480ec8 8d45f0                   lea        eax, [ebp - 0x10]
00480ecb 50                       push       eax
00480ecc e85f84fbff               call       0x439330
00480ed1 83c40c                   add        esp, 0xc
00480ed4 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00480ed7 83c170                   add        ecx, 0x70
00480eda 51                       push       ecx
00480edb 8d55a4                   lea        edx, [ebp - 0x5c]
00480ede 52                       push       edx
00480edf 8d4df0                   lea        ecx, [ebp - 0x10]
00480ee2 e85985faff               call       0x429440
00480ee7 8945b8                   mov        dword ptr [ebp - 0x48], eax
00480eea 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00480eed 05a0000000               add        eax, 0xa0
00480ef2 8945c0                   mov        dword ptr [ebp - 0x40], eax
00480ef5 6a07                     push       7
00480ef7 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00480efa e881b7ffff               call       0x47c680
00480eff f30f104024               movss      xmm0, dword ptr [eax + 0x24]
00480f04 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
00480f09 51                       push       ecx
00480f0a f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
00480f0f f30f110424               movss      dword ptr [esp], xmm0
00480f14 8d4d98                   lea        ecx, [ebp - 0x68]
00480f17 51                       push       ecx
00480f18 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00480f1b e8d083faff               call       0x4292f0
00480f20 50                       push       eax
00480f21 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00480f24 83c170                   add        ecx, 0x70
00480f27 e8b487faff               call       0x4296e0
00480f2c 90                       nop        
00480f2d 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00480f30 0f57c0                   xorps      xmm0, xmm0
00480f33 f30f114278               movss      dword ptr [edx + 0x78], xmm0
00480f38 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00480f3b 83c070                   add        eax, 0x70
00480f3e 50                       push       eax
00480f3f e85c93ffff               call       0x47a2a0
00480f44 83c404                   add        esp, 4
00480f47 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00480f4a d95920                   fstp       dword ptr [ecx + 0x20]
00480f4d 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00480f50 83c270                   add        edx, 0x70
00480f53 52                       push       edx
00480f54 e8b752fdff               call       0x456210
00480f59 d91c24                   fstp       dword ptr [esp]
00480f5c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00480f5f 81c184000000             add        ecx, 0x84
00480f65 e8b61ffdff               call       0x452f20
00480f6a 90                       nop        
00480f6b 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00480f6e 05a0000000               add        eax, 0xa0
00480f73 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00480f76 6a07                     push       7
00480f78 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00480f7b e800b7ffff               call       0x47c680
00480f80 8945b0                   mov        dword ptr [ebp - 0x50], eax
00480f83 6a00                     push       0
00480f85 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00480f88 e8b325faff               call       0x423540
00480f8d 33c0                     xor        eax, eax
00480f8f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00480f92 33cd                     xor        ecx, ebp
00480f94 e8551a0c00               call       0x5429ee
00480f99 8be5                     mov        esp, ebp
00480f9b 5d                       pop        ebp
00480f9c c3                       ret        
00480f9d cc                       int3       
00480f9e cc                       int3       
00480f9f cc                       int3       
