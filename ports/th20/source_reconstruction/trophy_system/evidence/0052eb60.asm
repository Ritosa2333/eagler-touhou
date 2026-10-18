0052eb60 55                       push       ebp
0052eb61 8bec                     mov        ebp, esp
0052eb63 51                       push       ecx
0052eb64 894dfc                   mov        dword ptr [ebp - 4], ecx
0052eb67 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052eb6a e881d4fcff               call       0x4fbff0
0052eb6f 83c004                   add        eax, 4
0052eb72 8be5                     mov        esp, ebp
0052eb74 5d                       pop        ebp
0052eb75 c3                       ret        
0052eb76 cc                       int3       
0052eb77 cc                       int3       
0052eb78 cc                       int3       
0052eb79 cc                       int3       
0052eb7a cc                       int3       
0052eb7b cc                       int3       
0052eb7c cc                       int3       
0052eb7d cc                       int3       
0052eb7e cc                       int3       
0052eb7f cc                       int3       
