0041caf0 55                       push       ebp
0041caf1 8bec                     mov        ebp, esp
0041caf3 51                       push       ecx
0041caf4 894dfc                   mov        dword ptr [ebp - 4], ecx
0041caf7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041cafa 8b4024                   mov        eax, dword ptr [eax + 0x24]
0041cafd 8be5                     mov        esp, ebp
0041caff 5d                       pop        ebp
0041cb00 c3                       ret        
0041cb01 cc                       int3       
0041cb02 cc                       int3       
0041cb03 cc                       int3       
0041cb04 cc                       int3       
0041cb05 cc                       int3       
0041cb06 cc                       int3       
0041cb07 cc                       int3       
0041cb08 cc                       int3       
0041cb09 cc                       int3       
0041cb0a cc                       int3       
0041cb0b cc                       int3       
0041cb0c cc                       int3       
0041cb0d cc                       int3       
0041cb0e cc                       int3       
0041cb0f cc                       int3       
