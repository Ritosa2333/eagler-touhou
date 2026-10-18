00462c10 55                       push       ebp
00462c11 8bec                     mov        ebp, esp
00462c13 83ec14                   sub        esp, 0x14
00462c16 894df8                   mov        dword ptr [ebp - 8], ecx
00462c19 8b45f8                   mov        eax, dword ptr [ebp - 8]
00462c1c 0528010000               add        eax, 0x128
00462c21 8945f4                   mov        dword ptr [ebp - 0xc], eax
00462c24 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00462c27 e87491faff               call       0x40bda0
00462c2c 8945fc                   mov        dword ptr [ebp - 4], eax
00462c2f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00462c32 e839edfbff               call       0x421970
00462c37 8945f0                   mov        dword ptr [ebp - 0x10], eax
00462c3a eb09                     jmp        0x462c45
00462c3c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00462c3f 83c104                   add        ecx, 4
00462c42 894dfc                   mov        dword ptr [ebp - 4], ecx
00462c45 8b55fc                   mov        edx, dword ptr [ebp - 4]
00462c48 3b55f0                   cmp        edx, dword ptr [ebp - 0x10]
00462c4b 7411                     je         0x462c5e
00462c4d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00462c50 8945ec                   mov        dword ptr [ebp - 0x14], eax
00462c53 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00462c56 e875d0feff               call       0x44fcd0
00462c5b 90                       nop        
00462c5c ebde                     jmp        0x462c3c
00462c5e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00462c61 81c138010000             add        ecx, 0x138
00462c67 e864d0feff               call       0x44fcd0
00462c6c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00462c6f 81c13c010000             add        ecx, 0x13c
00462c75 e856d0feff               call       0x44fcd0
00462c7a 33c0                     xor        eax, eax
00462c7c 8be5                     mov        esp, ebp
00462c7e 5d                       pop        ebp
00462c7f c3                       ret        
