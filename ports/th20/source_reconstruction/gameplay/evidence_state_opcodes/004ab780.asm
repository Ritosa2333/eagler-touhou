004ab780 55                       push       ebp
004ab781 8bec                     mov        ebp, esp
004ab783 83ec10                   sub        esp, 0x10
004ab786 894dfc                   mov        dword ptr [ebp - 4], ecx
004ab789 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ab78c 81c140030000             add        ecx, 0x340
004ab792 e869020000               call       0x4aba00
004ab797 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ab79a 83c101                   add        ecx, 1
004ab79d 3bc1                     cmp        eax, ecx
004ab79f 731c                     jae        0x4ab7bd
004ab7a1 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ab7a4 81c240030000             add        edx, 0x340
004ab7aa 8955f8                   mov        dword ptr [ebp - 8], edx
004ab7ad 8b4508                   mov        eax, dword ptr [ebp + 8]
004ab7b0 83c001                   add        eax, 1
004ab7b3 50                       push       eax
004ab7b4 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004ab7b7 e8b4fdffff               call       0x4ab570
004ab7bc 90                       nop        
004ab7bd 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
004ab7c1 742c                     je         0x4ab7ef
004ab7c3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ab7c6 81c140030000             add        ecx, 0x340
004ab7cc 894df4                   mov        dword ptr [ebp - 0xc], ecx
004ab7cf 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004ab7d2 52                       push       edx
004ab7d3 6a40                     push       0x40
004ab7d5 8b4508                   mov        eax, dword ptr [ebp + 8]
004ab7d8 50                       push       eax
004ab7d9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004ab7dc e87fe9ffff               call       0x4aa160
004ab7e1 83c048                   add        eax, 0x48
004ab7e4 50                       push       eax
004ab7e5 e826ce0900               call       0x548610
004ab7ea 83c40c                   add        esp, 0xc
004ab7ed eb25                     jmp        0x4ab814
004ab7ef 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ab7f2 81c140030000             add        ecx, 0x340
004ab7f8 894df0                   mov        dword ptr [ebp - 0x10], ecx
004ab7fb 8b5508                   mov        edx, dword ptr [ebp + 8]
004ab7fe 52                       push       edx
004ab7ff 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004ab802 e83905feff               call       0x48bd40
004ab807 b901000000               mov        ecx, 1
004ab80c 6bd100                   imul       edx, ecx, 0
004ab80f c644104800               mov        byte ptr [eax + edx + 0x48], 0
004ab814 8be5                     mov        esp, ebp
004ab816 5d                       pop        ebp
004ab817 c20800                   ret        8
004ab81a cc                       int3       
004ab81b cc                       int3       
004ab81c cc                       int3       
004ab81d cc                       int3       
004ab81e cc                       int3       
004ab81f cc                       int3       
