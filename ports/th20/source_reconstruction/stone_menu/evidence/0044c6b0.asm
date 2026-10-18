0044c6b0 55                       push       ebp
0044c6b1 8bec                     mov        ebp, esp
0044c6b3 83ec18                   sub        esp, 0x18
0044c6b6 894df4                   mov        dword ptr [ebp - 0xc], ecx
0044c6b9 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0044c6bc 0514050000               add        eax, 0x514
0044c6c1 8945ec                   mov        dword ptr [ebp - 0x14], eax
0044c6c4 6a01                     push       1
0044c6c6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0044c6c9 e8e2cdffff               call       0x4494b0
0044c6ce 8945f8                   mov        dword ptr [ebp - 8], eax
0044c6d1 837df800                 cmp        dword ptr [ebp - 8], 0
0044c6d5 0f84be000000             je         0x44c799
0044c6db 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0044c6de e81dfcfbff               call       0x40c300
0044c6e3 8945fc                   mov        dword ptr [ebp - 4], eax
0044c6e6 837dfc00                 cmp        dword ptr [ebp - 4], 0
0044c6ea 0f8499000000             je         0x44c789
0044c6f0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044c6f3 3b4df4                   cmp        ecx, dword ptr [ebp - 0xc]
0044c6f6 0f848d000000             je         0x44c789
0044c6fc 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044c6ff 0fbf8240040000           movsx      eax, word ptr [edx + 0x440]
0044c706 3b4508                   cmp        eax, dword ptr [ebp + 8]
0044c709 7406                     je         0x44c711
0044c70b 837d08ff                 cmp        dword ptr [ebp + 8], -1
0044c70f 7514                     jne        0x44c725
0044c711 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
0044c715 7505                     jne        0x44c71c
0044c717 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044c71a eb7f                     jmp        0x44c79b
0044c71c 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0044c71f 83e901                   sub        ecx, 1
0044c722 894d0c                   mov        dword ptr [ebp + 0xc], ecx
0044c725 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044c728 81c214050000             add        edx, 0x514
0044c72e 8955e8                   mov        dword ptr [ebp - 0x18], edx
0044c731 6a01                     push       1
0044c733 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0044c736 e875cdffff               call       0x4494b0
0044c73b 8bc8                     mov        ecx, eax
0044c73d e84e38fcff               call       0x40ff90
0044c742 85c0                     test       eax, eax
0044c744 741e                     je         0x44c764
0044c746 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0044c749 50                       push       eax
0044c74a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044c74d 51                       push       ecx
0044c74e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044c751 e85affffff               call       0x44c6b0
0044c756 8945f0                   mov        dword ptr [ebp - 0x10], eax
0044c759 837df000                 cmp        dword ptr [ebp - 0x10], 0
0044c75d 7405                     je         0x44c764
0044c75f 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0044c762 eb37                     jmp        0x44c79b
0044c764 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044c767 0fbf8240040000           movsx      eax, word ptr [edx + 0x440]
0044c76e 83f8fe                   cmp        eax, -2
0044c771 7516                     jne        0x44c789
0044c773 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0044c776 e81538fcff               call       0x40ff90
0044c77b 85c0                     test       eax, eax
0044c77d 750a                     jne        0x44c789
0044c77f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0044c782 e879fbfbff               call       0x40c300
0044c787 eb12                     jmp        0x44c79b
0044c789 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0044c78c e8ff37fcff               call       0x40ff90
0044c791 8945f8                   mov        dword ptr [ebp - 8], eax
0044c794 e938ffffff               jmp        0x44c6d1
0044c799 33c0                     xor        eax, eax
0044c79b 8be5                     mov        esp, ebp
0044c79d 5d                       pop        ebp
0044c79e c20800                   ret        8
0044c7a1 cc                       int3       
0044c7a2 cc                       int3       
0044c7a3 cc                       int3       
0044c7a4 cc                       int3       
0044c7a5 cc                       int3       
0044c7a6 cc                       int3       
0044c7a7 cc                       int3       
0044c7a8 cc                       int3       
0044c7a9 cc                       int3       
0044c7aa cc                       int3       
0044c7ab cc                       int3       
0044c7ac cc                       int3       
0044c7ad cc                       int3       
0044c7ae cc                       int3       
0044c7af cc                       int3       
