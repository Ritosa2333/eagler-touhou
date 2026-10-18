0041dbe0 55                       push       ebp
0041dbe1 8bec                     mov        ebp, esp
0041dbe3 83ec18                   sub        esp, 0x18
0041dbe6 894dec                   mov        dword ptr [ebp - 0x14], ecx
0041dbe9 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0041dbec 0530070006               add        eax, 0x6000730
0041dbf1 8945f0                   mov        dword ptr [ebp - 0x10], eax
0041dbf4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0041dbf7 e8a4e1feff               call       0x40bda0
0041dbfc 8945f4                   mov        dword ptr [ebp - 0xc], eax
0041dbff 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0041dc02 e869e2ffff               call       0x41be70
0041dc07 8945e8                   mov        dword ptr [ebp - 0x18], eax
0041dc0a eb09                     jmp        0x41dc15
0041dc0c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0041dc0f 83c104                   add        ecx, 4
0041dc12 894df4                   mov        dword ptr [ebp - 0xc], ecx
0041dc15 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0041dc18 3b55e8                   cmp        edx, dword ptr [ebp - 0x18]
0041dc1b 0f8498000000             je         0x41dcb9
0041dc21 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0041dc24 8945f8                   mov        dword ptr [ebp - 8], eax
0041dc27 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0041dc2a 833900                   cmp        dword ptr [ecx], 0
0041dc2d 0f8481000000             je         0x41dcb4
0041dc33 c745fc00000000           mov        dword ptr [ebp - 4], 0
0041dc3a eb09                     jmp        0x41dc45
0041dc3c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041dc3f 83c201                   add        edx, 1
0041dc42 8955fc                   mov        dword ptr [ebp - 4], edx
0041dc45 8b45f8                   mov        eax, dword ptr [ebp - 8]
0041dc48 8b08                     mov        ecx, dword ptr [eax]
0041dc4a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041dc4d 3b5144                   cmp        edx, dword ptr [ecx + 0x44]
0041dc50 7d62                     jge        0x41dcb4
0041dc52 8b45f8                   mov        eax, dword ptr [ebp - 8]
0041dc55 8b08                     mov        ecx, dword ptr [eax]
0041dc57 6b55fc18                 imul       edx, dword ptr [ebp - 4], 0x18
0041dc5b 8b4158                   mov        eax, dword ptr [ecx + 0x58]
0041dc5e 8b4c1014                 mov        ecx, dword ptr [eax + edx + 0x14]
0041dc62 83e103                   and        ecx, 3
0041dc65 744b                     je         0x41dcb2
0041dc67 8b55f8                   mov        edx, dword ptr [ebp - 8]
0041dc6a 8b02                     mov        eax, dword ptr [edx]
0041dc6c 6b4dfc18                 imul       ecx, dword ptr [ebp - 4], 0x18
0041dc70 8b5058                   mov        edx, dword ptr [eax + 0x58]
0041dc73 833c0a00                 cmp        dword ptr [edx + ecx], 0
0041dc77 7439                     je         0x41dcb2
0041dc79 8b45f8                   mov        eax, dword ptr [ebp - 8]
0041dc7c 8b08                     mov        ecx, dword ptr [eax]
0041dc7e 6b55fc18                 imul       edx, dword ptr [ebp - 4], 0x18
0041dc82 8b4158                   mov        eax, dword ptr [ecx + 0x58]
0041dc85 8b0c10                   mov        ecx, dword ptr [eax + edx]
0041dc88 8b55f8                   mov        edx, dword ptr [ebp - 8]
0041dc8b 8b02                     mov        eax, dword ptr [edx]
0041dc8d 6b55fc18                 imul       edx, dword ptr [ebp - 4], 0x18
0041dc91 8b4058                   mov        eax, dword ptr [eax + 0x58]
0041dc94 8b1410                   mov        edx, dword ptr [eax + edx]
0041dc97 8b01                     mov        eax, dword ptr [ecx]
0041dc99 52                       push       edx
0041dc9a 8b4808                   mov        ecx, dword ptr [eax + 8]
0041dc9d ffd1                     call       ecx
0041dc9f 8b55f8                   mov        edx, dword ptr [ebp - 8]
0041dca2 8b02                     mov        eax, dword ptr [edx]
0041dca4 6b4dfc18                 imul       ecx, dword ptr [ebp - 4], 0x18
0041dca8 8b5058                   mov        edx, dword ptr [eax + 0x58]
0041dcab c7040a00000000           mov        dword ptr [edx + ecx], 0
0041dcb2 eb88                     jmp        0x41dc3c
0041dcb4 e953ffffff               jmp        0x41dc0c
0041dcb9 8be5                     mov        esp, ebp
0041dcbb 5d                       pop        ebp
0041dcbc c3                       ret        
0041dcbd cc                       int3       
0041dcbe cc                       int3       
0041dcbf cc                       int3       
