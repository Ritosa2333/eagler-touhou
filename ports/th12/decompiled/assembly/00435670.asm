
// block 00435670
00435670  push       esi
00435671  mov        esi, 0x4d0e70

// block 00435676
00435676  mov        eax, dword ptr [esi]
00435678  test       eax, eax
0043567a  je         0x435695

// block 0043567c
0043567c  cmp        dword ptr [esi + 0x14], 0
00435680  je         0x435695

// block 00435682
00435682  mov        edx, dword ptr [esi + 8]
00435685  mov        edx, dword ptr [edx + 0xc]
00435688  mov        ecx, dword ptr [eax]
0043568a  push       edx
0043568b  push       0
0043568d  push       0
0043568f  push       eax
00435690  mov        eax, dword ptr [ecx + 0x30]
00435693  call       eax

// block 00435695
00435695  add        esi, 0x18
00435698  cmp        esi, 0x4d1410
0043569e  jl         0x435676

// block 004356a0
004356a0  pop        esi
004356a1  ret        
