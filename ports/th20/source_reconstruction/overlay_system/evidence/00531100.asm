00531100 55                       push       ebp
00531101 8bec                     mov        ebp, esp
00531103 51                       push       ecx
00531104 894dfc                   mov        dword ptr [ebp - 4], ecx
00531107 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053110a e8c1e9ffff               call       0x52fad0
0053110f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00531112 c700c45b5700             mov        dword ptr [eax], 0x575bc4
00531118 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053111b 83c138                   add        ecx, 0x38
0053111e e89d4befff               call       0x425cc0
00531123 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531126 83c13c                   add        ecx, 0x3c
00531129 e8621cefff               call       0x422d90
0053112e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531131 83c14c                   add        ecx, 0x4c
00531134 e8571cefff               call       0x422d90
00531139 90                       nop        
0053113a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053113d 8be5                     mov        esp, ebp
0053113f 5d                       pop        ebp
00531140 c3                       ret        
00531141 cc                       int3       
00531142 cc                       int3       
00531143 cc                       int3       
00531144 cc                       int3       
00531145 cc                       int3       
00531146 cc                       int3       
00531147 cc                       int3       
00531148 cc                       int3       
00531149 cc                       int3       
0053114a cc                       int3       
0053114b cc                       int3       
0053114c cc                       int3       
0053114d cc                       int3       
0053114e cc                       int3       
0053114f cc                       int3       
