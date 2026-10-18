00474dd0 55                       push       ebp
00474dd1 8bec                     mov        ebp, esp
00474dd3 83ec08                   sub        esp, 8
00474dd6 894df8                   mov        dword ptr [ebp - 8], ecx
00474dd9 8b45f8                   mov        eax, dword ptr [ebp - 8]
00474ddc 83782800                 cmp        dword ptr [eax + 0x28], 0
00474de0 7c09                     jl         0x474deb
00474de2 c745fc01000000           mov        dword ptr [ebp - 4], 1
00474de9 eb07                     jmp        0x474df2
00474deb c745fc00000000           mov        dword ptr [ebp - 4], 0
00474df2 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
00474df6 8be5                     mov        esp, ebp
00474df8 5d                       pop        ebp
00474df9 c3                       ret        
00474dfa cc                       int3       
00474dfb cc                       int3       
00474dfc cc                       int3       
00474dfd cc                       int3       
00474dfe cc                       int3       
00474dff cc                       int3       
