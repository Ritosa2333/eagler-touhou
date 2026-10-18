00429970 55                       push       ebp
00429971 8bec                     mov        ebp, esp
00429973 51                       push       ecx
00429974 894dfc                   mov        dword ptr [ebp - 4], ecx
00429977 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042997a c7402400000000           mov        dword ptr [eax + 0x24], 0
00429981 8be5                     mov        esp, ebp
00429983 5d                       pop        ebp
00429984 c3                       ret        
00429985 cc                       int3       
00429986 cc                       int3       
00429987 cc                       int3       
00429988 cc                       int3       
00429989 cc                       int3       
0042998a cc                       int3       
0042998b cc                       int3       
0042998c cc                       int3       
0042998d cc                       int3       
0042998e cc                       int3       
0042998f cc                       int3       
