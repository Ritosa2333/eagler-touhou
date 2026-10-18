0052c700 55                       push       ebp
0052c701 8bec                     mov        ebp, esp
0052c703 83ec08                   sub        esp, 8
0052c706 894df8                   mov        dword ptr [ebp - 8], ecx
0052c709 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052c70c 83781400                 cmp        dword ptr [eax + 0x14], 0
0052c710 7409                     je         0x52c71b
0052c712 c745fc01000000           mov        dword ptr [ebp - 4], 1
0052c719 eb07                     jmp        0x52c722
0052c71b c745fc00000000           mov        dword ptr [ebp - 4], 0
0052c722 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
0052c726 8be5                     mov        esp, ebp
0052c728 5d                       pop        ebp
0052c729 c3                       ret        
0052c72a cc                       int3       
0052c72b cc                       int3       
0052c72c cc                       int3       
0052c72d cc                       int3       
0052c72e cc                       int3       
0052c72f cc                       int3       
