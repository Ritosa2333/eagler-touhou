004d5bb0 55                       push       ebp
004d5bb1 8bec                     mov        ebp, esp
004d5bb3 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004d5bb9 e822fffaff               call       0x485ae0
004d5bbe 85c0                     test       eax, eax
004d5bc0 7407                     je         0x4d5bc9
004d5bc2 b801000000               mov        eax, 1
004d5bc7 eb08                     jmp        0x4d5bd1
004d5bc9 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d5bcc e8efa4ffff               call       0x4d00c0
004d5bd1 5d                       pop        ebp
004d5bd2 c3                       ret        
004d5bd3 cc                       int3       
004d5bd4 cc                       int3       
004d5bd5 cc                       int3       
004d5bd6 cc                       int3       
004d5bd7 cc                       int3       
004d5bd8 cc                       int3       
004d5bd9 cc                       int3       
004d5bda cc                       int3       
004d5bdb cc                       int3       
004d5bdc cc                       int3       
004d5bdd cc                       int3       
004d5bde cc                       int3       
004d5bdf cc                       int3       
