004c0ce0 55                       push       ebp
004c0ce1 8bec                     mov        ebp, esp
004c0ce3 51                       push       ecx
004c0ce4 894dfc                   mov        dword ptr [ebp - 4], ecx
004c0ce7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0cea 81c144060000             add        ecx, 0x644
004c0cf0 e83bc3f9ff               call       0x45d030
004c0cf5 8be5                     mov        esp, ebp
004c0cf7 5d                       pop        ebp
004c0cf8 c3                       ret        
004c0cf9 cc                       int3       
004c0cfa cc                       int3       
004c0cfb cc                       int3       
004c0cfc cc                       int3       
004c0cfd cc                       int3       
004c0cfe cc                       int3       
004c0cff cc                       int3       
