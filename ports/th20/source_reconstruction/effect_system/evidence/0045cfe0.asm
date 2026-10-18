0045cfe0 55                       push       ebp
0045cfe1 8bec                     mov        ebp, esp
0045cfe3 51                       push       ecx
0045cfe4 894dfc                   mov        dword ptr [ebp - 4], ecx
0045cfe7 6a00                     push       0
0045cfe9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045cfec 81c130190000             add        ecx, 0x1930
0045cff2 e82965fcff               call       0x423520
0045cff7 33c0                     xor        eax, eax
0045cff9 8be5                     mov        esp, ebp
0045cffb 5d                       pop        ebp
0045cffc c20800                   ret        8
0045cfff cc                       int3       
