004dfd20 55                       push       ebp
004dfd21 8bec                     mov        ebp, esp
004dfd23 6aff                     push       -1
004dfd25 6800765600               push       0x567600
004dfd2a 64a100000000             mov        eax, dword ptr fs:[0]
004dfd30 50                       push       eax
004dfd31 51                       push       ecx
004dfd32 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004dfd37 33c5                     xor        eax, ebp
004dfd39 50                       push       eax
004dfd3a 8d45f4                   lea        eax, [ebp - 0xc]
004dfd3d 64a300000000             mov        dword ptr fs:[0], eax
004dfd43 894df0                   mov        dword ptr [ebp - 0x10], ecx
004dfd46 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004dfd49 c70090205700             mov        dword ptr [eax], 0x572090
004dfd4f 68b4205700               push       0x5720b4
004dfd54 e857c9f2ff               call       0x40c6b0
004dfd59 83c404                   add        esp, 4
004dfd5c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfd5f 8b5108                   mov        edx, dword ptr [ecx + 8]
004dfd62 52                       push       edx
004dfd63 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004dfd69 e84227f3ff               call       0x4124b0
004dfd6e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004dfd71 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004dfd74 51                       push       ecx
004dfd75 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004dfd7b e83027f3ff               call       0x4124b0
004dfd80 c705b8605c0000000000     mov        dword ptr [0x5c60b8], 0
004dfd8a b9404d5c00               mov        ecx, 0x5c4d40
004dfd8f e87cdbffff               call       0x4dd910
004dfd94 90                       nop        
004dfd95 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfd98 83c114                   add        ecx, 0x14
004dfd9b e8d0fcfcff               call       0x4afa70
004dfda0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfda3 e84800f4ff               call       0x41fdf0
004dfda8 90                       nop        
004dfda9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004dfdac 64890d00000000           mov        dword ptr fs:[0], ecx
004dfdb3 59                       pop        ecx
004dfdb4 8be5                     mov        esp, ebp
004dfdb6 5d                       pop        ebp
004dfdb7 c3                       ret        
004dfdb8 cc                       int3       
004dfdb9 cc                       int3       
004dfdba cc                       int3       
004dfdbb cc                       int3       
004dfdbc cc                       int3       
004dfdbd cc                       int3       
004dfdbe cc                       int3       
004dfdbf cc                       int3       
