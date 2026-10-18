0041db40 55                       push       ebp
0041db41 8bec                     mov        ebp, esp
0041db43 51                       push       ecx
0041db44 894dfc                   mov        dword ptr [ebp - 4], ecx
0041db47 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041db4a c6800c0e00060b           mov        byte ptr [eax + 0x6000e0c], 0xb
0041db51 8be5                     mov        esp, ebp
0041db53 5d                       pop        ebp
0041db54 c3                       ret        
0041db55 cc                       int3       
0041db56 cc                       int3       
0041db57 cc                       int3       
0041db58 cc                       int3       
0041db59 cc                       int3       
0041db5a cc                       int3       
0041db5b cc                       int3       
0041db5c cc                       int3       
0041db5d cc                       int3       
0041db5e cc                       int3       
0041db5f cc                       int3       
