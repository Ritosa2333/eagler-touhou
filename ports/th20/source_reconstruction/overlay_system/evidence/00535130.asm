00535130 55                       push       ebp
00535131 8bec                     mov        ebp, esp
00535133 83ec24                   sub        esp, 0x24
00535136 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00535139 c745f460695c00           mov        dword ptr [ebp - 0xc], 0x5c6960
00535140 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00535143 e8586cedff               call       0x40bda0
00535148 8945fc                   mov        dword ptr [ebp - 4], eax
0053514b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053514e e81dc8eeff               call       0x421970
00535153 8945e8                   mov        dword ptr [ebp - 0x18], eax
00535156 eb09                     jmp        0x535161
00535158 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053515b 83c004                   add        eax, 4
0053515e 8945fc                   mov        dword ptr [ebp - 4], eax
00535161 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535164 3b4de8                   cmp        ecx, dword ptr [ebp - 0x18]
00535167 741b                     je         0x535184
00535169 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053516c 8955f0                   mov        dword ptr [ebp - 0x10], edx
0053516f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00535172 e859abf1ff               call       0x44fcd0
00535177 6a00                     push       0
00535179 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053517c e81fc6edff               call       0x4117a0
00535181 90                       nop        
00535182 ebd4                     jmp        0x535158
00535184 c745ec74695c00           mov        dword ptr [ebp - 0x14], 0x5c6974
0053518b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0053518e e80d6cedff               call       0x40bda0
00535193 8945f8                   mov        dword ptr [ebp - 8], eax
00535196 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00535199 e8d2c7eeff               call       0x421970
0053519e 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005351a1 eb09                     jmp        0x5351ac
005351a3 8b45f8                   mov        eax, dword ptr [ebp - 8]
005351a6 83c004                   add        eax, 4
005351a9 8945f8                   mov        dword ptr [ebp - 8], eax
005351ac 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005351af 3b4de4                   cmp        ecx, dword ptr [ebp - 0x1c]
005351b2 7411                     je         0x5351c5
005351b4 8b55f8                   mov        edx, dword ptr [ebp - 8]
005351b7 8955e0                   mov        dword ptr [ebp - 0x20], edx
005351ba 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005351bd e83ecdf8ff               call       0x4c1f00
005351c2 90                       nop        
005351c3 ebde                     jmp        0x5351a3
005351c5 8be5                     mov        esp, ebp
005351c7 5d                       pop        ebp
005351c8 c3                       ret        
005351c9 cc                       int3       
005351ca cc                       int3       
005351cb cc                       int3       
005351cc cc                       int3       
005351cd cc                       int3       
005351ce cc                       int3       
005351cf cc                       int3       
