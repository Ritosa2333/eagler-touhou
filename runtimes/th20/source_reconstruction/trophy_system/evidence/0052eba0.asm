0052eba0 55                       push       ebp
0052eba1 8bec                     mov        ebp, esp
0052eba3 51                       push       ecx
0052eba4 894dfc                   mov        dword ptr [ebp - 4], ecx
0052eba7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052ebaa e841d4fcff               call       0x4fbff0
0052ebaf 83c008                   add        eax, 8
0052ebb2 8be5                     mov        esp, ebp
0052ebb4 5d                       pop        ebp
0052ebb5 c3                       ret        
0052ebb6 cc                       int3       
0052ebb7 cc                       int3       
0052ebb8 cc                       int3       
0052ebb9 cc                       int3       
0052ebba cc                       int3       
0052ebbb cc                       int3       
0052ebbc cc                       int3       
0052ebbd cc                       int3       
0052ebbe cc                       int3       
0052ebbf cc                       int3       
