0044d880 55                       push       ebp
0044d881 8bec                     mov        ebp, esp
0044d883 83ec20                   sub        esp, 0x20
0044d886 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044d88b 33c5                     xor        eax, ebp
0044d88d 8945fc                   mov        dword ptr [ebp - 4], eax
0044d890 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0044d893 6a11                     push       0x11
0044d895 b940025c00               mov        ecx, 0x5c0240
0044d89a e871eafbff               call       0x40c310
0044d89f 50                       push       eax
0044d8a0 8d4df8                   lea        ecx, [ebp - 8]
0044d8a3 e888defbff               call       0x40b730
0044d8a8 90                       nop        
0044d8a9 a188585c00               mov        eax, dword ptr [0x5c5888]
0044d8ae c1e805                   shr        eax, 5
0044d8b1 83e003                   and        eax, 3
0044d8b4 7414                     je         0x44d8ca
0044d8b6 c645f701                 mov        byte ptr [ebp - 9], 1
0044d8ba 8d4df8                   lea        ecx, [ebp - 8]
0044d8bd e83ee0fbff               call       0x40b900
0044d8c2 8a45f7                   mov        al, byte ptr [ebp - 9]
0044d8c5 e994000000               jmp        0x44d95e
0044d8ca 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0044d8cd 81c130070006             add        ecx, 0x6000730
0044d8d3 894de8                   mov        dword ptr [ebp - 0x18], ecx
0044d8d6 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0044d8d9 e8c2e4fbff               call       0x40bda0
0044d8de 8945f0                   mov        dword ptr [ebp - 0x10], eax
0044d8e1 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0044d8e4 e887e5fcff               call       0x41be70
0044d8e9 8945e0                   mov        dword ptr [ebp - 0x20], eax
0044d8ec eb09                     jmp        0x44d8f7
0044d8ee 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0044d8f1 83c204                   add        edx, 4
0044d8f4 8955f0                   mov        dword ptr [ebp - 0x10], edx
0044d8f7 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0044d8fa 3b45e0                   cmp        eax, dword ptr [ebp - 0x20]
0044d8fd 7450                     je         0x44d94f
0044d8ff 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0044d902 894dec                   mov        dword ptr [ebp - 0x14], ecx
0044d905 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0044d908 833a00                   cmp        dword ptr [edx], 0
0044d90b 7440                     je         0x44d94d
0044d90d 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0044d910 8b08                     mov        ecx, dword ptr [eax]
0044d912 83796000                 cmp        dword ptr [ecx + 0x60], 0
0044d916 7413                     je         0x44d92b
0044d918 c645f600                 mov        byte ptr [ebp - 0xa], 0
0044d91c 8d4df8                   lea        ecx, [ebp - 8]
0044d91f e8dcdffbff               call       0x40b900
0044d924 8a45f6                   mov        al, byte ptr [ebp - 0xa]
0044d927 eb35                     jmp        0x44d95e
0044d929 eb22                     jmp        0x44d94d
0044d92b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0044d92e 8b0a                     mov        ecx, dword ptr [edx]
0044d930 83c15c                   add        ecx, 0x5c
0044d933 e888b9fdff               call       0x4292c0
0044d938 85c0                     test       eax, eax
0044d93a 7411                     je         0x44d94d
0044d93c c645f500                 mov        byte ptr [ebp - 0xb], 0
0044d940 8d4df8                   lea        ecx, [ebp - 8]
0044d943 e8b8dffbff               call       0x40b900
0044d948 8a45f5                   mov        al, byte ptr [ebp - 0xb]
0044d94b eb11                     jmp        0x44d95e
0044d94d eb9f                     jmp        0x44d8ee
0044d94f c645f401                 mov        byte ptr [ebp - 0xc], 1
0044d953 8d4df8                   lea        ecx, [ebp - 8]
0044d956 e8a5dffbff               call       0x40b900
0044d95b 8a45f4                   mov        al, byte ptr [ebp - 0xc]
0044d95e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044d961 33cd                     xor        ecx, ebp
0044d963 e886500f00               call       0x5429ee
0044d968 8be5                     mov        esp, ebp
0044d96a 5d                       pop        ebp
0044d96b c3                       ret        
0044d96c cc                       int3       
0044d96d cc                       int3       
0044d96e cc                       int3       
0044d96f cc                       int3       
