0051b6d0 55                       push       ebp
0051b6d1 8bec                     mov        ebp, esp
0051b6d3 833d20615c0000           cmp        dword ptr [0x5c6120], 0
0051b6da 741b                     je         0x51b6f7
0051b6dc a120615c00               mov        eax, dword ptr [0x5c6120]
0051b6e1 50                       push       eax
0051b6e2 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0051b6e8 e8d340f0ff               call       0x41f7c0
0051b6ed c70520615c0000000000     mov        dword ptr [0x5c6120], 0
0051b6f7 5d                       pop        ebp
0051b6f8 c3                       ret        
0051b6f9 cc                       int3       
0051b6fa cc                       int3       
0051b6fb cc                       int3       
0051b6fc cc                       int3       
0051b6fd cc                       int3       
0051b6fe cc                       int3       
0051b6ff cc                       int3       
