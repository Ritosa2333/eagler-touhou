005375d0 55                       push       ebp
005375d1 8bec                     mov        ebp, esp
005375d3 83ec24                   sub        esp, 0x24
005375d6 894ddc                   mov        dword ptr [ebp - 0x24], ecx
005375d9 6a00                     push       0
005375db e8a0caf2ff               call       0x464080
005375e0 83c404                   add        esp, 4
005375e3 8bc8                     mov        ecx, eax
005375e5 e8c654eeff               call       0x41cab0
005375ea 83f805                   cmp        eax, 5
005375ed 7518                     jne        0x537607
005375ef 6a00                     push       0
005375f1 e83accf2ff               call       0x464230
005375f6 83c404                   add        esp, 4
005375f9 8bc8                     mov        ecx, eax
005375fb e84082fcff               call       0x4ff840
00537600 0fb6c0                   movzx      eax, al
00537603 85c0                     test       eax, eax
00537605 754b                     jne        0x537652
00537607 c745f4a8695c00           mov        dword ptr [ebp - 0xc], 0x5c69a8
0053760e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00537611 e88a47edff               call       0x40bda0
00537616 8945fc                   mov        dword ptr [ebp - 4], eax
00537619 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053761c e84fa3eeff               call       0x421970
00537621 8945e8                   mov        dword ptr [ebp - 0x18], eax
00537624 eb09                     jmp        0x53762f
00537626 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00537629 83c104                   add        ecx, 4
0053762c 894dfc                   mov        dword ptr [ebp - 4], ecx
0053762f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00537632 3b55e8                   cmp        edx, dword ptr [ebp - 0x18]
00537635 741b                     je         0x537652
00537637 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053763a 8945f0                   mov        dword ptr [ebp - 0x10], eax
0053763d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00537640 e88b86f1ff               call       0x44fcd0
00537645 6a00                     push       0
00537647 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053764a e851a1edff               call       0x4117a0
0053764f 90                       nop        
00537650 ebd4                     jmp        0x537626
00537652 c745ecbc695c00           mov        dword ptr [ebp - 0x14], 0x5c69bc
00537659 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0053765c e83f47edff               call       0x40bda0
00537661 8945f8                   mov        dword ptr [ebp - 8], eax
00537664 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00537667 e804a3eeff               call       0x421970
0053766c 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0053766f eb09                     jmp        0x53767a
00537671 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00537674 83c104                   add        ecx, 4
00537677 894df8                   mov        dword ptr [ebp - 8], ecx
0053767a 8b55f8                   mov        edx, dword ptr [ebp - 8]
0053767d 3b55e4                   cmp        edx, dword ptr [ebp - 0x1c]
00537680 7411                     je         0x537693
00537682 8b45f8                   mov        eax, dword ptr [ebp - 8]
00537685 8945e0                   mov        dword ptr [ebp - 0x20], eax
00537688 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0053768b e870a8f8ff               call       0x4c1f00
00537690 90                       nop        
00537691 ebde                     jmp        0x537671
00537693 8be5                     mov        esp, ebp
00537695 5d                       pop        ebp
00537696 c3                       ret        
00537697 cc                       int3       
00537698 cc                       int3       
00537699 cc                       int3       
0053769a cc                       int3       
0053769b cc                       int3       
0053769c cc                       int3       
0053769d cc                       int3       
0053769e cc                       int3       
0053769f cc                       int3       
