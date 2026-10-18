00477d30 55                       push       ebp
00477d31 8bec                     mov        ebp, esp
00477d33 83ec08                   sub        esp, 8
00477d36 894dfc                   mov        dword ptr [ebp - 4], ecx
00477d39 8b4508                   mov        eax, dword ptr [ebp + 8]
00477d3c 50                       push       eax
00477d3d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477d40 e84b050000               call       0x478290
00477d45 6a00                     push       0
00477d47 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477d4a 83c108                   add        ecx, 8
00477d4d e8ceb7faff               call       0x423520
00477d52 6a00                     push       0
00477d54 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477d57 81c1ac000000             add        ecx, 0xac
00477d5d e83e9af9ff               call       0x4117a0
00477d62 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477d65 8b89b4000000             mov        ecx, dword ptr [ecx + 0xb4]
00477d6b e89099f9ff               call       0x411700
00477d70 8bc8                     mov        ecx, eax
00477d72 e839fe0000               call       0x487bb0
00477d77 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477d7a 8b8ab4000000             mov        ecx, dword ptr [edx + 0xb4]
00477d80 e8aba9f9ff               call       0x412730
00477d85 8945f8                   mov        dword ptr [ebp - 8], eax
00477d88 6a01                     push       1
00477d8a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00477d8d e8ae010000               call       0x477f40
00477d92 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477d95 8b88b4000000             mov        ecx, dword ptr [eax + 0xb4]
00477d9b e8f081f9ff               call       0x40ff90
00477da0 8bc8                     mov        ecx, eax
00477da2 e8a98afeff               call       0x460850
00477da7 50                       push       eax
00477da8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477dab 83c128                   add        ecx, 0x28
00477dae e8cdfdffff               call       0x477b80
00477db3 33c0                     xor        eax, eax
00477db5 8be5                     mov        esp, ebp
00477db7 5d                       pop        ebp
00477db8 c20400                   ret        4
00477dbb cc                       int3       
00477dbc cc                       int3       
00477dbd cc                       int3       
00477dbe cc                       int3       
00477dbf cc                       int3       
