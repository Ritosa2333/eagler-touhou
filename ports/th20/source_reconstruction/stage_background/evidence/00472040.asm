00472040 55                       push       ebp
00472041 8bec                     mov        ebp, esp
00472043 51                       push       ecx
00472044 894dfc                   mov        dword ptr [ebp - 4], ecx
00472047 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047204a 8b4004                   mov        eax, dword ptr [eax + 4]
0047204d 99                       cdq        
0047204e f77d08                   idiv       dword ptr [ebp + 8]
00472051 8bc2                     mov        eax, edx
00472053 8be5                     mov        esp, ebp
00472055 5d                       pop        ebp
00472056 c20400                   ret        4
00472059 cc                       int3       
0047205a cc                       int3       
0047205b cc                       int3       
0047205c cc                       int3       
0047205d cc                       int3       
0047205e cc                       int3       
0047205f cc                       int3       
