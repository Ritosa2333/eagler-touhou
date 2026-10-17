0047a2c0 55                       push       ebp
0047a2c1 8bec                     mov        ebp, esp
0047a2c3 51                       push       ecx
0047a2c4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a2c7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a2ca 81c198010000             add        ecx, 0x198
0047a2d0 e8cb1af9ff               call       0x40bda0
0047a2d5 8be5                     mov        esp, ebp
0047a2d7 5d                       pop        ebp
0047a2d8 c3                       ret        
0047a2d9 cc                       int3       
0047a2da cc                       int3       
0047a2db cc                       int3       
0047a2dc cc                       int3       
0047a2dd cc                       int3       
0047a2de cc                       int3       
0047a2df cc                       int3       
