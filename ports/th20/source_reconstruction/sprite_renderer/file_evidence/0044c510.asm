0044c510 55                       push       ebp
0044c511 8bec                     mov        ebp, esp
0044c513 51                       push       ecx
0044c514 894dfc                   mov        dword ptr [ebp - 4], ecx
0044c517 8b4508                   mov        eax, dword ptr [ebp + 8]
0044c51a 833800                   cmp        dword ptr [eax], 0
0044c51d 741b                     je         0x44c53a
0044c51f 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044c522 8b11                     mov        edx, dword ptr [ecx]
0044c524 8b4508                   mov        eax, dword ptr [ebp + 8]
0044c527 8b08                     mov        ecx, dword ptr [eax]
0044c529 8b12                     mov        edx, dword ptr [edx]
0044c52b 51                       push       ecx
0044c52c 8b4208                   mov        eax, dword ptr [edx + 8]
0044c52f ffd0                     call       eax
0044c531 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044c534 c70100000000             mov        dword ptr [ecx], 0
0044c53a 8b5508                   mov        edx, dword ptr [ebp + 8]
0044c53d 837a0400                 cmp        dword ptr [edx + 4], 0
0044c541 7425                     je         0x44c568
0044c543 8b4508                   mov        eax, dword ptr [ebp + 8]
0044c546 83780400                 cmp        dword ptr [eax + 4], 0
0044c54a 741c                     je         0x44c568
0044c54c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044c54f 8b5104                   mov        edx, dword ptr [ecx + 4]
0044c552 52                       push       edx
0044c553 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044c559 e81231fdff               call       0x41f670
0044c55e 8b4508                   mov        eax, dword ptr [ebp + 8]
0044c561 c7400400000000           mov        dword ptr [eax + 4], 0
0044c568 8be5                     mov        esp, ebp
0044c56a 5d                       pop        ebp
0044c56b c20400                   ret        4
0044c56e cc                       int3       
0044c56f cc                       int3       
