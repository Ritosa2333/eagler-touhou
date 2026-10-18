00531bd0 55                       push       ebp
00531bd1 8bec                     mov        ebp, esp
00531bd3 51                       push       ecx
00531bd4 894dfc                   mov        dword ptr [ebp - 4], ecx
00531bd7 6a00                     push       0
00531bd9 e85226f3ff               call       0x464230
00531bde 83c404                   add        esp, 4
00531be1 8bc8                     mov        ecx, eax
00531be3 e858dcfcff               call       0x4ff840
00531be8 8be5                     mov        esp, ebp
00531bea 5d                       pop        ebp
00531beb c3                       ret        
00531bec cc                       int3       
00531bed cc                       int3       
00531bee cc                       int3       
00531bef cc                       int3       
