0053a170 55                       push       ebp
0053a171 8bec                     mov        ebp, esp
0053a173 51                       push       ecx
0053a174 894dfc                   mov        dword ptr [ebp - 4], ecx
0053a177 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053a17a 83780800                 cmp        dword ptr [eax + 8], 0
0053a17e 7439                     je         0x53a1b9
0053a180 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053a183 8b5108                   mov        edx, dword ptr [ecx + 8]
0053a186 52                       push       edx
0053a187 68f4685700               push       0x5768f4
0053a18c e81f25edff               call       0x40c6b0
0053a191 83c408                   add        esp, 8
0053a194 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053a197 83780800                 cmp        dword ptr [eax + 8], 0
0053a19b 741c                     je         0x53a1b9
0053a19d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053a1a0 8b5108                   mov        edx, dword ptr [ecx + 8]
0053a1a3 52                       push       edx
0053a1a4 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a1aa e8c154eeff               call       0x41f670
0053a1af 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053a1b2 c7400800000000           mov        dword ptr [eax + 8], 0
0053a1b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053a1bc c7410800000000           mov        dword ptr [ecx + 8], 0
0053a1c3 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053a1c6 8b02                     mov        eax, dword ptr [edx]
0053a1c8 50                       push       eax
0053a1c9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a1cf e8dcf8ffff               call       0x539ab0
0053a1d4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053a1d7 c70100000000             mov        dword ptr [ecx], 0
0053a1dd 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053a1e0 8b420c                   mov        eax, dword ptr [edx + 0xc]
0053a1e3 50                       push       eax
0053a1e4 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a1ea e831f9ffff               call       0x539b20
0053a1ef 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053a1f2 c7410c00000000           mov        dword ptr [ecx + 0xc], 0
0053a1f9 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053a1fc c7420400000000           mov        dword ptr [edx + 4], 0
0053a203 8be5                     mov        esp, ebp
0053a205 5d                       pop        ebp
0053a206 c3                       ret        
0053a207 cc                       int3       
0053a208 cc                       int3       
0053a209 cc                       int3       
0053a20a cc                       int3       
0053a20b cc                       int3       
0053a20c cc                       int3       
0053a20d cc                       int3       
0053a20e cc                       int3       
0053a20f cc                       int3       
