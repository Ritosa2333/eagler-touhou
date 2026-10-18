0047d960 55                       push       ebp
0047d961 8bec                     mov        ebp, esp
0047d963 83ec14                   sub        esp, 0x14
0047d966 894dfc                   mov        dword ptr [ebp - 4], ecx
0047d969 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047d96c 8b4814                   mov        ecx, dword ptr [eax + 0x14]
0047d96f c1e90a                   shr        ecx, 0xa
0047d972 83e101                   and        ecx, 1
0047d975 7514                     jne        0x47d98b
0047d977 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047d97a e8e1810000               call       0x485b60
0047d97f 85c0                     test       eax, eax
0047d981 7408                     je         0x47d98b
0047d983 83c8ff                   or         eax, 0xffffffff
0047d986 e9a4000000               jmp        0x47da2f
0047d98b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047d98e 83c25c                   add        edx, 0x5c
0047d991 8955f4                   mov        dword ptr [ebp - 0xc], edx
0047d994 6a00                     push       0
0047d996 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047d999 e8b2ecffff               call       0x47c650
0047d99e 0fb6c0                   movzx      eax, al
0047d9a1 85c0                     test       eax, eax
0047d9a3 7419                     je         0x47d9be
0047d9a5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047d9a8 83c15c                   add        ecx, 0x5c
0047d9ab 894df0                   mov        dword ptr [ebp - 0x10], ecx
0047d9ae 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047d9b1 83c264                   add        edx, 0x64
0047d9b4 52                       push       edx
0047d9b5 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047d9b8 e80329fdff               call       0x4502c0
0047d9bd 90                       nop        
0047d9be 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047d9c1 8b4850                   mov        ecx, dword ptr [eax + 0x50]
0047d9c4 894df8                   mov        dword ptr [ebp - 8], ecx
0047d9c7 837df801                 cmp        dword ptr [ebp - 8], 1
0047d9cb 7436                     je         0x47da03
0047d9cd 837df802                 cmp        dword ptr [ebp - 8], 2
0047d9d1 7402                     je         0x47d9d5
0047d9d3 eb46                     jmp        0x47da1b
0047d9d5 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047d9d8 81c2f8040000             add        edx, 0x4f8
0047d9de 8955ec                   mov        dword ptr [ebp - 0x14], edx
0047d9e1 6a08                     push       8
0047d9e3 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047d9e6 e8d55bfaff               call       0x4235c0
0047d9eb 0fb6c0                   movzx      eax, al
0047d9ee 85c0                     test       eax, eax
0047d9f0 7411                     je         0x47da03
0047d9f2 6a00                     push       0
0047d9f4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047d9f7 e824700000               call       0x484a20
0047d9fc 83f801                   cmp        eax, 1
0047d9ff 7502                     jne        0x47da03
0047da01 eb18                     jmp        0x47da1b
0047da03 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047da06 8b5114                   mov        edx, dword ptr [ecx + 0x14]
0047da09 c1ea09                   shr        edx, 9
0047da0c 83e201                   and        edx, 1
0047da0f 750a                     jne        0x47da1b
0047da11 6a00                     push       0
0047da13 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047da16 e805700000               call       0x484a20
0047da1b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047da1e 8b4814                   mov        ecx, dword ptr [eax + 0x14]
0047da21 81e1fffbffff             and        ecx, 0xfffffbff
0047da27 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047da2a 894a14                   mov        dword ptr [edx + 0x14], ecx
0047da2d 33c0                     xor        eax, eax
0047da2f 8be5                     mov        esp, ebp
0047da31 5d                       pop        ebp
0047da32 c3                       ret        
0047da33 cc                       int3       
0047da34 cc                       int3       
0047da35 cc                       int3       
0047da36 cc                       int3       
0047da37 cc                       int3       
0047da38 cc                       int3       
0047da39 cc                       int3       
0047da3a cc                       int3       
0047da3b cc                       int3       
0047da3c cc                       int3       
0047da3d cc                       int3       
0047da3e cc                       int3       
0047da3f cc                       int3       
