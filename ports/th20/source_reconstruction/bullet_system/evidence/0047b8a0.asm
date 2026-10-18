0047b8a0 55                       push       ebp
0047b8a1 8bec                     mov        ebp, esp
0047b8a3 6aff                     push       -1
0047b8a5 6800765600               push       0x567600
0047b8aa 64a100000000             mov        eax, dword ptr fs:[0]
0047b8b0 50                       push       eax
0047b8b1 51                       push       ecx
0047b8b2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047b8b7 33c5                     xor        eax, ebp
0047b8b9 50                       push       eax
0047b8ba 8d45f4                   lea        eax, [ebp - 0xc]
0047b8bd 64a300000000             mov        dword ptr fs:[0], eax
0047b8c3 894df0                   mov        dword ptr [ebp - 0x10], ecx
0047b8c6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b8c9 e85244faff               call       0x41fd20
0047b8ce 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047b8d1 c7005cfc5600             mov        dword ptr [eax], 0x56fc5c
0047b8d7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b8da c7411000000000           mov        dword ptr [ecx + 0x10], 0
0047b8e1 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047b8e4 c7421400000000           mov        dword ptr [edx + 0x14], 0
0047b8eb 33c0                     xor        eax, eax
0047b8ed 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b8f0 83c118                   add        ecx, 0x18
0047b8f3 8901                     mov        dword ptr [ecx], eax
0047b8f5 894104                   mov        dword ptr [ecx + 4], eax
0047b8f8 894108                   mov        dword ptr [ecx + 8], eax
0047b8fb 89410c                   mov        dword ptr [ecx + 0xc], eax
0047b8fe 894110                   mov        dword ptr [ecx + 0x10], eax
0047b901 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047b904 c7422c00000000           mov        dword ptr [edx + 0x2c], 0
0047b90b 33c0                     xor        eax, eax
0047b90d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b910 83c130                   add        ecx, 0x30
0047b913 8901                     mov        dword ptr [ecx], eax
0047b915 894104                   mov        dword ptr [ecx + 4], eax
0047b918 894108                   mov        dword ptr [ecx + 8], eax
0047b91b 89410c                   mov        dword ptr [ecx + 0xc], eax
0047b91e 894110                   mov        dword ptr [ecx + 0x10], eax
0047b921 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047b924 c7424400000000           mov        dword ptr [edx + 0x44], 0
0047b92b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047b92e 0f57c0                   xorps      xmm0, xmm0
0047b931 f30f114048               movss      dword ptr [eax + 0x48], xmm0
0047b936 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b939 83c14c                   add        ecx, 0x4c
0047b93c e85fdffbff               call       0x4398a0
0047b941 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b944 83c154                   add        ecx, 0x54
0047b947 e854dffbff               call       0x4398a0
0047b94c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b94f 83c160                   add        ecx, 0x60
0047b952 e839feffff               call       0x47b790
0047b957 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b95a 81c1084e2800             add        ecx, 0x284e08
0047b960 e85bfeffff               call       0x47b7c0
0047b965 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b968 81c14c6d2800             add        ecx, 0x286d4c
0047b96e e8ddd0f9ff               call       0x418a50
0047b973 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b976 81c1646d2800             add        ecx, 0x286d64
0047b97c e8cfd0f9ff               call       0x418a50
0047b981 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b984 c7817c6d280000000000     mov        dword ptr [ecx + 0x286d7c], 0
0047b98e 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047b991 c782806d280000000000     mov        dword ptr [edx + 0x286d80], 0
0047b99b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047b99e c780846d280000000000     mov        dword ptr [eax + 0x286d84], 0
0047b9a8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b9ab c781886d280000000000     mov        dword ptr [ecx + 0x286d88], 0
0047b9b5 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047b9b8 c7828c6d280000000000     mov        dword ptr [edx + 0x286d8c], 0
0047b9c2 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047b9c5 c780906d280000000000     mov        dword ptr [eax + 0x286d90], 0
0047b9cf 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b9d2 c781946d280000000000     mov        dword ptr [ecx + 0x286d94], 0
0047b9dc 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047b9df c782986d280000000000     mov        dword ptr [edx + 0x286d98], 0
0047b9e9 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047b9ec c7809c6d280000000000     mov        dword ptr [eax + 0x286d9c], 0
0047b9f6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b9f9 c781a06d280000000000     mov        dword ptr [ecx + 0x286da0], 0
0047ba03 6868fc5600               push       0x56fc68
0047ba08 e8a30cf9ff               call       0x40c6b0
0047ba0d 83c404                   add        esp, 4
0047ba10 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047ba13 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047ba16 64890d00000000           mov        dword ptr fs:[0], ecx
0047ba1d 59                       pop        ecx
0047ba1e 8be5                     mov        esp, ebp
0047ba20 5d                       pop        ebp
0047ba21 c3                       ret        
0047ba22 cc                       int3       
0047ba23 cc                       int3       
0047ba24 cc                       int3       
0047ba25 cc                       int3       
0047ba26 cc                       int3       
0047ba27 cc                       int3       
0047ba28 cc                       int3       
0047ba29 cc                       int3       
0047ba2a cc                       int3       
0047ba2b cc                       int3       
0047ba2c cc                       int3       
0047ba2d cc                       int3       
0047ba2e cc                       int3       
0047ba2f cc                       int3       
