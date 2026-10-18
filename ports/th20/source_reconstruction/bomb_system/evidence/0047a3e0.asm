0047a3e0 55                       push       ebp
0047a3e1 8bec                     mov        ebp, esp
0047a3e3 51                       push       ecx
0047a3e4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a3e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a3ea 81c188000000             add        ecx, 0x88
0047a3f0 e87bffffff               call       0x47a370
0047a3f5 8be5                     mov        esp, ebp
0047a3f7 5d                       pop        ebp
0047a3f8 c3                       ret        
0047a3f9 cc                       int3       
0047a3fa cc                       int3       
0047a3fb cc                       int3       
0047a3fc cc                       int3       
0047a3fd cc                       int3       
0047a3fe cc                       int3       
0047a3ff cc                       int3       
