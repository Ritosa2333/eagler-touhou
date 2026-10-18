004deb80 55                       push       ebp
004deb81 8bec                     mov        ebp, esp
004deb83 6aff                     push       -1
004deb85 6800765600               push       0x567600
004deb8a 64a100000000             mov        eax, dword ptr fs:[0]
004deb90 50                       push       eax
004deb91 51                       push       ecx
004deb92 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004deb97 33c5                     xor        eax, ebp
004deb99 50                       push       eax
004deb9a 8d45f4                   lea        eax, [ebp - 0xc]
004deb9d 64a300000000             mov        dword ptr fs:[0], eax
004deba3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004deba6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004deba9 c700a81e5700             mov        dword ptr [eax], 0x571ea8
004debaf 68941f5700               push       0x571f94
004debb4 e8f7daf2ff               call       0x40c6b0
004debb9 83c404                   add        esp, 4
004debbc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004debbf 8b5108                   mov        edx, dword ptr [ecx + 8]
004debc2 52                       push       edx
004debc3 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004debc9 e8e238f3ff               call       0x4124b0
004debce 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004debd1 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004debd4 51                       push       ecx
004debd5 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004debdb e8d038f3ff               call       0x4124b0
004debe0 6a0e                     push       0xe
004debe2 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004debe8 e843d8f6ff               call       0x44c430
004debed 6a16                     push       0x16
004debef 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004debf5 e836d8f6ff               call       0x44c430
004debfa c705385b5c0000000000     mov        dword ptr [0x5c5b38], 0
004dec04 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dec07 83c124                   add        ecx, 0x24
004dec0a e8610efdff               call       0x4afa70
004dec0f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dec12 e8d911f4ff               call       0x41fdf0
004dec17 90                       nop        
004dec18 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004dec1b 64890d00000000           mov        dword ptr fs:[0], ecx
004dec22 59                       pop        ecx
004dec23 8be5                     mov        esp, ebp
004dec25 5d                       pop        ebp
004dec26 c3                       ret        
004dec27 cc                       int3       
004dec28 cc                       int3       
004dec29 cc                       int3       
004dec2a cc                       int3       
004dec2b cc                       int3       
004dec2c cc                       int3       
004dec2d cc                       int3       
004dec2e cc                       int3       
004dec2f cc                       int3       
