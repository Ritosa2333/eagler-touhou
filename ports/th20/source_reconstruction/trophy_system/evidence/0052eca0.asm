0052eca0 55                       push       ebp
0052eca1 8bec                     mov        ebp, esp
0052eca3 51                       push       ecx
0052eca4 894dfc                   mov        dword ptr [ebp - 4], ecx
0052eca7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052ecaa e841d3fcff               call       0x4fbff0
0052ecaf 83c00c                   add        eax, 0xc
0052ecb2 8be5                     mov        esp, ebp
0052ecb4 5d                       pop        ebp
0052ecb5 c3                       ret        
0052ecb6 cc                       int3       
0052ecb7 cc                       int3       
0052ecb8 cc                       int3       
0052ecb9 cc                       int3       
0052ecba cc                       int3       
0052ecbb cc                       int3       
0052ecbc cc                       int3       
0052ecbd cc                       int3       
0052ecbe cc                       int3       
0052ecbf cc                       int3       
