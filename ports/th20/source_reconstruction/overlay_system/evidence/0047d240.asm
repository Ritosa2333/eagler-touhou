0047d240 55                       push       ebp
0047d241 8bec                     mov        ebp, esp
0047d243 6aff                     push       -1
0047d245 68d5905600               push       0x5690d5
0047d24a 64a100000000             mov        eax, dword ptr fs:[0]
0047d250 50                       push       eax
0047d251 83ec28                   sub        esp, 0x28
0047d254 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047d259 33c5                     xor        eax, ebp
0047d25b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0047d25e 50                       push       eax
0047d25f 8d45f4                   lea        eax, [ebp - 0xc]
0047d262 64a300000000             mov        dword ptr fs:[0], eax
0047d268 894de0                   mov        dword ptr [ebp - 0x20], ecx
0047d26b c745fc00000000           mov        dword ptr [ebp - 4], 0
0047d272 c745dc00000000           mov        dword ptr [ebp - 0x24], 0
0047d279 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d27c e8ef860000               call       0x485970
0047d281 8945d8                   mov        dword ptr [ebp - 0x28], eax
0047d284 6a08                     push       8
0047d286 8d4de8                   lea        ecx, [ebp - 0x18]
0047d289 e8f2edf8ff               call       0x40c080
0047d28e 8d45e8                   lea        eax, [ebp - 0x18]
0047d291 50                       push       eax
0047d292 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0047d295 e8e64ff9ff               call       0x412280
0047d29a c645fc01                 mov        byte ptr [ebp - 4], 1
0047d29e 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0047d2a1 e89a52f9ff               call       0x412540
0047d2a6 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0047d2a9 eb09                     jmp        0x47d2b4
0047d2ab 8d4de8                   lea        ecx, [ebp - 0x18]
0047d2ae e87d49f9ff               call       0x411c30
0047d2b3 90                       nop        
0047d2b4 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0047d2b7 51                       push       ecx
0047d2b8 8d4de8                   lea        ecx, [ebp - 0x18]
0047d2bb e8f048f9ff               call       0x411bb0
0047d2c0 0fb6d0                   movzx      edx, al
0047d2c3 85d2                     test       edx, edx
0047d2c5 0f84b9000000             je         0x47d384
0047d2cb 8d4de8                   lea        ecx, [ebp - 0x18]
0047d2ce e82df0f8ff               call       0x40c300
0047d2d3 8945d0                   mov        dword ptr [ebp - 0x30], eax
0047d2d6 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0047d2d9 e822f0f8ff               call       0x40c300
0047d2de 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0047d2e1 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d2e4 83785002                 cmp        dword ptr [eax + 0x50], 2
0047d2e8 740b                     je         0x47d2f5
0047d2ea 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d2ed 83795001                 cmp        dword ptr [ecx + 0x50], 1
0047d2f1 7402                     je         0x47d2f5
0047d2f3 ebb6                     jmp        0x47d2ab
0047d2f5 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d2f8 837a1800                 cmp        dword ptr [edx + 0x18], 0
0047d2fc 7402                     je         0x47d300
0047d2fe ebab                     jmp        0x47d2ab
0047d300 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d303 f30f10407c               movss      xmm0, dword ptr [eax + 0x7c]
0047d308 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0047d310 f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
0047d315 51                       push       ecx
0047d316 f30f110424               movss      dword ptr [esp], xmm0
0047d31b 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047d31e 51                       push       ecx
0047d31f 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d322 83c264                   add        edx, 0x64
0047d325 52                       push       edx
0047d326 e835870000               call       0x485a60
0047d32b 83c40c                   add        esp, 0xc
0047d32e 85c0                     test       eax, eax
0047d330 744d                     je         0x47d37f
0047d332 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d335 83c064                   add        eax, 0x64
0047d338 50                       push       eax
0047d339 8d4d10                   lea        ecx, [ebp + 0x10]
0047d33c e8efc2fcff               call       0x449630
0047d341 85c0                     test       eax, eax
0047d343 743a                     je         0x47d37f
0047d345 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d348 8b5114                   mov        edx, dword ptr [ecx + 0x14]
0047d34b 81e2ffe7ffff             and        edx, 0xffffe7ff
0047d351 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d354 895014                   mov        dword ptr [eax + 0x14], edx
0047d357 6a00                     push       0
0047d359 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d35c e88ff5ffff               call       0x47c8f0
0047d361 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d364 8b91886d2800             mov        edx, dword ptr [ecx + 0x286d88]
0047d36a 83c201                   add        edx, 1
0047d36d 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0047d370 8990886d2800             mov        dword ptr [eax + 0x286d88], edx
0047d376 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0047d379 83c101                   add        ecx, 1
0047d37c 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0047d37f e927ffffff               jmp        0x47d2ab
0047d384 c645fc00                 mov        byte ptr [ebp - 4], 0
0047d388 8d4de8                   lea        ecx, [ebp - 0x18]
0047d38b e87047f9ff               call       0x411b00
0047d390 90                       nop        
0047d391 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0047d394 8955cc                   mov        dword ptr [ebp - 0x34], edx
0047d397 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0047d39e 8d4d10                   lea        ecx, [ebp + 0x10]
0047d3a1 e85a70f9ff               call       0x414400
0047d3a6 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
0047d3a9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047d3ac 64890d00000000           mov        dword ptr fs:[0], ecx
0047d3b3 59                       pop        ecx
0047d3b4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047d3b7 33cd                     xor        ecx, ebp
0047d3b9 e830560c00               call       0x5429ee
0047d3be 8be5                     mov        esp, ebp
0047d3c0 5d                       pop        ebp
0047d3c1 c23000                   ret        0x30
0047d3c4 cc                       int3       
0047d3c5 cc                       int3       
0047d3c6 cc                       int3       
0047d3c7 cc                       int3       
0047d3c8 cc                       int3       
0047d3c9 cc                       int3       
0047d3ca cc                       int3       
0047d3cb cc                       int3       
0047d3cc cc                       int3       
0047d3cd cc                       int3       
0047d3ce cc                       int3       
0047d3cf cc                       int3       
