00464480 55                       push       ebp
00464481 8bec                     mov        ebp, esp
00464483 83ec08                   sub        esp, 8
00464486 894dfc                   mov        dword ptr [ebp - 4], ecx
00464489 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046448c 83783000                 cmp        dword ptr [eax + 0x30], 0
00464490 7423                     je         0x4644b5
00464492 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00464495 8b5130                   mov        edx, dword ptr [ecx + 0x30]
00464498 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046449b 8b12                     mov        edx, dword ptr [edx]
0046449d 8b4830                   mov        ecx, dword ptr [eax + 0x30]
004644a0 8b426c                   mov        eax, dword ptr [edx + 0x6c]
004644a3 ffd0                     call       eax
004644a5 0fb6c8                   movzx      ecx, al
004644a8 85c9                     test       ecx, ecx
004644aa 7409                     je         0x4644b5
004644ac c745f801000000           mov        dword ptr [ebp - 8], 1
004644b3 eb07                     jmp        0x4644bc
004644b5 c745f800000000           mov        dword ptr [ebp - 8], 0
004644bc 0fb645f8                 movzx      eax, byte ptr [ebp - 8]
004644c0 8be5                     mov        esp, ebp
004644c2 5d                       pop        ebp
004644c3 c3                       ret        
004644c4 cc                       int3       
004644c5 cc                       int3       
004644c6 cc                       int3       
004644c7 cc                       int3       
004644c8 cc                       int3       
004644c9 cc                       int3       
004644ca cc                       int3       
004644cb cc                       int3       
004644cc cc                       int3       
004644cd cc                       int3       
004644ce cc                       int3       
004644cf cc                       int3       
