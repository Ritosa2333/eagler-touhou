004c5eb0 55                       push       ebp
004c5eb1 8bec                     mov        ebp, esp
004c5eb3 83ec08                   sub        esp, 8
004c5eb6 894df8                   mov        dword ptr [ebp - 8], ecx
004c5eb9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c5ebc 833802                   cmp        dword ptr [eax], 2
004c5ebf 7509                     jne        0x4c5eca
004c5ec1 c745fc01000000           mov        dword ptr [ebp - 4], 1
004c5ec8 eb07                     jmp        0x4c5ed1
004c5eca c745fc00000000           mov        dword ptr [ebp - 4], 0
004c5ed1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c5ed4 8be5                     mov        esp, ebp
004c5ed6 5d                       pop        ebp
004c5ed7 c3                       ret        
004c5ed8 cc                       int3       
004c5ed9 cc                       int3       
004c5eda cc                       int3       
004c5edb cc                       int3       
004c5edc cc                       int3       
004c5edd cc                       int3       
004c5ede cc                       int3       
004c5edf cc                       int3       
