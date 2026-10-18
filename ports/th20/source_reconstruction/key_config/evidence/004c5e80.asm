004c5e80 55                       push       ebp
004c5e81 8bec                     mov        ebp, esp
004c5e83 83ec08                   sub        esp, 8
004c5e86 894df8                   mov        dword ptr [ebp - 8], ecx
004c5e89 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c5e8c 833800                   cmp        dword ptr [eax], 0
004c5e8f 7509                     jne        0x4c5e9a
004c5e91 c745fc01000000           mov        dword ptr [ebp - 4], 1
004c5e98 eb07                     jmp        0x4c5ea1
004c5e9a c745fc00000000           mov        dword ptr [ebp - 4], 0
004c5ea1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c5ea4 8be5                     mov        esp, ebp
004c5ea6 5d                       pop        ebp
004c5ea7 c3                       ret        
004c5ea8 cc                       int3       
004c5ea9 cc                       int3       
004c5eaa cc                       int3       
004c5eab cc                       int3       
004c5eac cc                       int3       
004c5ead cc                       int3       
004c5eae cc                       int3       
004c5eaf cc                       int3       
