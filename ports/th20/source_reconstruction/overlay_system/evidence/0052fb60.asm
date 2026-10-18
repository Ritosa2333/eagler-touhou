0052fb60 55                       push       ebp
0052fb61 8bec                     mov        ebp, esp
0052fb63 51                       push       ecx
0052fb64 894dfc                   mov        dword ptr [ebp - 4], ecx
0052fb67 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052fb6a c6403500                 mov        byte ptr [eax + 0x35], 0
0052fb6e 8be5                     mov        esp, ebp
0052fb70 5d                       pop        ebp
0052fb71 c3                       ret        
0052fb72 cc                       int3       
0052fb73 cc                       int3       
0052fb74 cc                       int3       
0052fb75 cc                       int3       
0052fb76 cc                       int3       
0052fb77 cc                       int3       
0052fb78 cc                       int3       
0052fb79 cc                       int3       
0052fb7a cc                       int3       
0052fb7b cc                       int3       
0052fb7c cc                       int3       
0052fb7d cc                       int3       
0052fb7e cc                       int3       
0052fb7f cc                       int3       
