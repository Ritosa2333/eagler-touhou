005388c0 55                       push       ebp
005388c1 8bec                     mov        ebp, esp
005388c3 83ec48                   sub        esp, 0x48
005388c6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005388cb 33c5                     xor        eax, ebp
005388cd 8945fc                   mov        dword ptr [ebp - 4], eax
005388d0 894dc0                   mov        dword ptr [ebp - 0x40], ecx
005388d3 e888b9ffff               call       0x534260
005388d8 50                       push       eax
005388d9 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
005388dc 83c114                   add        ecx, 0x14
005388df e83caceeff               call       0x423520
005388e4 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
005388e7 c6403401                 mov        byte ptr [eax + 0x34], 1
005388eb 8d4dc4                   lea        ecx, [ebp - 0x3c]
005388ee e83d31f4ff               call       0x47ba30
005388f3 6a00                     push       0
005388f5 e8367ff2ff               call       0x460830
005388fa 83c404                   add        esp, 4
005388fd 8bc8                     mov        ecx, eax
005388ff e84c7ff2ff               call       0x460850
00538904 8b08                     mov        ecx, dword ptr [eax]
00538906 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
00538909 8b5004                   mov        edx, dword ptr [eax + 4]
0053890c 8955c8                   mov        dword ptr [ebp - 0x38], edx
0053890f 8b4008                   mov        eax, dword ptr [eax + 8]
00538912 8945cc                   mov        dword ptr [ebp - 0x34], eax
00538915 b9dc695c00               mov        ecx, 0x5c69dc
0053891a e8b173f1ff               call       0x44fcd0
0053891f 90                       nop        
00538920 6a00                     push       0
00538922 e8f9ebefff               call       0x437520
00538927 83c404                   add        esp, 4
0053892a 8945bc                   mov        dword ptr [ebp - 0x44], eax
0053892d 6a00                     push       0
0053892f 8d4dc4                   lea        ecx, [ebp - 0x3c]
00538932 51                       push       ecx
00538933 6a07                     push       7
00538935 8d55b8                   lea        edx, [ebp - 0x48]
00538938 52                       push       edx
00538939 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0053893c e82f52f6ff               call       0x49db70
00538941 8b00                     mov        eax, dword ptr [eax]
00538943 a3dc695c00               mov        dword ptr [0x5c69dc], eax
00538948 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0053894b 51                       push       ecx
0053894c b9dc695c00               mov        ecx, 0x5c69dc
00538951 e8fa7ef1ff               call       0x450850
00538956 68304e5300               push       0x534e30
0053895b b9dc695c00               mov        ecx, 0x5c69dc
00538960 e89b67f1ff               call       0x44f100
00538965 90                       nop        
00538966 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00538969 33cd                     xor        ecx, ebp
0053896b e87ea00000               call       0x5429ee
00538970 8be5                     mov        esp, ebp
00538972 5d                       pop        ebp
00538973 c3                       ret        
00538974 cc                       int3       
00538975 cc                       int3       
00538976 cc                       int3       
00538977 cc                       int3       
00538978 cc                       int3       
00538979 cc                       int3       
0053897a cc                       int3       
0053897b cc                       int3       
0053897c cc                       int3       
0053897d cc                       int3       
0053897e cc                       int3       
0053897f cc                       int3       
