
// block 00425940
00425940  push       ebx
00425941  push       ebp
00425942  mov        ebp, dword ptr [esp + 0xc]
00425946  push       esi
00425947  push       edi
00425948  push       0x24
0042594a  call       0x46c9ea

// block 0042594f
0042594f  xor        edi, edi
00425951  add        esp, 4
00425954  cmp        eax, edi
00425956  je         0x425974

// block 00425958
00425958  and        dword ptr [eax + 4], 0xfffffffe
0042595c  mov        dword ptr [eax + 8], edi
0042595f  mov        dword ptr [eax + 0xc], edi
00425962  mov        dword ptr [eax + 0x10], edi
00425965  mov        dword ptr [eax], edi
00425967  mov        dword ptr [eax + 0x14], eax
0042596a  mov        dword ptr [eax + 0x18], edi
0042596d  mov        dword ptr [eax + 0x1c], edi
00425970  mov        esi, eax
00425972  jmp        0x425976

// block 00425974
00425974  xor        esi, esi

// block 00425976
00425976  mov        eax, dword ptr [esi + 4]
00425979  and        eax, 0xfffffffd
0042597c  or         eax, 1
0042597f  mov        ebx, 0x16
00425984  mov        dword ptr [esi + 8], 0x427380
0042598b  mov        dword ptr [esi + 0xc], edi
0042598e  mov        dword ptr [esi + 0x10], edi
00425991  mov        dword ptr [esi + 0x20], ebp
00425994  mov        dword ptr [esi + 4], eax
00425997  call       0x462380

// block 0042599c
0042599c  push       0x24
0042599e  mov        dword ptr [ebp + 8], esi
004259a1  call       0x46c9ea

// block 004259a6
004259a6  add        esp, 4
004259a9  cmp        eax, edi
004259ab  je         0x4259c9

// block 004259ad
004259ad  and        dword ptr [eax + 4], 0xfffffffe
004259b1  mov        dword ptr [eax + 8], edi
004259b4  mov        dword ptr [eax + 0xc], edi
004259b7  mov        dword ptr [eax + 0x10], edi
004259ba  mov        dword ptr [eax], edi
004259bc  mov        dword ptr [eax + 0x14], eax
004259bf  mov        dword ptr [eax + 0x18], edi
004259c2  mov        dword ptr [eax + 0x1c], edi
004259c5  mov        esi, eax
004259c7  jmp        0x4259cb

// block 004259c9
004259c9  xor        esi, esi

// block 004259cb
004259cb  mov        ecx, dword ptr [esi + 4]
004259ce  and        ecx, 0xfffffffd
004259d1  or         ecx, 1
004259d4  mov        ebx, 0x1b
004259d9  mov        dword ptr [esi + 8], 0x4273c0
004259e0  mov        dword ptr [esi + 0xc], edi
004259e3  mov        dword ptr [esi + 0x10], edi
004259e6  mov        dword ptr [esi + 0x20], ebp
004259e9  mov        dword ptr [esi + 4], ecx
004259ec  call       0x462420

// block 004259f1
004259f1  pop        edi
004259f2  mov        dword ptr [ebp + 0xc], esi
004259f5  pop        esi
004259f6  pop        ebp
004259f7  xor        eax, eax
004259f9  pop        ebx
004259fa  ret        4
