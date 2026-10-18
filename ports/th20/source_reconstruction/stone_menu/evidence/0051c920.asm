0051c920 55                       push       ebp
0051c921 8bec                     mov        ebp, esp
0051c923 6aff                     push       -1
0051c925 682d765600               push       0x56762d
0051c92a 64a100000000             mov        eax, dword ptr fs:[0]
0051c930 50                       push       eax
0051c931 83ec0c                   sub        esp, 0xc
0051c934 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051c939 33c5                     xor        eax, ebp
0051c93b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0051c93e 50                       push       eax
0051c93f 8d45f4                   lea        eax, [ebp - 0xc]
0051c942 64a300000000             mov        dword ptr fs:[0], eax
0051c948 894de8                   mov        dword ptr [ebp - 0x18], ecx
0051c94b 6a14                     push       0x14
0051c94d b940025c00               mov        ecx, 0x5c0240
0051c952 e8b9f9eeff               call       0x40c310
0051c957 50                       push       eax
0051c958 8d4dec                   lea        ecx, [ebp - 0x14]
0051c95b e8d0edeeff               call       0x40b730
0051c960 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051c967 837d0800                 cmp        dword ptr [ebp + 8], 0
0051c96b 7c1e                     jl         0x51c98b
0051c96d 837d0804                 cmp        dword ptr [ebp + 8], 4
0051c971 7d18                     jge        0x51c98b
0051c973 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
0051c977 7c12                     jl         0x51c98b
0051c979 837d0c02                 cmp        dword ptr [ebp + 0xc], 2
0051c97d 7d0c                     jge        0x51c98b
0051c97f 837d1000                 cmp        dword ptr [ebp + 0x10], 0
0051c983 7c06                     jl         0x51c98b
0051c985 837d1009                 cmp        dword ptr [ebp + 0x10], 9
0051c989 7c12                     jl         0x51c99d
0051c98b c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051c992 8d4dec                   lea        ecx, [ebp - 0x14]
0051c995 e866efeeff               call       0x40b900
0051c99a 90                       nop        
0051c99b eb65                     jmp        0x51ca02
0051c99d b968a55b00               mov        ecx, 0x5ba568
0051c9a2 e8f976f4ff               call       0x4640a0
0051c9a7 83f804                   cmp        eax, 4
0051c9aa 7520                     jne        0x51c9cc
0051c9ac 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0051c9af e82c77f4ff               call       0x4640e0
0051c9b4 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0051c9b7 c1e104                   shl        ecx, 4
0051c9ba 8d940848010000           lea        edx, [eax + ecx + 0x148]
0051c9c1 8b4508                   mov        eax, dword ptr [ebp + 8]
0051c9c4 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0051c9c7 890c82                   mov        dword ptr [edx + eax*4], ecx
0051c9ca eb1e                     jmp        0x51c9ea
0051c9cc 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0051c9cf e80c77f4ff               call       0x4640e0
0051c9d4 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0051c9d7 c1e204                   shl        edx, 4
0051c9da 8d841028010000           lea        eax, [eax + edx + 0x128]
0051c9e1 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0051c9e4 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0051c9e7 891488                   mov        dword ptr [eax + ecx*4], edx
0051c9ea 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0051c9ed e84e21faff               call       0x4beb40
0051c9f2 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051c9f9 8d4dec                   lea        ecx, [ebp - 0x14]
0051c9fc e8ffeeeeff               call       0x40b900
0051ca01 90                       nop        
0051ca02 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051ca05 64890d00000000           mov        dword ptr fs:[0], ecx
0051ca0c 59                       pop        ecx
0051ca0d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051ca10 33cd                     xor        ecx, ebp
0051ca12 e8d75f0200               call       0x5429ee
0051ca17 8be5                     mov        esp, ebp
0051ca19 5d                       pop        ebp
0051ca1a c20c00                   ret        0xc
0051ca1d cc                       int3       
0051ca1e cc                       int3       
0051ca1f cc                       int3       
