0051d790 55                       push       ebp
0051d791 8bec                     mov        ebp, esp
0051d793 83ec08                   sub        esp, 8
0051d796 894dfc                   mov        dword ptr [ebp - 4], ecx
0051d799 8b4508                   mov        eax, dword ptr [ebp + 8]
0051d79c 50                       push       eax
0051d79d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051d7a0 e82bcdf4ff               call       0x46a4d0
0051d7a5 0fb6c8                   movzx      ecx, al
0051d7a8 85c9                     test       ecx, ecx
0051d7aa 7402                     je         0x51d7ae
0051d7ac eb50                     jmp        0x51d7fe
0051d7ae 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d7b1 83c210                   add        edx, 0x10
0051d7b4 8955f8                   mov        dword ptr [ebp - 8], edx
0051d7b7 8d4508                   lea        eax, [ebp + 8]
0051d7ba 50                       push       eax
0051d7bb 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0051d7be e80df40000               call       0x52cbd0
0051d7c3 90                       nop        
0051d7c4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051d7c7 8b11                     mov        edx, dword ptr [ecx]
0051d7c9 52                       push       edx
0051d7ca 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051d7cd e8feccf4ff               call       0x46a4d0
0051d7d2 0fb6c0                   movzx      eax, al
0051d7d5 85c0                     test       eax, eax
0051d7d7 7425                     je         0x51d7fe
0051d7d9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051d7dc 8b11                     mov        edx, dword ptr [ecx]
0051d7de 83c201                   add        edx, 1
0051d7e1 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051d7e4 8910                     mov        dword ptr [eax], edx
0051d7e6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051d7e9 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d7ec 8b01                     mov        eax, dword ptr [ecx]
0051d7ee 3b4208                   cmp        eax, dword ptr [edx + 8]
0051d7f1 7c09                     jl         0x51d7fc
0051d7f3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051d7f6 c70100000000             mov        dword ptr [ecx], 0
0051d7fc ebc6                     jmp        0x51d7c4
0051d7fe 8be5                     mov        esp, ebp
0051d800 5d                       pop        ebp
0051d801 c20400                   ret        4
0051d804 cc                       int3       
0051d805 cc                       int3       
0051d806 cc                       int3       
0051d807 cc                       int3       
0051d808 cc                       int3       
0051d809 cc                       int3       
0051d80a cc                       int3       
0051d80b cc                       int3       
0051d80c cc                       int3       
0051d80d cc                       int3       
0051d80e cc                       int3       
0051d80f cc                       int3       
