00477c20 55                       push       ebp
00477c21 8bec                     mov        ebp, esp
00477c23 83ec08                   sub        esp, 8
00477c26 894dfc                   mov        dword ptr [ebp - 4], ecx
00477c29 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477c2c 8b4834                   mov        ecx, dword ptr [eax + 0x34]
00477c2f 51                       push       ecx
00477c30 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00477c36 e865faffff               call       0x4776a0
00477c3b 90                       nop        
00477c3c 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477c3f 837a1400                 cmp        dword ptr [edx + 0x14], 0
00477c43 7441                     je         0x477c86
00477c45 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477c48 e8e3040000               call       0x478130
00477c4d 0fb6c0                   movzx      eax, al
00477c50 85c0                     test       eax, eax
00477c52 7412                     je         0x477c66
00477c54 6a00                     push       0
00477c56 e805040000               call       0x478060
00477c5b 83c404                   add        esp, 4
00477c5e 8bc8                     mov        ecx, eax
00477c60 e82b050000               call       0x478190
00477c65 90                       nop        
00477c66 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477c69 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00477c6c 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477c6f 8b12                     mov        edx, dword ptr [edx]
00477c71 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00477c74 8b4208                   mov        eax, dword ptr [edx + 8]
00477c77 ffd0                     call       eax
00477c79 85c0                     test       eax, eax
00477c7b 7409                     je         0x477c86
00477c7d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477c80 e87b020000               call       0x477f00
00477c85 90                       nop        
00477c86 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477c89 83c11c                   add        ecx, 0x1c
00477c8c 894df8                   mov        dword ptr [ebp - 8], ecx
00477c8f 6a00                     push       0
00477c91 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00477c94 e8a7b8faff               call       0x423540
00477c99 b801000000               mov        eax, 1
00477c9e 8be5                     mov        esp, ebp
00477ca0 5d                       pop        ebp
00477ca1 c3                       ret        
00477ca2 cc                       int3       
00477ca3 cc                       int3       
00477ca4 cc                       int3       
00477ca5 cc                       int3       
00477ca6 cc                       int3       
00477ca7 cc                       int3       
00477ca8 cc                       int3       
00477ca9 cc                       int3       
00477caa cc                       int3       
00477cab cc                       int3       
00477cac cc                       int3       
00477cad cc                       int3       
00477cae cc                       int3       
00477caf cc                       int3       
