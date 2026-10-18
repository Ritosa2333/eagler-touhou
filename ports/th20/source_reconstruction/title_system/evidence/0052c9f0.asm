0052c9f0 55                       push       ebp
0052c9f1 8bec                     mov        ebp, esp
0052c9f3 83ec08                   sub        esp, 8
0052c9f6 894dfc                   mov        dword ptr [ebp - 4], ecx
0052c9f9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052c9fc e8af75f3ff               call       0x463fb0
0052ca01 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052ca04 83b88042120000           cmp        dword ptr [eax + 0x124280], 0
0052ca0b 7e09                     jle        0x52ca16
0052ca0d c745f801000000           mov        dword ptr [ebp - 8], 1
0052ca14 eb07                     jmp        0x52ca1d
0052ca16 c745f800000000           mov        dword ptr [ebp - 8], 0
0052ca1d 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052ca20 8be5                     mov        esp, ebp
0052ca22 5d                       pop        ebp
0052ca23 c3                       ret        
0052ca24 cc                       int3       
0052ca25 cc                       int3       
0052ca26 cc                       int3       
0052ca27 cc                       int3       
0052ca28 cc                       int3       
0052ca29 cc                       int3       
0052ca2a cc                       int3       
0052ca2b cc                       int3       
0052ca2c cc                       int3       
0052ca2d cc                       int3       
0052ca2e cc                       int3       
0052ca2f cc                       int3       
