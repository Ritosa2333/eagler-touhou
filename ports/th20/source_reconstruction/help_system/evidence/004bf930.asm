004bf930 55                       push       ebp
004bf931 8bec                     mov        ebp, esp
004bf933 51                       push       ecx
004bf934 680c0c5700               push       0x570c0c
004bf939 6a0e                     push       0xe
004bf93b 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004bf941 e8eaf1f8ff               call       0x44eb30
004bf946 8945fc                   mov        dword ptr [ebp - 4], eax
004bf949 a1244d5c00               mov        eax, dword ptr [0x5c4d24]
004bf94e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bf951 8988b0000000             mov        dword ptr [eax + 0xb0], ecx
004bf957 837dfc00                 cmp        dword ptr [ebp - 4], 0
004bf95b 7502                     jne        0x4bf95f
004bf95d eb26                     jmp        0x4bf985
004bf95f b9d05a5c00               mov        ecx, 0x5c5ad0
004bf964 e847afffff               call       0x4ba8b0
004bf969 8b15244d5c00             mov        edx, dword ptr [0x5c4d24]
004bf96f 8b4a08                   mov        ecx, dword ptr [edx + 8]
004bf972 e80934f5ff               call       0x412d80
004bf977 a1244d5c00               mov        eax, dword ptr [0x5c4d24]
004bf97c 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004bf97f e8fc33f5ff               call       0x412d80
004bf984 90                       nop        
004bf985 8be5                     mov        esp, ebp
004bf987 5d                       pop        ebp
004bf988 c3                       ret        
004bf989 cc                       int3       
004bf98a cc                       int3       
004bf98b cc                       int3       
004bf98c cc                       int3       
004bf98d cc                       int3       
004bf98e cc                       int3       
004bf98f cc                       int3       
