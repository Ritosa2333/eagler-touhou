00466a90 55                       push       ebp
00466a91 8bec                     mov        ebp, esp
00466a93 83ec0c                   sub        esp, 0xc
00466a96 894dfc                   mov        dword ptr [ebp - 4], ecx
00466a99 8b45fc                   mov        eax, dword ptr [ebp - 4]
00466a9c 05082d0000               add        eax, 0x2d08
00466aa1 8945f8                   mov        dword ptr [ebp - 8], eax
00466aa4 6a3c                     push       0x3c
00466aa6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00466aa9 e812cbfbff               call       0x4235c0
00466aae 0fb6c8                   movzx      ecx, al
00466ab1 85c9                     test       ecx, ecx
00466ab3 7407                     je         0x466abc
00466ab5 b801000000               mov        eax, 1
00466aba eb18                     jmp        0x466ad4
00466abc 8b55fc                   mov        edx, dword ptr [ebp - 4]
00466abf 81c2082d0000             add        edx, 0x2d08
00466ac5 8955f4                   mov        dword ptr [ebp - 0xc], edx
00466ac8 6a00                     push       0
00466aca 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00466acd e86ecafbff               call       0x423540
00466ad2 33c0                     xor        eax, eax
00466ad4 8be5                     mov        esp, ebp
00466ad6 5d                       pop        ebp
00466ad7 c3                       ret        
00466ad8 cc                       int3       
00466ad9 cc                       int3       
00466ada cc                       int3       
00466adb cc                       int3       
00466adc cc                       int3       
00466add cc                       int3       
00466ade cc                       int3       
00466adf cc                       int3       
