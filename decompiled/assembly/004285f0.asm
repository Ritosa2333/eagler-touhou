
// block 004285f0
004285f0  mov        eax, dword ptr [0x4b44f4]
004285f5  mov        edx, dword ptr [edi]
004285f7  mov        ecx, dword ptr [eax + 0x18]
004285fa  mov        dword ptr [eax + 0x470], edx
00428600  mov        edx, dword ptr [edi + 4]
00428603  mov        dword ptr [eax + 0x474], edx
00428609  mov        edx, dword ptr [edi + 8]
0042860c  mov        dword ptr [eax + 0x478], edx
00428612  mov        edx, dword ptr [esi]
00428614  mov        dword ptr [eax + 0x47c], edx
0042861a  mov        edx, dword ptr [esi + 4]
0042861d  mov        dword ptr [eax + 0x480], edx
00428623  mov        edx, dword ptr [esi + 8]
00428626  push       ebx
00428627  xor        ebx, ebx
00428629  mov        dword ptr [eax + 0x484], edx
0042862f  test       ecx, ecx
00428631  je         0x42865c

// block 00428633
00428633  push       ebp

// block 00428634
00428634  cmp        dword ptr [ecx + 0xc], 1
00428638  mov        ebp, dword ptr [ecx + 8]
0042863b  je         0x428655

// block 0042863d
0042863d  cmp        byte ptr [ecx + 0x7d], 0
00428641  je         0x428655

// block 00428643
00428643  mov        edx, dword ptr [esp + 0xc]
00428647  mov        eax, dword ptr [ecx]
00428649  mov        eax, dword ptr [eax + 0x18]
0042864c  push       1
0042864e  push       edx
0042864f  push       esi
00428650  push       edi
00428651  call       eax

// block 00428653
00428653  add        ebx, eax

// block 00428655
00428655  mov        ecx, ebp
00428657  test       ebp, ebp
00428659  jne        0x428634

// block 0042865b
0042865b  pop        ebp

// block 0042865c
0042865c  mov        eax, ebx
0042865e  pop        ebx
0042865f  ret        4
