00534180 55                       push       ebp
00534181 8bec                     mov        ebp, esp
00534183 83ec08                   sub        esp, 8
00534186 56                       push       esi
00534187 894dfc                   mov        dword ptr [ebp - 4], ecx
0053418a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053418d 8b480c                   mov        ecx, dword ptr [eax + 0xc]
00534190 894df8                   mov        dword ptr [ebp - 8], ecx
00534193 837df800                 cmp        dword ptr [ebp - 8], 0
00534197 740e                     je         0x5341a7
00534199 837df801                 cmp        dword ptr [ebp - 8], 1
0053419d 7424                     je         0x5341c3
0053419f 837df802                 cmp        dword ptr [ebp - 8], 2
005341a3 743c                     je         0x5341e1
005341a5 eb58                     jmp        0x5341ff
005341a7 8b55fc                   mov        edx, dword ptr [ebp - 4]
005341aa 6b720414                 imul       esi, dword ptr [edx + 4], 0x14
005341ae 6a00                     push       0
005341b0 e8cbfef2ff               call       0x464080
005341b5 83c404                   add        esp, 4
005341b8 8bc8                     mov        ecx, eax
005341ba e8e13ff8ff               call       0x4b81a0
005341bf 03c6                     add        eax, esi
005341c1 eb3e                     jmp        0x534201
005341c3 8b45fc                   mov        eax, dword ptr [ebp - 4]
005341c6 6b700414                 imul       esi, dword ptr [eax + 4], 0x14
005341ca 6a00                     push       0
005341cc e8affef2ff               call       0x464080
005341d1 83c404                   add        esp, 4
005341d4 8bc8                     mov        ecx, eax
005341d6 e8c53ff8ff               call       0x4b81a0
005341db 8d440605                 lea        eax, [esi + eax + 5]
005341df eb20                     jmp        0x534201
005341e1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005341e4 6b710414                 imul       esi, dword ptr [ecx + 4], 0x14
005341e8 6a00                     push       0
005341ea e891fef2ff               call       0x464080
005341ef 83c404                   add        esp, 4
005341f2 8bc8                     mov        ecx, eax
005341f4 e8a73ff8ff               call       0x4b81a0
005341f9 8d44060a                 lea        eax, [esi + eax + 0xa]
005341fd eb02                     jmp        0x534201
005341ff 33c0                     xor        eax, eax
00534201 5e                       pop        esi
00534202 8be5                     mov        esp, ebp
00534204 5d                       pop        ebp
00534205 c3                       ret        
00534206 cc                       int3       
00534207 cc                       int3       
00534208 cc                       int3       
00534209 cc                       int3       
0053420a cc                       int3       
0053420b cc                       int3       
0053420c cc                       int3       
0053420d cc                       int3       
0053420e cc                       int3       
0053420f cc                       int3       
