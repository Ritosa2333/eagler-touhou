0047d580 55                       push       ebp
0047d581 8bec                     mov        ebp, esp
0047d583 83ec0c                   sub        esp, 0xc
0047d586 8b4508                   mov        eax, dword ptr [ebp + 8]
0047d589 8b88c8050000             mov        ecx, dword ptr [eax + 0x5c8]
0047d58f 894dfc                   mov        dword ptr [ebp - 4], ecx
0047d592 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047d595 e8a6800000               call       0x485640
0047d59a 69d058010000             imul       edx, eax, 0x158
0047d5a0 b814000000               mov        eax, 0x14
0047d5a5 6bc800                   imul       ecx, eax, 0
0047d5a8 8d940aac065c00           lea        edx, [edx + ecx + 0x5c06ac]
0047d5af b804000000               mov        eax, 4
0047d5b4 6bc800                   imul       ecx, eax, 0
0047d5b7 833c0a00                 cmp        dword ptr [edx + ecx], 0
0047d5bb 7c35                     jl         0x47d5f2
0047d5bd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047d5c0 e87b800000               call       0x485640
0047d5c5 69d058010000             imul       edx, eax, 0x158
0047d5cb 81c2ac065c00             add        edx, 0x5c06ac
0047d5d1 8955f8                   mov        dword ptr [ebp - 8], edx
0047d5d4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047d5d7 e844810000               call       0x485720
0047d5dc 6bc014                   imul       eax, eax, 0x14
0047d5df 8945f4                   mov        dword ptr [ebp - 0xc], eax
0047d5e2 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047d5e5 034df4                   add        ecx, dword ptr [ebp - 0xc]
0047d5e8 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0047d5eb 8b0491                   mov        eax, dword ptr [ecx + edx*4]
0047d5ee eb05                     jmp        0x47d5f5
0047d5f0 eb03                     jmp        0x47d5f5
0047d5f2 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0047d5f5 8be5                     mov        esp, ebp
0047d5f7 5d                       pop        ebp
0047d5f8 c3                       ret        
0047d5f9 cc                       int3       
0047d5fa cc                       int3       
0047d5fb cc                       int3       
0047d5fc cc                       int3       
0047d5fd cc                       int3       
0047d5fe cc                       int3       
0047d5ff cc                       int3       
