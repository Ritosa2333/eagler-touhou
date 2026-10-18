004887a0 55                       push       ebp
004887a1 8bec                     mov        ebp, esp
004887a3 83ec08                   sub        esp, 8
004887a6 894df8                   mov        dword ptr [ebp - 8], ecx
004887a9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004887ac 8b4878                   mov        ecx, dword ptr [eax + 0x78]
004887af 83e101                   and        ecx, 1
004887b2 7409                     je         0x4887bd
004887b4 c745fc01000000           mov        dword ptr [ebp - 4], 1
004887bb eb07                     jmp        0x4887c4
004887bd c745fc00000000           mov        dword ptr [ebp - 4], 0
004887c4 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
004887c8 8be5                     mov        esp, ebp
004887ca 5d                       pop        ebp
004887cb c3                       ret        
004887cc cc                       int3       
004887cd cc                       int3       
004887ce cc                       int3       
004887cf cc                       int3       
