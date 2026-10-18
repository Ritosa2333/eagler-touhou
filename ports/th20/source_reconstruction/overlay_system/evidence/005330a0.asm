005330a0 55                       push       ebp
005330a1 8bec                     mov        ebp, esp
005330a3 51                       push       ecx
005330a4 56                       push       esi
005330a5 894dfc                   mov        dword ptr [ebp - 4], ecx
005330a8 0fb64508                 movzx      eax, byte ptr [ebp + 8]
005330ac 85c0                     test       eax, eax
005330ae 7458                     je         0x533108
005330b0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005330b3 8b5128                   mov        edx, dword ptr [ecx + 0x28]
005330b6 8b45fc                   mov        eax, dword ptr [ebp - 4]
005330b9 8b12                     mov        edx, dword ptr [edx]
005330bb 8b4828                   mov        ecx, dword ptr [eax + 0x28]
005330be 8b421c                   mov        eax, dword ptr [edx + 0x1c]
005330c1 ffd0                     call       eax
005330c3 90                       nop        
005330c4 6a00                     push       0
005330c6 e865d7f2ff               call       0x460830
005330cb 83c404                   add        esp, 4
005330ce 8bc8                     mov        ecx, eax
005330d0 e80bc5fcff               call       0x4ff5e0
005330d5 0fb6c8                   movzx      ecx, al
005330d8 85c9                     test       ecx, ecx
005330da 7416                     je         0x5330f2
005330dc 8b55fc                   mov        edx, dword ptr [ebp - 4]
005330df 8b422c                   mov        eax, dword ptr [edx + 0x2c]
005330e2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005330e5 8b10                     mov        edx, dword ptr [eax]
005330e7 8b492c                   mov        ecx, dword ptr [ecx + 0x2c]
005330ea 8b4220                   mov        eax, dword ptr [edx + 0x20]
005330ed ffd0                     call       eax
005330ef 90                       nop        
005330f0 eb14                     jmp        0x533106
005330f2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005330f5 8b5130                   mov        edx, dword ptr [ecx + 0x30]
005330f8 8b45fc                   mov        eax, dword ptr [ebp - 4]
005330fb 8b12                     mov        edx, dword ptr [edx]
005330fd 8b4830                   mov        ecx, dword ptr [eax + 0x30]
00533100 8b4224                   mov        eax, dword ptr [edx + 0x24]
00533103 ffd0                     call       eax
00533105 90                       nop        
00533106 eb6a                     jmp        0x533172
00533108 6a00                     push       0
0053310a e8710ff3ff               call       0x464080
0053310f 83c404                   add        esp, 4
00533112 8bc8                     mov        ecx, eax
00533114 e8e7e5edff               call       0x411700
00533119 8bf0                     mov        esi, eax
0053311b 6a00                     push       0
0053311d e85e0ff3ff               call       0x464080
00533122 83c404                   add        esp, 4
00533125 8bc8                     mov        ecx, eax
00533127 e86499eeff               call       0x41ca90
0053312c 3bf0                     cmp        esi, eax
0053312e 7442                     je         0x533172
00533130 6a00                     push       0
00533132 e8f9d6f2ff               call       0x460830
00533137 83c404                   add        esp, 4
0053313a 8bc8                     mov        ecx, eax
0053313c e89fc4fcff               call       0x4ff5e0
00533141 0fb6c8                   movzx      ecx, al
00533144 85c9                     test       ecx, ecx
00533146 7516                     jne        0x53315e
00533148 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053314b 8b422c                   mov        eax, dword ptr [edx + 0x2c]
0053314e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00533151 8b10                     mov        edx, dword ptr [eax]
00533153 8b492c                   mov        ecx, dword ptr [ecx + 0x2c]
00533156 8b4220                   mov        eax, dword ptr [edx + 0x20]
00533159 ffd0                     call       eax
0053315b 90                       nop        
0053315c eb14                     jmp        0x533172
0053315e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00533161 8b5130                   mov        edx, dword ptr [ecx + 0x30]
00533164 8b45fc                   mov        eax, dword ptr [ebp - 4]
00533167 8b12                     mov        edx, dword ptr [edx]
00533169 8b4830                   mov        ecx, dword ptr [eax + 0x30]
0053316c 8b4224                   mov        eax, dword ptr [edx + 0x24]
0053316f ffd0                     call       eax
00533171 90                       nop        
00533172 5e                       pop        esi
00533173 8be5                     mov        esp, ebp
00533175 5d                       pop        ebp
00533176 c20400                   ret        4
00533179 cc                       int3       
0053317a cc                       int3       
0053317b cc                       int3       
0053317c cc                       int3       
0053317d cc                       int3       
0053317e cc                       int3       
0053317f cc                       int3       
