0052f8e0 55                       push       ebp
0052f8e1 8bec                     mov        ebp, esp
0052f8e3 51                       push       ecx
0052f8e4 894dfc                   mov        dword ptr [ebp - 4], ecx
0052f8e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052f8ea e8d1f3ffff               call       0x52ecc0
0052f8ef 8b00                     mov        eax, dword ptr [eax]
0052f8f1 8be5                     mov        esp, ebp
0052f8f3 5d                       pop        ebp
0052f8f4 c3                       ret        
0052f8f5 cc                       int3       
0052f8f6 cc                       int3       
0052f8f7 cc                       int3       
0052f8f8 cc                       int3       
0052f8f9 cc                       int3       
0052f8fa cc                       int3       
0052f8fb cc                       int3       
0052f8fc cc                       int3       
0052f8fd cc                       int3       
0052f8fe cc                       int3       
0052f8ff cc                       int3       
