00537530 55                       push       ebp
00537531 8bec                     mov        ebp, esp
00537533 83ec24                   sub        esp, 0x24
00537536 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00537539 c745f4a8695c00           mov        dword ptr [ebp - 0xc], 0x5c69a8
00537540 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00537543 e85848edff               call       0x40bda0
00537548 8945fc                   mov        dword ptr [ebp - 4], eax
0053754b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053754e e81da4eeff               call       0x421970
00537553 8945e8                   mov        dword ptr [ebp - 0x18], eax
00537556 eb09                     jmp        0x537561
00537558 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053755b 83c004                   add        eax, 4
0053755e 8945fc                   mov        dword ptr [ebp - 4], eax
00537561 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00537564 3b4de8                   cmp        ecx, dword ptr [ebp - 0x18]
00537567 741b                     je         0x537584
00537569 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053756c 8955f0                   mov        dword ptr [ebp - 0x10], edx
0053756f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00537572 e85987f1ff               call       0x44fcd0
00537577 6a00                     push       0
00537579 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053757c e81fa2edff               call       0x4117a0
00537581 90                       nop        
00537582 ebd4                     jmp        0x537558
00537584 c745ecbc695c00           mov        dword ptr [ebp - 0x14], 0x5c69bc
0053758b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0053758e e80d48edff               call       0x40bda0
00537593 8945f8                   mov        dword ptr [ebp - 8], eax
00537596 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00537599 e8d2a3eeff               call       0x421970
0053759e 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005375a1 eb09                     jmp        0x5375ac
005375a3 8b45f8                   mov        eax, dword ptr [ebp - 8]
005375a6 83c004                   add        eax, 4
005375a9 8945f8                   mov        dword ptr [ebp - 8], eax
005375ac 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005375af 3b4de4                   cmp        ecx, dword ptr [ebp - 0x1c]
005375b2 7411                     je         0x5375c5
005375b4 8b55f8                   mov        edx, dword ptr [ebp - 8]
005375b7 8955e0                   mov        dword ptr [ebp - 0x20], edx
005375ba 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005375bd e83ea9f8ff               call       0x4c1f00
005375c2 90                       nop        
005375c3 ebde                     jmp        0x5375a3
005375c5 8be5                     mov        esp, ebp
005375c7 5d                       pop        ebp
005375c8 c3                       ret        
005375c9 cc                       int3       
005375ca cc                       int3       
005375cb cc                       int3       
005375cc cc                       int3       
005375cd cc                       int3       
005375ce cc                       int3       
005375cf cc                       int3       
