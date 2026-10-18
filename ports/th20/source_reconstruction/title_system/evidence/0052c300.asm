0052c300 55                       push       ebp
0052c301 8bec                     mov        ebp, esp
0052c303 51                       push       ecx
0052c304 894dfc                   mov        dword ptr [ebp - 4], ecx
0052c307 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052c30a 0560020000               add        eax, 0x260
0052c30f 8be5                     mov        esp, ebp
0052c311 5d                       pop        ebp
0052c312 c3                       ret        
0052c313 cc                       int3       
0052c314 cc                       int3       
0052c315 cc                       int3       
0052c316 cc                       int3       
0052c317 cc                       int3       
0052c318 cc                       int3       
0052c319 cc                       int3       
0052c31a cc                       int3       
0052c31b cc                       int3       
0052c31c cc                       int3       
0052c31d cc                       int3       
0052c31e cc                       int3       
0052c31f cc                       int3       
