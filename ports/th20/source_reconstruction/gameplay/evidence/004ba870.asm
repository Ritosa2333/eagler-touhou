004ba870 55                       push       ebp
004ba871 8bec                     mov        ebp, esp
004ba873 51                       push       ecx
004ba874 894dfc                   mov        dword ptr [ebp - 4], ecx
004ba877 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004ba87d e8eea8fbff               call       0x475170
004ba882 90                       nop        
004ba883 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ba886 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004ba88c c1e902                   shr        ecx, 2
004ba88f 83e101                   and        ecx, 1
004ba892 7407                     je         0x4ba89b
004ba894 b801000000               mov        eax, 1
004ba899 eb05                     jmp        0x4ba8a0
004ba89b b801000000               mov        eax, 1
004ba8a0 8be5                     mov        esp, ebp
004ba8a2 5d                       pop        ebp
004ba8a3 c3                       ret        
004ba8a4 cc                       int3       
004ba8a5 cc                       int3       
004ba8a6 cc                       int3       
004ba8a7 cc                       int3       
004ba8a8 cc                       int3       
004ba8a9 cc                       int3       
004ba8aa cc                       int3       
004ba8ab cc                       int3       
004ba8ac cc                       int3       
004ba8ad cc                       int3       
004ba8ae cc                       int3       
004ba8af cc                       int3       
