004642e0 55                       push       ebp
004642e1 8bec                     mov        ebp, esp
004642e3 83ec08                   sub        esp, 8
004642e6 894dfc                   mov        dword ptr [ebp - 4], ecx
004642e9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004642ec 83782c00                 cmp        dword ptr [eax + 0x2c], 0
004642f0 7423                     je         0x464315
004642f2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004642f5 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
004642f8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004642fb 8b12                     mov        edx, dword ptr [edx]
004642fd 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
00464300 8b4270                   mov        eax, dword ptr [edx + 0x70]
00464303 ffd0                     call       eax
00464305 0fb6c8                   movzx      ecx, al
00464308 85c9                     test       ecx, ecx
0046430a 7409                     je         0x464315
0046430c c745f801000000           mov        dword ptr [ebp - 8], 1
00464313 eb07                     jmp        0x46431c
00464315 c745f800000000           mov        dword ptr [ebp - 8], 0
0046431c 0fb645f8                 movzx      eax, byte ptr [ebp - 8]
00464320 8be5                     mov        esp, ebp
00464322 5d                       pop        ebp
00464323 c3                       ret        
00464324 cc                       int3       
00464325 cc                       int3       
00464326 cc                       int3       
00464327 cc                       int3       
00464328 cc                       int3       
00464329 cc                       int3       
0046432a cc                       int3       
0046432b cc                       int3       
0046432c cc                       int3       
0046432d cc                       int3       
0046432e cc                       int3       
0046432f cc                       int3       
