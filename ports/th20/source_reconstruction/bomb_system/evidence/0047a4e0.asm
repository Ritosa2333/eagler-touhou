0047a4e0 55                       push       ebp
0047a4e1 8bec                     mov        ebp, esp
0047a4e3 51                       push       ecx
0047a4e4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a4e7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a4ea 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047a4ed 8988a0000000             mov        dword ptr [eax + 0xa0], ecx
0047a4f3 8be5                     mov        esp, ebp
0047a4f5 5d                       pop        ebp
0047a4f6 c20400                   ret        4
0047a4f9 cc                       int3       
0047a4fa cc                       int3       
0047a4fb cc                       int3       
0047a4fc cc                       int3       
0047a4fd cc                       int3       
0047a4fe cc                       int3       
0047a4ff cc                       int3       
