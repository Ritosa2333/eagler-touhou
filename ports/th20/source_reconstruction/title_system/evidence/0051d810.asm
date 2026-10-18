0051d810 55                       push       ebp
0051d811 8bec                     mov        ebp, esp
0051d813 83ec0c                   sub        esp, 0xc
0051d816 894dfc                   mov        dword ptr [ebp - 4], ecx
0051d819 8b4508                   mov        eax, dword ptr [ebp + 8]
0051d81c 50                       push       eax
0051d81d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051d820 e8abccf4ff               call       0x46a4d0
0051d825 0fb6c8                   movzx      ecx, al
0051d828 85c9                     test       ecx, ecx
0051d82a 7402                     je         0x51d82e
0051d82c eb56                     jmp        0x51d884
0051d82e 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d831 83c210                   add        edx, 0x10
0051d834 8955f4                   mov        dword ptr [ebp - 0xc], edx
0051d837 8b4508                   mov        eax, dword ptr [ebp + 8]
0051d83a 8945f8                   mov        dword ptr [ebp - 8], eax
0051d83d 8d4df8                   lea        ecx, [ebp - 8]
0051d840 51                       push       ecx
0051d841 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051d844 e8678ffcff               call       0x4e67b0
0051d849 90                       nop        
0051d84a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d84d 8b02                     mov        eax, dword ptr [edx]
0051d84f 50                       push       eax
0051d850 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051d853 e878ccf4ff               call       0x46a4d0
0051d858 0fb6c8                   movzx      ecx, al
0051d85b 85c9                     test       ecx, ecx
0051d85d 7425                     je         0x51d884
0051d85f 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d862 8b02                     mov        eax, dword ptr [edx]
0051d864 83c001                   add        eax, 1
0051d867 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051d86a 8901                     mov        dword ptr [ecx], eax
0051d86c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d86f 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051d872 8b0a                     mov        ecx, dword ptr [edx]
0051d874 3b4808                   cmp        ecx, dword ptr [eax + 8]
0051d877 7c09                     jl         0x51d882
0051d879 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d87c c70200000000             mov        dword ptr [edx], 0
0051d882 ebc6                     jmp        0x51d84a
0051d884 8be5                     mov        esp, ebp
0051d886 5d                       pop        ebp
0051d887 c20400                   ret        4
0051d88a cc                       int3       
0051d88b cc                       int3       
0051d88c cc                       int3       
0051d88d cc                       int3       
0051d88e cc                       int3       
0051d88f cc                       int3       
