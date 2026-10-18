0041dba0 55                       push       ebp
0041dba1 8bec                     mov        ebp, esp
0041dba3 51                       push       ecx
0041dba4 894dfc                   mov        dword ptr [ebp - 4], ecx
0041dba7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041dbaa c780080e0006ffffffff     mov        dword ptr [eax + 0x6000e08], 0xffffffff
0041dbb4 8be5                     mov        esp, ebp
0041dbb6 5d                       pop        ebp
0041dbb7 c3                       ret        
0041dbb8 cc                       int3       
0041dbb9 cc                       int3       
0041dbba cc                       int3       
0041dbbb cc                       int3       
0041dbbc cc                       int3       
0041dbbd cc                       int3       
0041dbbe cc                       int3       
0041dbbf cc                       int3       
