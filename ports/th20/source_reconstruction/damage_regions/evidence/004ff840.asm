004ff840 55                       push       ebp
004ff841 8bec                     mov        ebp, esp
004ff843 83ec08                   sub        esp, 8
004ff846 894df8                   mov        dword ptr [ebp - 8], ecx
004ff849 8b45f8                   mov        eax, dword ptr [ebp - 8]
004ff84c 83785401                 cmp        dword ptr [eax + 0x54], 1
004ff850 7509                     jne        0x4ff85b
004ff852 c745fc01000000           mov        dword ptr [ebp - 4], 1
004ff859 eb07                     jmp        0x4ff862
004ff85b c745fc00000000           mov        dword ptr [ebp - 4], 0
004ff862 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
004ff866 8be5                     mov        esp, ebp
004ff868 5d                       pop        ebp
004ff869 c3                       ret        
004ff86a cc                       int3       
004ff86b cc                       int3       
004ff86c cc                       int3       
004ff86d cc                       int3       
004ff86e cc                       int3       
004ff86f cc                       int3       
