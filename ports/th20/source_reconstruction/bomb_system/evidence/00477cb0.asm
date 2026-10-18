00477cb0 55                       push       ebp
00477cb1 8bec                     mov        ebp, esp
00477cb3 51                       push       ecx
00477cb4 894dfc                   mov        dword ptr [ebp - 4], ecx
00477cb7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477cba 83781400                 cmp        dword ptr [eax + 0x14], 0
00477cbe 7414                     je         0x477cd4
00477cc0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477cc3 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00477cc6 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477cc9 8b12                     mov        edx, dword ptr [edx]
00477ccb 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00477cce 8b420c                   mov        eax, dword ptr [edx + 0xc]
00477cd1 ffd0                     call       eax
00477cd3 90                       nop        
00477cd4 b801000000               mov        eax, 1
00477cd9 8be5                     mov        esp, ebp
00477cdb 5d                       pop        ebp
00477cdc c3                       ret        
00477cdd cc                       int3       
00477cde cc                       int3       
00477cdf cc                       int3       
