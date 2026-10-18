005301c0 55                       push       ebp
005301c1 8bec                     mov        ebp, esp
005301c3 83ec30                   sub        esp, 0x30
005301c6 894dfc                   mov        dword ptr [ebp - 4], ecx
005301c9 8b4508                   mov        eax, dword ptr [ebp + 8]
005301cc 8945f8                   mov        dword ptr [ebp - 8], eax
005301cf 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005301d2 83e901                   sub        ecx, 1
005301d5 894df8                   mov        dword ptr [ebp - 8], ecx
005301d8 837df803                 cmp        dword ptr [ebp - 8], 3
005301dc 770a                     ja         0x5301e8
005301de 8b55f8                   mov        edx, dword ptr [ebp - 8]
005301e1 ff249540035300           jmp        dword ptr [edx*4 + 0x530340]
005301e8 6a00                     push       0
005301ea e84106f3ff               call       0x460830
005301ef 83c404                   add        esp, 4
005301f2 8bc8                     mov        ecx, eax
005301f4 e84761fdff               call       0x506340
005301f9 05d4000000               add        eax, 0xd4
005301fe 8945f4                   mov        dword ptr [ebp - 0xc], eax
00530201 8b45fc                   mov        eax, dword ptr [ebp - 4]
00530204 694804a0000000           imul       ecx, dword ptr [eax + 4], 0xa0
0053020b 894df0                   mov        dword ptr [ebp - 0x10], ecx
0053020e 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00530211 0345f0                   add        eax, dword ptr [ebp - 0x10]
00530214 ba50000000               mov        edx, 0x50
00530219 c1e200                   shl        edx, 0
0053021c 03c2                     add        eax, edx
0053021e b908000000               mov        ecx, 8
00530223 6bd100                   imul       edx, ecx, 0
00530226 03c2                     add        eax, edx
00530228 e90d010000               jmp        0x53033a
0053022d e908010000               jmp        0x53033a
00530232 6a00                     push       0
00530234 e8f705f3ff               call       0x460830
00530239 83c404                   add        esp, 4
0053023c 8bc8                     mov        ecx, eax
0053023e e8fd60fdff               call       0x506340
00530243 05d4000000               add        eax, 0xd4
00530248 8945ec                   mov        dword ptr [ebp - 0x14], eax
0053024b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053024e 694804a0000000           imul       ecx, dword ptr [eax + 4], 0xa0
00530255 894de8                   mov        dword ptr [ebp - 0x18], ecx
00530258 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0053025b 0355e8                   add        edx, dword ptr [ebp - 0x18]
0053025e b850000000               mov        eax, 0x50
00530263 c1e000                   shl        eax, 0
00530266 03d0                     add        edx, eax
00530268 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0053026b 8d04ca                   lea        eax, [edx + ecx*8]
0053026e e9c7000000               jmp        0x53033a
00530273 e9c2000000               jmp        0x53033a
00530278 6a00                     push       0
0053027a e8b105f3ff               call       0x460830
0053027f 83c404                   add        esp, 4
00530282 8bc8                     mov        ecx, eax
00530284 e8b760fdff               call       0x506340
00530289 05d4000000               add        eax, 0xd4
0053028e 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00530291 8b55fc                   mov        edx, dword ptr [ebp - 4]
00530294 694204a0000000           imul       eax, dword ptr [edx + 4], 0xa0
0053029b 8945e0                   mov        dword ptr [ebp - 0x20], eax
0053029e 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
005302a1 034de0                   add        ecx, dword ptr [ebp - 0x20]
005302a4 ba50000000               mov        edx, 0x50
005302a9 c1e200                   shl        edx, 0
005302ac 03ca                     add        ecx, edx
005302ae 8b450c                   mov        eax, dword ptr [ebp + 0xc]
005302b1 8d44c108                 lea        eax, [ecx + eax*8 + 8]
005302b5 e980000000               jmp        0x53033a
005302ba eb7e                     jmp        0x53033a
005302bc 6a00                     push       0
005302be e86d05f3ff               call       0x460830
005302c3 83c404                   add        esp, 4
005302c6 8bc8                     mov        ecx, eax
005302c8 e87360fdff               call       0x506340
005302cd 05d4000000               add        eax, 0xd4
005302d2 8945dc                   mov        dword ptr [ebp - 0x24], eax
005302d5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005302d8 695104a0000000           imul       edx, dword ptr [ecx + 4], 0xa0
005302df 8955d8                   mov        dword ptr [ebp - 0x28], edx
005302e2 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
005302e5 0345d8                   add        eax, dword ptr [ebp - 0x28]
005302e8 b950000000               mov        ecx, 0x50
005302ed c1e100                   shl        ecx, 0
005302f0 03c1                     add        eax, ecx
005302f2 8b550c                   mov        edx, dword ptr [ebp + 0xc]
005302f5 8d44d018                 lea        eax, [eax + edx*8 + 0x18]
005302f9 eb3f                     jmp        0x53033a
005302fb eb3d                     jmp        0x53033a
005302fd 6a00                     push       0
005302ff e82c05f3ff               call       0x460830
00530304 83c404                   add        esp, 4
00530307 8bc8                     mov        ecx, eax
00530309 e83260fdff               call       0x506340
0053030e 05d4000000               add        eax, 0xd4
00530313 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00530316 8b45fc                   mov        eax, dword ptr [ebp - 4]
00530319 694804a0000000           imul       ecx, dword ptr [eax + 4], 0xa0
00530320 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00530323 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00530326 0355d0                   add        edx, dword ptr [ebp - 0x30]
00530329 b850000000               mov        eax, 0x50
0053032e c1e000                   shl        eax, 0
00530331 03d0                     add        edx, eax
00530333 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00530336 8d44ca30                 lea        eax, [edx + ecx*8 + 0x30]
0053033a 8be5                     mov        esp, ebp
0053033c 5d                       pop        ebp
0053033d c20800                   ret        8
00530340 3202                     xor        al, byte ptr [edx]
00530342 53                       push       ebx
00530343 007802                   add        byte ptr [eax + 2], bh
00530346 53                       push       ebx
00530347 00bc025300fd02           add        byte ptr [edx + eax + 0x2fd0053], bh
0053034e 53                       push       ebx
