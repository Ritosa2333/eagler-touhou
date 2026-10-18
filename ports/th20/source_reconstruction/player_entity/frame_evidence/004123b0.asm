004123b0 55                       push       ebp
004123b1 8bec                     mov        ebp, esp
004123b3 51                       push       ecx
004123b4 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004123b7 50                       push       eax
004123b8 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004123be e8ed030000               call       0x4127b0
004123c3 8945fc                   mov        dword ptr [ebp - 4], eax
004123c6 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004123c9 51                       push       ecx
004123ca 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004123cd e83e090000               call       0x412d10
004123d2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004123d5 e816040000               call       0x4127f0
004123da 8b5508                   mov        edx, dword ptr [ebp + 8]
004123dd 52                       push       edx
004123de 8b45fc                   mov        eax, dword ptr [ebp - 4]
004123e1 50                       push       eax
004123e2 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004123e8 e813fdffff               call       0x412100
004123ed 8b45fc                   mov        eax, dword ptr [ebp - 4]
004123f0 8be5                     mov        esp, ebp
004123f2 5d                       pop        ebp
004123f3 c3                       ret        
004123f4 cc                       int3       
004123f5 cc                       int3       
004123f6 cc                       int3       
004123f7 cc                       int3       
004123f8 cc                       int3       
004123f9 cc                       int3       
004123fa cc                       int3       
004123fb cc                       int3       
004123fc cc                       int3       
004123fd cc                       int3       
004123fe cc                       int3       
004123ff cc                       int3       
