00477f00 55                       push       ebp
00477f01 8bec                     mov        ebp, esp
00477f03 83ec08                   sub        esp, 8
00477f06 894dfc                   mov        dword ptr [ebp - 4], ecx
00477f09 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477f0c 83781400                 cmp        dword ptr [eax + 0x14], 0
00477f10 7418                     je         0x477f2a
00477f12 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477f15 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00477f18 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477f1b 8b12                     mov        edx, dword ptr [edx]
00477f1d 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00477f20 8b4210                   mov        eax, dword ptr [edx + 0x10]
00477f23 ffd0                     call       eax
00477f25 8945f8                   mov        dword ptr [ebp - 8], eax
00477f28 eb07                     jmp        0x477f31
00477f2a c745f800000000           mov        dword ptr [ebp - 8], 0
00477f31 8b45f8                   mov        eax, dword ptr [ebp - 8]
00477f34 8be5                     mov        esp, ebp
00477f36 5d                       pop        ebp
00477f37 c3                       ret        
00477f38 cc                       int3       
00477f39 cc                       int3       
00477f3a cc                       int3       
00477f3b cc                       int3       
00477f3c cc                       int3       
00477f3d cc                       int3       
00477f3e cc                       int3       
00477f3f cc                       int3       
