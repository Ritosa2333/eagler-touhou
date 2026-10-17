00536120 55                       push       ebp
00536121 8bec                     mov        ebp, esp
00536123 83ec24                   sub        esp, 0x24
00536126 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00536129 6a00                     push       0
0053612b e850dff2ff               call       0x464080
00536130 83c404                   add        esp, 4
00536133 8bc8                     mov        ecx, eax
00536135 e87669eeff               call       0x41cab0
0053613a 83f805                   cmp        eax, 5
0053613d 7518                     jne        0x536157
0053613f 6a00                     push       0
00536141 e8eae0f2ff               call       0x464230
00536146 83c404                   add        esp, 4
00536149 8bc8                     mov        ecx, eax
0053614b e8f096fcff               call       0x4ff840
00536150 0fb6c0                   movzx      eax, al
00536153 85c0                     test       eax, eax
00536155 754b                     jne        0x5361a2
00536157 c745f484695c00           mov        dword ptr [ebp - 0xc], 0x5c6984
0053615e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00536161 e83a5cedff               call       0x40bda0
00536166 8945fc                   mov        dword ptr [ebp - 4], eax
00536169 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053616c e8ffb7eeff               call       0x421970
00536171 8945e8                   mov        dword ptr [ebp - 0x18], eax
00536174 eb09                     jmp        0x53617f
00536176 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00536179 83c104                   add        ecx, 4
0053617c 894dfc                   mov        dword ptr [ebp - 4], ecx
0053617f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00536182 3b55e8                   cmp        edx, dword ptr [ebp - 0x18]
00536185 741b                     je         0x5361a2
00536187 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053618a 8945f0                   mov        dword ptr [ebp - 0x10], eax
0053618d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00536190 e83b9bf1ff               call       0x44fcd0
00536195 6a00                     push       0
00536197 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053619a e801b6edff               call       0x4117a0
0053619f 90                       nop        
005361a0 ebd4                     jmp        0x536176
005361a2 c745ec98695c00           mov        dword ptr [ebp - 0x14], 0x5c6998
005361a9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
005361ac e8ef5bedff               call       0x40bda0
005361b1 8945f8                   mov        dword ptr [ebp - 8], eax
005361b4 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
005361b7 e8b4b7eeff               call       0x421970
005361bc 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005361bf eb09                     jmp        0x5361ca
005361c1 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005361c4 83c104                   add        ecx, 4
005361c7 894df8                   mov        dword ptr [ebp - 8], ecx
005361ca 8b55f8                   mov        edx, dword ptr [ebp - 8]
005361cd 3b55e4                   cmp        edx, dword ptr [ebp - 0x1c]
005361d0 7411                     je         0x5361e3
005361d2 8b45f8                   mov        eax, dword ptr [ebp - 8]
005361d5 8945e0                   mov        dword ptr [ebp - 0x20], eax
005361d8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005361db e820bdf8ff               call       0x4c1f00
005361e0 90                       nop        
005361e1 ebde                     jmp        0x5361c1
005361e3 8be5                     mov        esp, ebp
005361e5 5d                       pop        ebp
005361e6 c3                       ret        
005361e7 cc                       int3       
005361e8 cc                       int3       
005361e9 cc                       int3       
005361ea cc                       int3       
005361eb cc                       int3       
005361ec cc                       int3       
005361ed cc                       int3       
005361ee cc                       int3       
005361ef cc                       int3       
