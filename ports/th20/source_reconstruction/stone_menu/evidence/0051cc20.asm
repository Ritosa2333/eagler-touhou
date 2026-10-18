0051cc20 55                       push       ebp
0051cc21 8bec                     mov        ebp, esp
0051cc23 51                       push       ecx
0051cc24 68804e5700               push       0x574e80
0051cc29 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0051cc2f e81c86ffff               call       0x515250
0051cc34 8945fc                   mov        dword ptr [ebp - 4], eax
0051cc37 8b4508                   mov        eax, dword ptr [ebp + 8]
0051cc3a 50                       push       eax
0051cc3b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051cc3e e81dcdffff               call       0x519960
0051cc43 85c0                     test       eax, eax
0051cc45 7404                     je         0x51cc4b
0051cc47 eb10                     jmp        0x51cc59
0051cc49 eb0e                     jmp        0x51cc59
0051cc4b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051cc4e 890d20615c00             mov        dword ptr [0x5c6120], ecx
0051cc54 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051cc57 eb1e                     jmp        0x51cc77
0051cc59 837dfc00                 cmp        dword ptr [ebp - 4], 0
0051cc5d 7416                     je         0x51cc75
0051cc5f 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051cc62 52                       push       edx
0051cc63 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0051cc69 e8522bf0ff               call       0x41f7c0
0051cc6e c745fc00000000           mov        dword ptr [ebp - 4], 0
0051cc75 33c0                     xor        eax, eax
0051cc77 8be5                     mov        esp, ebp
0051cc79 5d                       pop        ebp
0051cc7a c3                       ret        
0051cc7b cc                       int3       
0051cc7c cc                       int3       
0051cc7d cc                       int3       
0051cc7e cc                       int3       
0051cc7f cc                       int3       
