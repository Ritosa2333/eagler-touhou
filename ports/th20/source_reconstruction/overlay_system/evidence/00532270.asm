00532270 55                       push       ebp
00532271 8bec                     mov        ebp, esp
00532273 51                       push       ecx
00532274 894dfc                   mov        dword ptr [ebp - 4], ecx
00532277 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053227a e851d8ffff               call       0x52fad0
0053227f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00532282 c700305e5700             mov        dword ptr [eax], 0x575e30
00532288 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053228b 83c138                   add        ecx, 0x38
0053228e e82d3aefff               call       0x425cc0
00532293 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532296 83c13c                   add        ecx, 0x3c
00532299 e8f20aefff               call       0x422d90
0053229e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005322a1 83c14c                   add        ecx, 0x4c
005322a4 e8e70aefff               call       0x422d90
005322a9 90                       nop        
005322aa 8b45fc                   mov        eax, dword ptr [ebp - 4]
005322ad 8be5                     mov        esp, ebp
005322af 5d                       pop        ebp
005322b0 c3                       ret        
005322b1 cc                       int3       
005322b2 cc                       int3       
005322b3 cc                       int3       
005322b4 cc                       int3       
005322b5 cc                       int3       
005322b6 cc                       int3       
005322b7 cc                       int3       
005322b8 cc                       int3       
005322b9 cc                       int3       
005322ba cc                       int3       
005322bb cc                       int3       
005322bc cc                       int3       
005322bd cc                       int3       
005322be cc                       int3       
005322bf cc                       int3       
