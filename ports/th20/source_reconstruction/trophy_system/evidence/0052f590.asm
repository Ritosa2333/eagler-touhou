0052f590 55                       push       ebp
0052f591 8bec                     mov        ebp, esp
0052f593 51                       push       ecx
0052f594 894dfc                   mov        dword ptr [ebp - 4], ecx
0052f597 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052f59a c700ffffffff             mov        dword ptr [eax], 0xffffffff
0052f5a0 8be5                     mov        esp, ebp
0052f5a2 5d                       pop        ebp
0052f5a3 c3                       ret        
0052f5a4 cc                       int3       
0052f5a5 cc                       int3       
0052f5a6 cc                       int3       
0052f5a7 cc                       int3       
0052f5a8 cc                       int3       
0052f5a9 cc                       int3       
0052f5aa cc                       int3       
0052f5ab cc                       int3       
0052f5ac cc                       int3       
0052f5ad cc                       int3       
0052f5ae cc                       int3       
0052f5af cc                       int3       
