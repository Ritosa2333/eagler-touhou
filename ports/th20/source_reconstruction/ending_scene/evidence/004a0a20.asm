004a0a20 55                       push       ebp
004a0a21 8bec                     mov        ebp, esp
004a0a23 83ec0c                   sub        esp, 0xc
004a0a26 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0a29 b801000000               mov        eax, 1
004a0a2e 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004a0a31 d3e0                     shl        eax, cl
004a0a33 50                       push       eax
004a0a34 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0a37 e8648bfaff               call       0x4495a0
004a0a3c 85c0                     test       eax, eax
004a0a3e 741f                     je         0x4a0a5f
004a0a40 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0a43 81c198010000             add        ecx, 0x198
004a0a49 894df4                   mov        dword ptr [ebp - 0xc], ecx
004a0a4c 8b5508                   mov        edx, dword ptr [ebp + 8]
004a0a4f 52                       push       edx
004a0a50 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004a0a53 e8283bf7ff               call       0x414580
004a0a58 8b00                     mov        eax, dword ptr [eax]
004a0a5a 8945f8                   mov        dword ptr [ebp - 8], eax
004a0a5d eb07                     jmp        0x4a0a66
004a0a5f c745f800000000           mov        dword ptr [ebp - 8], 0
004a0a66 8b45f8                   mov        eax, dword ptr [ebp - 8]
004a0a69 8be5                     mov        esp, ebp
004a0a6b 5d                       pop        ebp
004a0a6c c20400                   ret        4
004a0a6f cc                       int3       
