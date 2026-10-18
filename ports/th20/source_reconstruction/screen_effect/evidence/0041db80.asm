0041db80 55                       push       ebp
0041db81 8bec                     mov        ebp, esp
0041db83 51                       push       ecx
0041db84 894dfc                   mov        dword ptr [ebp - 4], ecx
0041db87 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041db8a c6800e0e0006ff           mov        byte ptr [eax + 0x6000e0e], 0xff
0041db91 8be5                     mov        esp, ebp
0041db93 5d                       pop        ebp
0041db94 c3                       ret        
0041db95 cc                       int3       
0041db96 cc                       int3       
0041db97 cc                       int3       
0041db98 cc                       int3       
0041db99 cc                       int3       
0041db9a cc                       int3       
0041db9b cc                       int3       
0041db9c cc                       int3       
0041db9d cc                       int3       
0041db9e cc                       int3       
0041db9f cc                       int3       
