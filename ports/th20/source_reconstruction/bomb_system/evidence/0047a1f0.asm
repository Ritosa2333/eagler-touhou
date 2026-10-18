0047a1f0 55                       push       ebp
0047a1f1 8bec                     mov        ebp, esp
0047a1f3 51                       push       ecx
0047a1f4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a1f7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a1fa e8419cfdff               call       0x453e40
0047a1ff 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a202 e8b998fdff               call       0x453ac0
0047a207 90                       nop        
0047a208 8be5                     mov        esp, ebp
0047a20a 5d                       pop        ebp
0047a20b c3                       ret        
0047a20c cc                       int3       
0047a20d cc                       int3       
0047a20e cc                       int3       
0047a20f cc                       int3       
