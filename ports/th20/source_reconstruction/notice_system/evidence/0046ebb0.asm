0046ebb0 55                       push       ebp
0046ebb1 8bec                     mov        ebp, esp
0046ebb3 51                       push       ecx
0046ebb4 894dfc                   mov        dword ptr [ebp - 4], ecx
0046ebb7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046ebba 83c004                   add        eax, 4
0046ebbd 50                       push       eax
0046ebbe e8edb8ffff               call       0x46a4b0
0046ebc3 83c404                   add        esp, 4
0046ebc6 8be5                     mov        esp, ebp
0046ebc8 5d                       pop        ebp
0046ebc9 c3                       ret        
0046ebca cc                       int3       
0046ebcb cc                       int3       
0046ebcc cc                       int3       
0046ebcd cc                       int3       
0046ebce cc                       int3       
0046ebcf cc                       int3       
