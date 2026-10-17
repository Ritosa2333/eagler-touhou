0051e1c0 55                       push       ebp
0051e1c1 8bec                     mov        ebp, esp
0051e1c3 6aff                     push       -1
0051e1c5 687dae5600               push       0x56ae7d
0051e1ca 64a100000000             mov        eax, dword ptr fs:[0]
0051e1d0 50                       push       eax
0051e1d1 83ec48                   sub        esp, 0x48
0051e1d4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051e1d9 33c5                     xor        eax, ebp
0051e1db 8945f0                   mov        dword ptr [ebp - 0x10], eax
0051e1de 50                       push       eax
0051e1df 8d45f4                   lea        eax, [ebp - 0xc]
0051e1e2 64a300000000             mov        dword ptr fs:[0], eax
0051e1e8 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
0051e1eb 6a14                     push       0x14
0051e1ed b940025c00               mov        ecx, 0x5c0240
0051e1f2 e819e1eeff               call       0x40c310
0051e1f7 50                       push       eax
0051e1f8 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051e1fb e830d5eeff               call       0x40b730
0051e200 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051e207 c745d000000000           mov        dword ptr [ebp - 0x30], 0
0051e20e eb09                     jmp        0x51e219
0051e210 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
0051e213 83c001                   add        eax, 1
0051e216 8945d0                   mov        dword ptr [ebp - 0x30], eax
0051e219 837dd071                 cmp        dword ptr [ebp - 0x30], 0x71
0051e21d 7d2c                     jge        0x51e24b
0051e21f 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0051e222 e8d9a4f6ff               call       0x488700
0051e227 05080b0000               add        eax, 0xb08
0051e22c 8945b0                   mov        dword ptr [ebp - 0x50], eax
0051e22f 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0051e232 51                       push       ecx
0051e233 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0051e236 e8d58bf6ff               call       0x486e10
0051e23b 8945ac                   mov        dword ptr [ebp - 0x54], eax
0051e23e 6a00                     push       0
0051e240 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0051e243 e848a3f6ff               call       0x488590
0051e248 90                       nop        
0051e249 ebc5                     jmp        0x51e210
0051e24b c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0051e252 8d55e4                   lea        edx, [ebp - 0x1c]
0051e255 8955c0                   mov        dword ptr [ebp - 0x40], edx
0051e258 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
0051e25b 8b08                     mov        ecx, dword ptr [eax]
0051e25d 51                       push       ecx
0051e25e 8d55ec                   lea        edx, [ebp - 0x14]
0051e261 52                       push       edx
0051e262 e8a932f4ff               call       0x461510
0051e267 83c408                   add        esp, 8
0051e26a 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
0051e26d 8b08                     mov        ecx, dword ptr [eax]
0051e26f 51                       push       ecx
0051e270 8d55e0                   lea        edx, [ebp - 0x20]
0051e273 52                       push       edx
0051e274 e8e7cbfeff               call       0x50ae60
0051e279 83c408                   add        esp, 8
0051e27c eb09                     jmp        0x51e287
0051e27e 8d4dec                   lea        ecx, [ebp - 0x14]
0051e281 e8da35f4ff               call       0x461860
0051e286 90                       nop        
0051e287 8d45e0                   lea        eax, [ebp - 0x20]
0051e28a 50                       push       eax
0051e28b 8d4dec                   lea        ecx, [ebp - 0x14]
0051e28e e8cdb1f2ff               call       0x449460
0051e293 0fb6c8                   movzx      ecx, al
0051e296 85c9                     test       ecx, ecx
0051e298 0f84ea000000             je         0x51e388
0051e29e 8d4dec                   lea        ecx, [ebp - 0x14]
0051e2a1 e85ae0eeff               call       0x40c300
0051e2a6 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0051e2a9 c745dc00000000           mov        dword ptr [ebp - 0x24], 0
0051e2b0 8d55dc                   lea        edx, [ebp - 0x24]
0051e2b3 8955bc                   mov        dword ptr [ebp - 0x44], edx
0051e2b6 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
0051e2b9 8b08                     mov        ecx, dword ptr [eax]
0051e2bb 51                       push       ecx
0051e2bc 8d55e8                   lea        edx, [ebp - 0x18]
0051e2bf 52                       push       edx
0051e2c0 e84b32f4ff               call       0x461510
0051e2c5 83c408                   add        esp, 8
0051e2c8 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
0051e2cb 8b08                     mov        ecx, dword ptr [eax]
0051e2cd 51                       push       ecx
0051e2ce 8d55d8                   lea        edx, [ebp - 0x28]
0051e2d1 52                       push       edx
0051e2d2 e8a9cbfeff               call       0x50ae80
0051e2d7 83c408                   add        esp, 8
0051e2da eb09                     jmp        0x51e2e5
0051e2dc 8d4de8                   lea        ecx, [ebp - 0x18]
0051e2df e87c35f4ff               call       0x461860
0051e2e4 90                       nop        
0051e2e5 8d45d8                   lea        eax, [ebp - 0x28]
0051e2e8 50                       push       eax
0051e2e9 8d4de8                   lea        ecx, [ebp - 0x18]
0051e2ec e86fb1f2ff               call       0x449460
0051e2f1 0fb6c8                   movzx      ecx, al
0051e2f4 85c9                     test       ecx, ecx
0051e2f6 0f8487000000             je         0x51e383
0051e2fc 8d4de8                   lea        ecx, [ebp - 0x18]
0051e2ff e8fcdfeeff               call       0x40c300
0051e304 8945b8                   mov        dword ptr [ebp - 0x48], eax
0051e307 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
0051e30a 52                       push       edx
0051e30b 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
0051e30e 50                       push       eax
0051e30f 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0051e312 e849f1f9ff               call       0x4bd460
0051e317 c780f076000001000000     mov        dword ptr [eax + 0x76f0], 1
0051e321 c745cc00000000           mov        dword ptr [ebp - 0x34], 0
0051e328 eb09                     jmp        0x51e333
0051e32a 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0051e32d 83c101                   add        ecx, 1
0051e330 894dcc                   mov        dword ptr [ebp - 0x34], ecx
0051e333 837dcc06                 cmp        dword ptr [ebp - 0x34], 6
0051e337 7d45                     jge        0x51e37e
0051e339 c745c800000000           mov        dword ptr [ebp - 0x38], 0
0051e340 eb09                     jmp        0x51e34b
0051e342 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0051e345 83c201                   add        edx, 1
0051e348 8955c8                   mov        dword ptr [ebp - 0x38], edx
0051e34b 837dc808                 cmp        dword ptr [ebp - 0x38], 8
0051e34f 7d2b                     jge        0x51e37c
0051e351 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
0051e354 50                       push       eax
0051e355 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0051e358 51                       push       ecx
0051e359 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0051e35c e8fff0f9ff               call       0x4bd460
0051e361 6955cc90000000           imul       edx, dword ptr [ebp - 0x34], 0x90
0051e368 8d8410f8760000           lea        eax, [eax + edx + 0x76f8]
0051e36f 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0051e372 c1e104                   shl        ecx, 4
0051e375 c644080901               mov        byte ptr [eax + ecx + 9], 1
0051e37a ebc6                     jmp        0x51e342
0051e37c ebac                     jmp        0x51e32a
0051e37e e959ffffff               jmp        0x51e2dc
0051e383 e9f6feffff               jmp        0x51e27e
0051e388 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0051e38b e8b007faff               call       0x4beb40
0051e390 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051e397 8d4dd4                   lea        ecx, [ebp - 0x2c]
0051e39a e861d5eeff               call       0x40b900
0051e39f 90                       nop        
0051e3a0 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051e3a3 64890d00000000           mov        dword ptr fs:[0], ecx
0051e3aa 59                       pop        ecx
0051e3ab 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051e3ae 33cd                     xor        ecx, ebp
0051e3b0 e839460200               call       0x5429ee
0051e3b5 8be5                     mov        esp, ebp
0051e3b7 5d                       pop        ebp
0051e3b8 c3                       ret        
0051e3b9 cc                       int3       
0051e3ba cc                       int3       
0051e3bb cc                       int3       
0051e3bc cc                       int3       
0051e3bd cc                       int3       
0051e3be cc                       int3       
0051e3bf cc                       int3       
