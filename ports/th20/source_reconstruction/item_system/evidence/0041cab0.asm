0041cab0 55                       push       ebp
0041cab1 8bec                     mov        ebp, esp
0041cab3 51                       push       ecx
0041cab4 894dfc                   mov        dword ptr [ebp - 4], ecx
0041cab7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041caba 8b400c                   mov        eax, dword ptr [eax + 0xc]
0041cabd 8be5                     mov        esp, ebp
0041cabf 5d                       pop        ebp
0041cac0 c3                       ret        
0041cac1 cc                       int3       
0041cac2 cc                       int3       
0041cac3 cc                       int3       
0041cac4 cc                       int3       
0041cac5 cc                       int3       
0041cac6 cc                       int3       
0041cac7 cc                       int3       
0041cac8 cc                       int3       
0041cac9 cc                       int3       
0041caca cc                       int3       
0041cacb cc                       int3       
0041cacc cc                       int3       
0041cacd cc                       int3       
0041cace cc                       int3       
0041cacf cc                       int3       
