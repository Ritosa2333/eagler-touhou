0044c670 55                       push       ebp
0044c671 8bec                     mov        ebp, esp
0044c673 83ec0c                   sub        esp, 0xc
0044c676 894df4                   mov        dword ptr [ebp - 0xc], ecx
0044c679 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044c67c e84f080000               call       0x44ced0
0044c681 8945fc                   mov        dword ptr [ebp - 4], eax
0044c684 837dfc00                 cmp        dword ptr [ebp - 4], 0
0044c688 7415                     je         0x44c69f
0044c68a 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0044c68d 50                       push       eax
0044c68e 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044c691 51                       push       ecx
0044c692 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044c695 e816000000               call       0x44c6b0
0044c69a 8945f8                   mov        dword ptr [ebp - 8], eax
0044c69d eb07                     jmp        0x44c6a6
0044c69f c745f800000000           mov        dword ptr [ebp - 8], 0
0044c6a6 8b45f8                   mov        eax, dword ptr [ebp - 8]
0044c6a9 8be5                     mov        esp, ebp
0044c6ab 5d                       pop        ebp
0044c6ac c20800                   ret        8
0044c6af cc                       int3       
