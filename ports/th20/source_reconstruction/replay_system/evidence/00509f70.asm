00509f70 55                       push       ebp
00509f71 8bec                     mov        ebp, esp
00509f73 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
00509f7a 7416                     je         0x509f92
00509f7c 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
00509f82 e859bbf7ff               call       0x485ae0
00509f87 85c0                     test       eax, eax
00509f89 7407                     je         0x509f92
00509f8b b801000000               mov        eax, 1
00509f90 eb08                     jmp        0x509f9a
00509f92 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00509f95 e846e2ffff               call       0x5081e0
00509f9a 5d                       pop        ebp
00509f9b c3                       ret        
00509f9c cc                       int3       
00509f9d cc                       int3       
00509f9e cc                       int3       
00509f9f cc                       int3       
