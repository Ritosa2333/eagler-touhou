004cfb40 55                       push       ebp
004cfb41 8bec                     mov        ebp, esp
004cfb43 83ec08                   sub        esp, 8
004cfb46 894df8                   mov        dword ptr [ebp - 8], ecx
004cfb49 68d0115700               push       0x5711d0
004cfb4e 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004cfb54 e83786ffff               call       0x4c8190
004cfb59 8945fc                   mov        dword ptr [ebp - 4], eax
004cfb5c 68f8180000               push       0x18f8
004cfb61 8b45f8                   mov        eax, dword ptr [ebp - 8]
004cfb64 50                       push       eax
004cfb65 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfb68 51                       push       ecx
004cfb69 e8b2420700               call       0x543e20
004cfb6e 83c40c                   add        esp, 0xc
004cfb71 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cfb74 8be5                     mov        esp, ebp
004cfb76 5d                       pop        ebp
004cfb77 c3                       ret        
004cfb78 cc                       int3       
004cfb79 cc                       int3       
004cfb7a cc                       int3       
004cfb7b cc                       int3       
004cfb7c cc                       int3       
004cfb7d cc                       int3       
004cfb7e cc                       int3       
004cfb7f cc                       int3       
