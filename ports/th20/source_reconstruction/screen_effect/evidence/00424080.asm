00424080 55                       push       ebp
00424081 8bec                     mov        ebp, esp
00424083 51                       push       ecx
00424084 894dfc                   mov        dword ptr [ebp - 4], ecx
00424087 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042408a c780180e000600000000     mov        dword ptr [eax + 0x6000e18], 0
00424094 8be5                     mov        esp, ebp
00424096 5d                       pop        ebp
00424097 c3                       ret        
00424098 cc                       int3       
00424099 cc                       int3       
0042409a cc                       int3       
0042409b cc                       int3       
0042409c cc                       int3       
0042409d cc                       int3       
0042409e cc                       int3       
0042409f cc                       int3       
