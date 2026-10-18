00533780 55                       push       ebp
00533781 8bec                     mov        ebp, esp
00533783 83ec24                   sub        esp, 0x24
00533786 56                       push       esi
00533787 894dfc                   mov        dword ptr [ebp - 4], ecx
0053378a 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
00533790 e8cb49f4ff               call       0x478160
00533795 85c0                     test       eax, eax
00533797 7405                     je         0x53379e
00533799 e971010000               jmp        0x53390f
0053379e 8b45fc                   mov        eax, dword ptr [ebp - 4]
005337a1 83785400                 cmp        dword ptr [eax + 0x54], 0
005337a5 7405                     je         0x5337ac
005337a7 e963010000               jmp        0x53390f
005337ac 6a00                     push       0
005337ae e8cd08f3ff               call       0x464080
005337b3 83c404                   add        esp, 4
005337b6 8bc8                     mov        ecx, eax
005337b8 e8430cf3ff               call       0x464400
005337bd 0fb6c8                   movzx      ecx, al
005337c0 85c9                     test       ecx, ecx
005337c2 7409                     je         0x5337cd
005337c4 c745f80f000000           mov        dword ptr [ebp - 8], 0xf
005337cb eb07                     jmp        0x5337d4
005337cd c745f80a000000           mov        dword ptr [ebp - 8], 0xa
005337d4 8b55f8                   mov        edx, dword ptr [ebp - 8]
005337d7 8955f4                   mov        dword ptr [ebp - 0xc], edx
005337da 6a00                     push       0
005337dc e89f08f3ff               call       0x464080
005337e1 83c404                   add        esp, 4
005337e4 8bc8                     mov        ecx, eax
005337e6 e8a50b0000               call       0x534390
005337eb 0fb6c0                   movzx      eax, al
005337ee 85c0                     test       eax, eax
005337f0 743c                     je         0x53382e
005337f2 6a00                     push       0
005337f4 e88708f3ff               call       0x464080
005337f9 83c404                   add        esp, 4
005337fc 8bc8                     mov        ecx, eax
005337fe e89d80feff               call       0x51b8a0
00533803 99                       cdq        
00533804 f77df4                   idiv       dword ptr [ebp - 0xc]
00533807 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053380a 83e901                   sub        ecx, 1
0053380d 3bd1                     cmp        edx, ecx
0053380f 751d                     jne        0x53382e
00533811 8b5508                   mov        edx, dword ptr [ebp + 8]
00533814 83c201                   add        edx, 1
00533817 895508                   mov        dword ptr [ebp + 8], edx
0053381a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053381d 8b4834                   mov        ecx, dword ptr [eax + 0x34]
00533820 894df0                   mov        dword ptr [ebp - 0x10], ecx
00533823 6a01                     push       1
00533825 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00533828 e8832df5ff               call       0x4865b0
0053382d 90                       nop        
0053382e 6a00                     push       0
00533830 e84b08f3ff               call       0x464080
00533835 83c404                   add        esp, 4
00533838 8bc8                     mov        ecx, eax
0053383a e86180feff               call       0x51b8a0
0053383f 8bf0                     mov        esi, eax
00533841 6a00                     push       0
00533843 e83808f3ff               call       0x464080
00533848 83c404                   add        esp, 4
0053384b 8bc8                     mov        ecx, eax
0053384d e8ce80feff               call       0x51b920
00533852 3bf0                     cmp        esi, eax
00533854 7c41                     jl         0x533897
00533856 8b55fc                   mov        edx, dword ptr [ebp - 4]
00533859 837a5400                 cmp        dword ptr [edx + 0x54], 0
0053385d 7536                     jne        0x533895
0053385f 6a00                     push       0
00533861 e81a08f3ff               call       0x464080
00533866 83c404                   add        esp, 4
00533869 8945e8                   mov        dword ptr [ebp - 0x18], eax
0053386c 6a00                     push       0
0053386e e80d08f3ff               call       0x464080
00533873 83c404                   add        esp, 4
00533876 8bc8                     mov        ecx, eax
00533878 e8a380feff               call       0x51b920
0053387d 8945ec                   mov        dword ptr [ebp - 0x14], eax
00533880 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00533883 50                       push       eax
00533884 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00533887 e8b4b0f8ff               call       0x4be940
0053388c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053388f e86c140000               call       0x534d00
00533894 90                       nop        
00533895 eb78                     jmp        0x53390f
00533897 6a00                     push       0
00533899 e8e207f3ff               call       0x464080
0053389e 83c404                   add        esp, 4
005338a1 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005338a4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005338a7 51                       push       ecx
005338a8 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
005338ab e870000000               call       0x533920
005338b0 90                       nop        
005338b1 6a00                     push       0
005338b3 e8c807f3ff               call       0x464080
005338b8 83c404                   add        esp, 4
005338bb 8bc8                     mov        ecx, eax
005338bd e8de7ffeff               call       0x51b8a0
005338c2 8bf0                     mov        esi, eax
005338c4 6a00                     push       0
005338c6 e8b507f3ff               call       0x464080
005338cb 83c404                   add        esp, 4
005338ce 8bc8                     mov        ecx, eax
005338d0 e84b80feff               call       0x51b920
005338d5 3bf0                     cmp        esi, eax
005338d7 7c36                     jl         0x53390f
005338d9 6a00                     push       0
005338db e8a007f3ff               call       0x464080
005338e0 83c404                   add        esp, 4
005338e3 8945dc                   mov        dword ptr [ebp - 0x24], eax
005338e6 6a00                     push       0
005338e8 e89307f3ff               call       0x464080
005338ed 83c404                   add        esp, 4
005338f0 8bc8                     mov        ecx, eax
005338f2 e82980feff               call       0x51b920
005338f7 8945e0                   mov        dword ptr [ebp - 0x20], eax
005338fa 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
005338fd 52                       push       edx
005338fe 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00533901 e83ab0f8ff               call       0x4be940
00533906 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00533909 e8f2130000               call       0x534d00
0053390e 90                       nop        
0053390f 5e                       pop        esi
00533910 8be5                     mov        esp, ebp
00533912 5d                       pop        ebp
00533913 c20400                   ret        4
00533916 cc                       int3       
00533917 cc                       int3       
00533918 cc                       int3       
00533919 cc                       int3       
0053391a cc                       int3       
0053391b cc                       int3       
0053391c cc                       int3       
0053391d cc                       int3       
0053391e cc                       int3       
0053391f cc                       int3       
