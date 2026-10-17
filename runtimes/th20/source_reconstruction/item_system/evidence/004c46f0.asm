004c46f0 55                       push       ebp
004c46f1 8bec                     mov        ebp, esp
004c46f3 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
004c46fa 7416                     je         0x4c4712
004c46fc 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004c4702 e829f9f5ff               call       0x424030
004c4707 85c0                     test       eax, eax
004c4709 7407                     je         0x4c4712
004c470b b801000000               mov        eax, 1
004c4710 eb2b                     jmp        0x4c473d
004c4712 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
004c4719 741a                     je         0x4c4735
004c471b a128a85b00               mov        eax, dword ptr [0x5ba828]
004c4720 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004c4726 c1e90b                   shr        ecx, 0xb
004c4729 83e101                   and        ecx, 1
004c472c 7407                     je         0x4c4735
004c472e b801000000               mov        eax, 1
004c4733 eb08                     jmp        0x4c473d
004c4735 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c4738 e863deffff               call       0x4c25a0
004c473d 5d                       pop        ebp
004c473e c3                       ret        
004c473f cc                       int3       
