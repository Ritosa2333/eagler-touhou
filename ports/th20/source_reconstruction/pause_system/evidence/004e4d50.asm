004e4d50 55                       push       ebp
004e4d51 8bec                     mov        ebp, esp
004e4d53 83ec38                   sub        esp, 0x38
004e4d56 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004e4d5b 33c5                     xor        eax, ebp
004e4d5d 8945fc                   mov        dword ptr [ebp - 4], eax
004e4d60 894de0                   mov        dword ptr [ebp - 0x20], ecx
004e4d63 6a01                     push       1
004e4d65 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e4d6b e8b01c0000               call       0x4e6a20
004e4d70 90                       nop        
004e4d71 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e4d74 81c1cc000000             add        ecx, 0xcc
004e4d7a e8c155f9ff               call       0x47a340
004e4d7f 0fb6c0                   movzx      eax, al
004e4d82 85c0                     test       eax, eax
004e4d84 7441                     je         0x4e4dc7
004e4d86 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e4d89 81c1cc000000             add        ecx, 0xcc
004e4d8f 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004e4d92 6a00                     push       0
004e4d94 6a5c                     push       0x5c
004e4d96 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004e4d99 e8d278f6ff               call       0x44c670
004e4d9e 8945d8                   mov        dword ptr [ebp - 0x28], eax
004e4da1 837dd800                 cmp        dword ptr [ebp - 0x28], 0
004e4da5 7420                     je         0x4e4dc7
004e4da7 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004e4daa e83109f6ff               call       0x4456e0
004e4daf 0d000000ff               or         eax, 0xff000000
004e4db4 8945d0                   mov        dword ptr [ebp - 0x30], eax
004e4db7 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004e4dba 52                       push       edx
004e4dbb 8b0df44e5c00             mov        ecx, dword ptr [0x5c4ef4]
004e4dc1 e84aacf6ff               call       0x44fa10
004e4dc6 90                       nop        
004e4dc7 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004e4dca 8b88d0000000             mov        ecx, dword ptr [eax + 0xd0]
004e4dd0 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004e4dd3 837ddc01                 cmp        dword ptr [ebp - 0x24], 1
004e4dd7 7411                     je         0x4e4dea
004e4dd9 837ddc02                 cmp        dword ptr [ebp - 0x24], 2
004e4ddd 7446                     je         0x4e4e25
004e4ddf 837ddc03                 cmp        dword ptr [ebp - 0x24], 3
004e4de3 7405                     je         0x4e4dea
004e4de5 e914020000               jmp        0x4e4ffe
004e4dea 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004e4ded 8b82d0020000             mov        eax, dword ptr [edx + 0x2d0]
004e4df3 83e003                   and        eax, 3
004e4df6 83f801                   cmp        eax, 1
004e4df9 750b                     jne        0x4e4e06
004e4dfb 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e4dfe e83d040000               call       0x4e5240
004e4e03 90                       nop        
004e4e04 eb1a                     jmp        0x4e4e20
004e4e06 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e4e09 8b91d0020000             mov        edx, dword ptr [ecx + 0x2d0]
004e4e0f 83e203                   and        edx, 3
004e4e12 83fa02                   cmp        edx, 2
004e4e15 7509                     jne        0x4e4e20
004e4e17 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e4e1a e8a1060000               call       0x4e54c0
004e4e1f 90                       nop        
004e4e20 e9d9010000               jmp        0x4e4ffe
004e4e25 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004e4e28 83b8d80000000f           cmp        dword ptr [eax + 0xd8], 0xf
004e4e2f 7509                     jne        0x4e4e3a
004e4e31 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e4e34 e8e7070000               call       0x4e5620
004e4e39 90                       nop        
004e4e3a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e4e3d 8b91d0020000             mov        edx, dword ptr [ecx + 0x2d0]
004e4e43 83e203                   and        edx, 3
004e4e46 83fa01                   cmp        edx, 1
004e4e49 750e                     jne        0x4e4e59
004e4e4b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e4e4e e8ed030000               call       0x4e5240
004e4e53 90                       nop        
004e4e54 e99b000000               jmp        0x4e4ef4
004e4e59 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004e4e5c 8b88d0020000             mov        ecx, dword ptr [eax + 0x2d0]
004e4e62 83e103                   and        ecx, 3
004e4e65 83f902                   cmp        ecx, 2
004e4e68 750b                     jne        0x4e4e75
004e4e6a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e4e6d e84e060000               call       0x4e54c0
004e4e72 90                       nop        
004e4e73 eb7f                     jmp        0x4e4ef4
004e4e75 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004e4e78 83bad80000000e           cmp        dword ptr [edx + 0xd8], 0xe
004e4e7f 7473                     je         0x4e4ef4
004e4e81 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004e4e84 83b8d800000010           cmp        dword ptr [eax + 0xd8], 0x10
004e4e8b 7467                     je         0x4e4ef4
004e4e8d b968a55b00               mov        ecx, 0x5ba568
004e4e92 e8b939faff               call       0x488850
004e4e97 0fb6c8                   movzx      ecx, al
004e4e9a 85c9                     test       ecx, ecx
004e4e9c 7556                     jne        0x4e4ef4
004e4e9e 51                       push       ecx
004e4e9f 0f57c0                   xorps      xmm0, xmm0
004e4ea2 f30f110424               movss      dword ptr [esp], xmm0
004e4ea7 51                       push       ecx
004e4ea8 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
004e4eb0 f30f110424               movss      dword ptr [esp], xmm0
004e4eb5 51                       push       ecx
004e4eb6 f30f10055c265700         movss      xmm0, dword ptr [0x57265c]
004e4ebe f30f110424               movss      dword ptr [esp], xmm0
004e4ec3 8d4df0                   lea        ecx, [ebp - 0x10]
004e4ec6 e805dff3ff               call       0x422dd0
004e4ecb b968a55b00               mov        ecx, 0x5ba568
004e4ed0 e8fb170000               call       0x4e66d0
004e4ed5 8945cc                   mov        dword ptr [ebp - 0x34], eax
004e4ed8 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
004e4edb 52                       push       edx
004e4edc 68f8255700               push       0x5725f8
004e4ee1 8d45f0                   lea        eax, [ebp - 0x10]
004e4ee4 50                       push       eax
004e4ee5 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e4eeb 51                       push       ecx
004e4eec e89f7af8ff               call       0x46c990
004e4ef1 83c410                   add        esp, 0x10
004e4ef4 b968a55b00               mov        ecx, 0x5ba568
004e4ef9 e862fef8ff               call       0x474d60
004e4efe 83f804                   cmp        eax, 4
004e4f01 0f8cf7000000             jl         0x4e4ffe
004e4f07 b968a55b00               mov        ecx, 0x5ba568
004e4f0c e84ffef8ff               call       0x474d60
004e4f11 83f806                   cmp        eax, 6
004e4f14 0f8fe4000000             jg         0x4e4ffe
004e4f1a b968a55b00               mov        ecx, 0x5ba568
004e4f1f e8fc88fdff               call       0x4bd820
004e4f24 0fb6d0                   movzx      edx, al
004e4f27 85d2                     test       edx, edx
004e4f29 0f85cf000000             jne        0x4e4ffe
004e4f2f 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004e4f32 83b8d800000006           cmp        dword ptr [eax + 0xd8], 6
004e4f39 0f85bf000000             jne        0x4e4ffe
004e4f3f 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004e4f45 e8e638faff               call       0x488830
004e4f4a 85c0                     test       eax, eax
004e4f4c 0f85ac000000             jne        0x4e4ffe
004e4f52 6a00                     push       0
004e4f54 e827f1f7ff               call       0x464080
004e4f59 83c404                   add        esp, 4
004e4f5c 8bc8                     mov        ecx, eax
004e4f5e e84d7bf3ff               call       0x41cab0
004e4f63 8945c8                   mov        dword ptr [ebp - 0x38], eax
004e4f66 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004e4f69 51                       push       ecx
004e4f6a 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004e4f70 e89b86fdff               call       0x4bd610
004e4f75 83f809                   cmp        eax, 9
004e4f78 0f8d80000000             jge        0x4e4ffe
004e4f7e 51                       push       ecx
004e4f7f 0f57c0                   xorps      xmm0, xmm0
004e4f82 f30f110424               movss      dword ptr [esp], xmm0
004e4f87 51                       push       ecx
004e4f88 f30f100560265700         movss      xmm0, dword ptr [0x572660]
004e4f90 f30f110424               movss      dword ptr [esp], xmm0
004e4f95 51                       push       ecx
004e4f96 f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
004e4f9e f30f110424               movss      dword ptr [esp], xmm0
004e4fa3 8d4de4                   lea        ecx, [ebp - 0x1c]
004e4fa6 e825def3ff               call       0x422dd0
004e4fab 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e4fb1 e82a180000               call       0x4e67e0
004e4fb6 6a80                     push       -0x80
004e4fb8 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e4fbe e80dbbf8ff               call       0x470ad0
004e4fc3 6a01                     push       1
004e4fc5 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e4fcb e8501a0000               call       0x4e6a20
004e4fd0 90                       nop        
004e4fd1 83ec08                   sub        esp, 8
004e4fd4 8bcc                     mov        ecx, esp
004e4fd6 6804265700               push       0x572604
004e4fdb e8f08df2ff               call       0x40ddd0
004e4fe0 8d55e4                   lea        edx, [ebp - 0x1c]
004e4fe3 52                       push       edx
004e4fe4 a198065c00               mov        eax, dword ptr [0x5c0698]
004e4fe9 50                       push       eax
004e4fea e82172f8ff               call       0x46c210
004e4fef 83c410                   add        esp, 0x10
004e4ff2 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e4ff8 e8e3170000               call       0x4e67e0
004e4ffd 90                       nop        
004e4ffe 6a00                     push       0
004e5000 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e5006 e8151a0000               call       0x4e6a20
004e500b b801000000               mov        eax, 1
004e5010 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5013 33cd                     xor        ecx, ebp
004e5015 e8d4d90500               call       0x5429ee
004e501a 8be5                     mov        esp, ebp
004e501c 5d                       pop        ebp
004e501d c3                       ret        
004e501e cc                       int3       
004e501f cc                       int3       
