004afaa0 55                       push       ebp
004afaa1 8bec                     mov        ebp, esp
004afaa3 6aff                     push       -1
004afaa5 6860805600               push       0x568060
004afaaa 64a100000000             mov        eax, dword ptr fs:[0]
004afab0 50                       push       eax
004afab1 83ec14                   sub        esp, 0x14
004afab4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004afab9 33c5                     xor        eax, ebp
004afabb 50                       push       eax
004afabc 8d45f4                   lea        eax, [ebp - 0xc]
004afabf 64a300000000             mov        dword ptr fs:[0], eax
004afac5 894df0                   mov        dword ptr [ebp - 0x10], ecx
004afac8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004afacb c7002c065700             mov        dword ptr [eax], 0x57062c
004afad1 6880065700               push       0x570680
004afad6 e8d5cbf5ff               call       0x40c6b0
004afadb 83c404                   add        esp, 4
004afade 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004afae1 e83a670000               call       0x4b6220
004afae6 e875be0600               call       0x51b960
004afaeb 8bc8                     mov        ecx, eax
004afaed e8de9d0600               call       0x5198d0
004afaf2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004afaf5 8b5108                   mov        edx, dword ptr [ecx + 8]
004afaf8 52                       push       edx
004afaf9 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004afaff e8ac29f6ff               call       0x4124b0
004afb04 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004afb07 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004afb0a 51                       push       ecx
004afb0b 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004afb11 e89a29f6ff               call       0x4124b0
004afb16 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004afb19 8b8250010000             mov        eax, dword ptr [edx + 0x150]
004afb1f 50                       push       eax
004afb20 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004afb26 e88529f6ff               call       0x4124b0
004afb2b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004afb2e c7410800000000           mov        dword ptr [ecx + 8], 0
004afb35 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004afb38 81c1f8000000             add        ecx, 0xf8
004afb3e e88d01faff               call       0x44fcd0
004afb43 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004afb46 81c110010000             add        ecx, 0x110
004afb4c e87f01faff               call       0x44fcd0
004afb51 90                       nop        
004afb52 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004afb59 eb09                     jmp        0x4afb64
004afb5b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004afb5e 83c201                   add        edx, 1
004afb61 8955ec                   mov        dword ptr [ebp - 0x14], edx
004afb64 837dec0a                 cmp        dword ptr [ebp - 0x14], 0xa
004afb68 7d21                     jge        0x4afb8b
004afb6a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004afb6d 0594000000               add        eax, 0x94
004afb72 8945e8                   mov        dword ptr [ebp - 0x18], eax
004afb75 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004afb78 51                       push       ecx
004afb79 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004afb7c e8ff49f6ff               call       0x414580
004afb81 8bc8                     mov        ecx, eax
004afb83 e84801faff               call       0x44fcd0
004afb88 90                       nop        
004afb89 ebd0                     jmp        0x4afb5b
004afb8b 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004afb8e 83c248                   add        edx, 0x48
004afb91 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004afb94 6a00                     push       0
004afb96 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004afb99 e8e249f6ff               call       0x414580
004afb9e 8bc8                     mov        ecx, eax
004afba0 e82b01faff               call       0x44fcd0
004afba5 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004afba8 83c048                   add        eax, 0x48
004afbab 8945e0                   mov        dword ptr [ebp - 0x20], eax
004afbae 6a01                     push       1
004afbb0 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004afbb3 e8c849f6ff               call       0x414580
004afbb8 8bc8                     mov        ecx, eax
004afbba e81101faff               call       0x44fcd0
004afbbf 6a00                     push       0
004afbc1 6a05                     push       5
004afbc3 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004afbc9 e82268fdff               call       0x4863f0
004afbce c705a4065c0000000000     mov        dword ptr [0x5c06a4], 0
004afbd8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004afbdb e81002f7ff               call       0x41fdf0
004afbe0 90                       nop        
004afbe1 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004afbe4 64890d00000000           mov        dword ptr fs:[0], ecx
004afbeb 59                       pop        ecx
004afbec 8be5                     mov        esp, ebp
004afbee 5d                       pop        ebp
004afbef c3                       ret        
004afbf0 cc                       int3       
004afbf1 cc                       int3       
004afbf2 cc                       int3       
004afbf3 cc                       int3       
004afbf4 cc                       int3       
004afbf5 cc                       int3       
004afbf6 cc                       int3       
004afbf7 cc                       int3       
004afbf8 cc                       int3       
004afbf9 cc                       int3       
004afbfa cc                       int3       
004afbfb cc                       int3       
004afbfc cc                       int3       
004afbfd cc                       int3       
004afbfe cc                       int3       
004afbff cc                       int3       
