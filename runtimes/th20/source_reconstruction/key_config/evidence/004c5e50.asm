004c5e50 55                       push       ebp
004c5e51 8bec                     mov        ebp, esp
004c5e53 83ec08                   sub        esp, 8
004c5e56 894df8                   mov        dword ptr [ebp - 8], ecx
004c5e59 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c5e5c 833801                   cmp        dword ptr [eax], 1
004c5e5f 7509                     jne        0x4c5e6a
004c5e61 c745fc01000000           mov        dword ptr [ebp - 4], 1
004c5e68 eb07                     jmp        0x4c5e71
004c5e6a c745fc00000000           mov        dword ptr [ebp - 4], 0
004c5e71 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c5e74 8be5                     mov        esp, ebp
004c5e76 5d                       pop        ebp
004c5e77 c3                       ret        
004c5e78 cc                       int3       
004c5e79 cc                       int3       
004c5e7a cc                       int3       
004c5e7b cc                       int3       
004c5e7c cc                       int3       
004c5e7d cc                       int3       
004c5e7e cc                       int3       
004c5e7f cc                       int3       
