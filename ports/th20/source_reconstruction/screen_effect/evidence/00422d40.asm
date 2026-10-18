00422d40 55                       push       ebp
00422d41 8bec                     mov        ebp, esp
00422d43 83ec0c                   sub        esp, 0xc
00422d46 894df4                   mov        dword ptr [ebp - 0xc], ecx
00422d49 6a44                     push       0x44
00422d4b e80dfa1100               call       0x54275d
00422d50 83c404                   add        esp, 4
00422d53 8945fc                   mov        dword ptr [ebp - 4], eax
00422d56 837dfc00                 cmp        dword ptr [ebp - 4], 0
00422d5a 7417                     je         0x422d73
00422d5c 6a44                     push       0x44
00422d5e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00422d61 e81a93feff               call       0x40c080
00422d66 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00422d69 e842060000               call       0x4233b0
00422d6e 8945f8                   mov        dword ptr [ebp - 8], eax
00422d71 eb07                     jmp        0x422d7a
00422d73 c745f800000000           mov        dword ptr [ebp - 8], 0
00422d7a 8b45f8                   mov        eax, dword ptr [ebp - 8]
00422d7d 8be5                     mov        esp, ebp
00422d7f 5d                       pop        ebp
00422d80 c20400                   ret        4
00422d83 cc                       int3       
00422d84 cc                       int3       
00422d85 cc                       int3       
00422d86 cc                       int3       
00422d87 cc                       int3       
00422d88 cc                       int3       
00422d89 cc                       int3       
00422d8a cc                       int3       
00422d8b cc                       int3       
00422d8c cc                       int3       
00422d8d cc                       int3       
00422d8e cc                       int3       
00422d8f cc                       int3       
