00532a90 55                       push       ebp
00532a91 8bec                     mov        ebp, esp
00532a93 51                       push       ecx
00532a94 894dfc                   mov        dword ptr [ebp - 4], ecx
00532a97 6a00                     push       0
00532a99 e89217f3ff               call       0x464230
00532a9e 83c404                   add        esp, 4
00532aa1 8bc8                     mov        ecx, eax
00532aa3 e8c80e0000               call       0x533970
00532aa8 33c0                     xor        eax, eax
00532aaa 8be5                     mov        esp, ebp
00532aac 5d                       pop        ebp
00532aad c20400                   ret        4
