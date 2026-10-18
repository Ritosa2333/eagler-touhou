0041db60 55                       push       ebp
0041db61 8bec                     mov        ebp, esp
0041db63 51                       push       ecx
0041db64 894dfc                   mov        dword ptr [ebp - 4], ecx
0041db67 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041db6a c6800d0e0006ff           mov        byte ptr [eax + 0x6000e0d], 0xff
0041db71 8be5                     mov        esp, ebp
0041db73 5d                       pop        ebp
0041db74 c3                       ret        
0041db75 cc                       int3       
0041db76 cc                       int3       
0041db77 cc                       int3       
0041db78 cc                       int3       
0041db79 cc                       int3       
0041db7a cc                       int3       
0041db7b cc                       int3       
0041db7c cc                       int3       
0041db7d cc                       int3       
0041db7e cc                       int3       
0041db7f cc                       int3       
