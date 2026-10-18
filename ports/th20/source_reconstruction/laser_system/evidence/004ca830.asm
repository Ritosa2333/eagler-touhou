004ca830 55                       push       ebp
004ca831 8bec                     mov        ebp, esp
004ca833 81ec44020000             sub        esp, 0x244
004ca839 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004ca83e 33c5                     xor        eax, ebp
004ca840 8945fc                   mov        dword ptr [ebp - 4], eax
004ca843 898de0fdffff             mov        dword ptr [ebp - 0x220], ecx
004ca849 837d1400                 cmp        dword ptr [ebp + 0x14], 0
004ca84d 7416                     je         0x4ca865
004ca84f 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004ca855 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004ca85c 7407                     je         0x4ca865
004ca85e 33c0                     xor        eax, eax
004ca860 e942020000               jmp        0x4caaa7
004ca865 c785d8fdffff00000000     mov        dword ptr [ebp - 0x228], 0
004ca86f c785dcfdffff00000000     mov        dword ptr [ebp - 0x224], 0
004ca879 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004ca87f e88c85f5ff               call       0x422e10
004ca884 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004ca88a 83c158                   add        ecx, 0x58
004ca88d 8b11                     mov        edx, dword ptr [ecx]
004ca88f 8995e4fdffff             mov        dword ptr [ebp - 0x21c], edx
004ca895 8b4104                   mov        eax, dword ptr [ecx + 4]
004ca898 8985e8fdffff             mov        dword ptr [ebp - 0x218], eax
004ca89e 8b4908                   mov        ecx, dword ptr [ecx + 8]
004ca8a1 898decfdffff             mov        dword ptr [ebp - 0x214], ecx
004ca8a7 6800020000               push       0x200
004ca8ac 6a00                     push       0
004ca8ae 8d95fcfdffff             lea        edx, [ebp - 0x204]
004ca8b4 52                       push       edx
004ca8b5 e8a69e0700               call       0x544760
004ca8ba 83c40c                   add        esp, 0xc
004ca8bd f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004ca8c2 f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
004ca8c7 f30f11450c               movss      dword ptr [ebp + 0xc], xmm0
004ca8cc 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004ca8d2 8b8828130000             mov        ecx, dword ptr [eax + 0x1328]
004ca8d8 898dd4fdffff             mov        dword ptr [ebp - 0x22c], ecx
004ca8de c785dcfdffff00000000     mov        dword ptr [ebp - 0x224], 0
004ca8e8 eb0f                     jmp        0x4ca8f9
004ca8ea 8b95dcfdffff             mov        edx, dword ptr [ebp - 0x224]
004ca8f0 83c201                   add        edx, 1
004ca8f3 8995dcfdffff             mov        dword ptr [ebp - 0x224], edx
004ca8f9 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004ca8ff 8b8ddcfdffff             mov        ecx, dword ptr [ebp - 0x224]
004ca905 3b8818070000             cmp        ecx, dword ptr [eax + 0x718]
004ca90b 0f8d2d010000             jge        0x4caa3e
004ca911 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ca917 8b02                     mov        eax, dword ptr [edx]
004ca919 8985f0fdffff             mov        dword ptr [ebp - 0x210], eax
004ca91f 8b4a04                   mov        ecx, dword ptr [edx + 4]
004ca922 898df4fdffff             mov        dword ptr [ebp - 0x20c], ecx
004ca928 8b5208                   mov        edx, dword ptr [edx + 8]
004ca92b 8995f8fdffff             mov        dword ptr [ebp - 0x208], edx
004ca931 8d85f0fdffff             lea        eax, [ebp - 0x210]
004ca937 50                       push       eax
004ca938 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ca93b 51                       push       ecx
004ca93c e88fb7f8ff               call       0x4560d0
004ca941 83c408                   add        esp, 8
004ca944 d99dccfdffff             fstp       dword ptr [ebp - 0x234]
004ca94a f30f1085ccfdffff         movss      xmm0, dword ptr [ebp - 0x234]
004ca952 0f2f450c                 comiss     xmm0, dword ptr [ebp + 0xc]
004ca956 760a                     jbe        0x4ca962
004ca958 e9cd000000               jmp        0x4caa2a
004ca95d e9c8000000               jmp        0x4caa2a
004ca962 8b95dcfdffff             mov        edx, dword ptr [ebp - 0x224]
004ca968 c68415fcfdffff01         mov        byte ptr [ebp + edx - 0x204], 1
004ca970 8b85d8fdffff             mov        eax, dword ptr [ebp - 0x228]
004ca976 83c001                   add        eax, 1
004ca979 8985d8fdffff             mov        dword ptr [ebp - 0x228], eax
004ca97f 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004ca985 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004ca98b e87019f4ff               call       0x40c300
004ca990 05886d2800               add        eax, 0x286d88
004ca995 8985d0fdffff             mov        dword ptr [ebp - 0x230], eax
004ca99b 8b95d0fdffff             mov        edx, dword ptr [ebp - 0x230]
004ca9a1 8b02                     mov        eax, dword ptr [edx]
004ca9a3 83c001                   add        eax, 1
004ca9a6 8b8dd0fdffff             mov        ecx, dword ptr [ebp - 0x230]
004ca9ac 8901                     mov        dword ptr [ecx], eax
004ca9ae 8b85dcfdffff             mov        eax, dword ptr [ebp - 0x224]
004ca9b4 99                       cdq        
004ca9b5 b914000000               mov        ecx, 0x14
004ca9ba f7f9                     idiv       ecx
004ca9bc 85d2                     test       edx, edx
004ca9be 756a                     jne        0x4caa2a
004ca9c0 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004ca9c6 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004ca9cc e82f19f4ff               call       0x40c300
004ca9d1 8bc8                     mov        ecx, eax
004ca9d3 e8d8affbff               call       0x4859b0
004ca9d8 8985c4fdffff             mov        dword ptr [ebp - 0x23c], eax
004ca9de 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004ca9e4 8b8814070000             mov        ecx, dword ptr [eax + 0x714]
004ca9ea 8d9409d4000000           lea        edx, [ecx + ecx + 0xd4]
004ca9f1 8995c8fdffff             mov        dword ptr [ebp - 0x238], edx
004ca9f7 6a00                     push       0
004ca9f9 6aff                     push       -1
004ca9fb 51                       push       ecx
004ca9fc 0f57c0                   xorps      xmm0, xmm0
004ca9ff f30f110424               movss      dword ptr [esp], xmm0
004caa04 8d85f0fdffff             lea        eax, [ebp - 0x210]
004caa0a 50                       push       eax
004caa0b 8b8dc8fdffff             mov        ecx, dword ptr [ebp - 0x238]
004caa11 51                       push       ecx
004caa12 6814fd5600               push       0x56fd14
004caa17 8d95bcfdffff             lea        edx, [ebp - 0x244]
004caa1d 52                       push       edx
004caa1e 8b8dc4fdffff             mov        ecx, dword ptr [ebp - 0x23c]
004caa24 e8b7e6faff               call       0x4790e0
004caa29 90                       nop        
004caa2a 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004caa30 83c020                   add        eax, 0x20
004caa33 8985d4fdffff             mov        dword ptr [ebp - 0x22c], eax
004caa39 e9acfeffff               jmp        0x4ca8ea
004caa3e 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004caa44 8b95d8fdffff             mov        edx, dword ptr [ebp - 0x228]
004caa4a 3b9118070000             cmp        edx, dword ptr [ecx + 0x718]
004caa50 7c1a                     jl         0x4caa6c
004caa52 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004caa58 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004caa5b 83e1f9                   and        ecx, 0xfffffff9
004caa5e 83c902                   or         ecx, 2
004caa61 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004caa67 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004caa6a eb35                     jmp        0x4caaa1
004caa6c 83bdd8fdffff00           cmp        dword ptr [ebp - 0x228], 0
004caa73 7e2c                     jle        0x4caaa1
004caa75 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004caa7b 8b08                     mov        ecx, dword ptr [eax]
004caa7d 8b5138                   mov        edx, dword ptr [ecx + 0x38]
004caa80 8995c0fdffff             mov        dword ptr [ebp - 0x240], edx
004caa86 8b85dcfdffff             mov        eax, dword ptr [ebp - 0x224]
004caa8c 50                       push       eax
004caa8d 8d8dfcfdffff             lea        ecx, [ebp - 0x204]
004caa93 51                       push       ecx
004caa94 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004caa9a ff95c0fdffff             call       dword ptr [ebp - 0x240]
004caaa0 90                       nop        
004caaa1 8b85d8fdffff             mov        eax, dword ptr [ebp - 0x228]
004caaa7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004caaaa 33cd                     xor        ecx, ebp
004caaac e83d7f0700               call       0x5429ee
004caab1 8be5                     mov        esp, ebp
004caab3 5d                       pop        ebp
004caab4 c21000                   ret        0x10
004caab7 cc                       int3       
004caab8 cc                       int3       
004caab9 cc                       int3       
004caaba cc                       int3       
004caabb cc                       int3       
004caabc cc                       int3       
004caabd cc                       int3       
004caabe cc                       int3       
004caabf cc                       int3       
