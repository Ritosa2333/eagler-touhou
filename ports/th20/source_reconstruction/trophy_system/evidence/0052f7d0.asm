0052f7d0 55                       push       ebp
0052f7d1 8bec                     mov        ebp, esp
0052f7d3 51                       push       ecx
0052f7d4 894dfc                   mov        dword ptr [ebp - 4], ecx
0052f7d7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052f7da e801f5ffff               call       0x52ece0
0052f7df 8b4508                   mov        eax, dword ptr [ebp + 8]
0052f7e2 50                       push       eax
0052f7e3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052f7e6 e885daffff               call       0x52d270
0052f7eb 90                       nop        
0052f7ec 8be5                     mov        esp, ebp
0052f7ee 5d                       pop        ebp
0052f7ef c20400                   ret        4
0052f7f2 cc                       int3       
0052f7f3 cc                       int3       
0052f7f4 cc                       int3       
0052f7f5 cc                       int3       
0052f7f6 cc                       int3       
0052f7f7 cc                       int3       
0052f7f8 cc                       int3       
0052f7f9 cc                       int3       
0052f7fa cc                       int3       
0052f7fb cc                       int3       
0052f7fc cc                       int3       
0052f7fd cc                       int3       
0052f7fe cc                       int3       
0052f7ff cc                       int3       
