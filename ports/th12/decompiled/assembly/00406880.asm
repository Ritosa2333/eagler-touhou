
// block 00406880
00406880  push       ebx
00406881  push       ebp
00406882  mov        ebp, dword ptr [esp + 0xc]
00406886  push       esi
00406887  push       edi
00406888  push       0x24
0040688a  call       0x46c9ea

// block 0040688f
0040688f  xor        edi, edi
00406891  add        esp, 4
00406894  cmp        eax, edi
00406896  je         0x4068b4

// block 00406898
00406898  and        dword ptr [eax + 4], 0xfffffffe
0040689c  mov        dword ptr [eax + 8], edi
0040689f  mov        dword ptr [eax + 0xc], edi
004068a2  mov        dword ptr [eax + 0x10], edi
004068a5  mov        dword ptr [eax], edi
004068a7  mov        dword ptr [eax + 0x14], eax
004068aa  mov        dword ptr [eax + 0x18], edi
004068ad  mov        dword ptr [eax + 0x1c], edi
004068b0  mov        esi, eax
004068b2  jmp        0x4068b6

// block 004068b4
004068b4  xor        esi, esi

// block 004068b6
004068b6  or         dword ptr [esi + 4], 3
004068ba  mov        ebx, 0x11
004068bf  mov        dword ptr [esi + 8], 0x406bb0
004068c6  mov        dword ptr [esi + 0xc], edi
004068c9  mov        dword ptr [esi + 0x10], edi
004068cc  mov        dword ptr [esi + 0x20], ebp
004068cf  call       0x462380

// block 004068d4
004068d4  push       0x24
004068d6  mov        dword ptr [ebp + 8], esi
004068d9  call       0x46c9ea

// block 004068de
004068de  add        esp, 4
004068e1  cmp        eax, edi
004068e3  je         0x406901

// block 004068e5
004068e5  and        dword ptr [eax + 4], 0xfffffffe
004068e9  mov        dword ptr [eax + 8], edi
004068ec  mov        dword ptr [eax + 0xc], edi
004068ef  mov        dword ptr [eax + 0x10], edi
004068f2  mov        dword ptr [eax], edi
004068f4  mov        dword ptr [eax + 0x14], eax
004068f7  mov        dword ptr [eax + 0x18], edi
004068fa  mov        dword ptr [eax + 0x1c], edi
004068fd  mov        esi, eax
004068ff  jmp        0x406903

// block 00406901
00406901  xor        esi, esi

// block 00406903
00406903  or         dword ptr [esi + 4], 3
00406907  mov        ebx, 0x23
0040690c  mov        dword ptr [esi + 8], 0x406bc0
00406913  mov        dword ptr [esi + 0xc], edi
00406916  mov        dword ptr [esi + 0x10], edi
00406919  mov        dword ptr [esi + 0x20], ebp
0040691c  call       0x462420

// block 00406921
00406921  pop        edi
00406922  mov        dword ptr [ebp + 0xc], esi
00406925  pop        esi
00406926  pop        ebp
00406927  xor        eax, eax
00406929  pop        ebx
0040692a  ret        4
