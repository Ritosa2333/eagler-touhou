
// block 004286f0
004286f0  mov        eax, dword ptr [0x4b44f4]
004286f5  mov        edx, dword ptr [esi]
004286f7  mov        ecx, dword ptr [eax + 0x18]
004286fa  mov        dword ptr [eax + 0x470], edx
00428700  mov        edx, dword ptr [esi + 4]
00428703  push       ebx
00428704  mov        dword ptr [eax + 0x474], edx
0042870a  mov        edx, dword ptr [esi + 8]
0042870d  xor        ebx, ebx
0042870f  push       ebp
00428710  mov        ebp, dword ptr [esp + 0x14]
00428714  mov        dword ptr [eax + 0x478], edx
0042871a  test       ecx, ecx
0042871c  je         0x428748

// block 0042871e
0042871e  push       edi
0042871f  nop        

// block 00428720
00428720  cmp        dword ptr [ecx + 0xc], 1
00428724  mov        edi, dword ptr [ecx + 8]
00428727  je         0x428741

// block 00428729
00428729  mov        edx, dword ptr [esp + 0x14]
0042872d  fld        dword ptr [esp + 0x10]
00428731  mov        eax, dword ptr [ecx]
00428733  mov        eax, dword ptr [eax + 0x1c]
00428736  push       ebp
00428737  push       edx
00428738  push       ecx
00428739  fstp       dword ptr [esp]
0042873c  push       esi
0042873d  call       eax

// block 0042873f
0042873f  add        ebx, eax

// block 00428741
00428741  mov        ecx, edi
00428743  test       edi, edi
00428745  jne        0x428720

// block 00428747
00428747  pop        edi

// block 00428748
00428748  pop        ebp
00428749  mov        eax, ebx
0042874b  pop        ebx
0042874c  ret        0xc
