
// block 00402520
00402520  sub        esp, 0x10
00402523  mov        eax, dword ptr [esi + 0x438]
00402529  mov        ecx, dword ptr [esi + 0x20]
0040252c  push       ebx
0040252d  mov        ebx, dword ptr [esi + 0x430]
00402533  push       ebp
00402534  mov        ebp, dword ptr [esi + 0x434]
0040253a  push       edi
0040253b  push       0x4b4
00402540  xor        edi, edi
00402542  push       edi
00402543  push       esi
00402544  mov        dword ptr [esp + 0x24], eax
00402548  mov        dword ptr [esp + 0x18], ecx
0040254c  call       0x477420

// block 00402551
00402551  fld1       
00402553  mov        edx, dword ptr [esp + 0x18]
00402557  fst        dword ptr [esi + 0x40]
0040255a  mov        eax, dword ptr [esp + 0x24]
0040255e  fst        dword ptr [esi + 0x44]
00402561  fst        dword ptr [esi + 0x50]
00402564  mov        dword ptr [esi + 0x430], ebx
0040256a  fst        dword ptr [esi + 0x54]
0040256d  mov        dword ptr [esi + 0x434], ebp
00402573  fldz       
00402575  mov        dword ptr [esi + 0x20], edx
00402578  mov        dword ptr [esi + 0x438], eax
0040257e  mov        dword ptr [esi + 0x3bc], 0xffffffff
00402588  fst        dword ptr [esi + 0x334]
0040258e  fst        dword ptr [esi + 0x330]
00402594  mov        ecx, 7
00402599  fst        dword ptr [esi + 0x32c]
0040259f  add        esp, 0xc
004025a2  fst        dword ptr [esi + 0x328]
004025a8  fst        dword ptr [esi + 0x320]
004025ae  fst        dword ptr [esi + 0x31c]
004025b4  fst        dword ptr [esi + 0x318]
004025ba  fst        dword ptr [esi + 0x314]
004025c0  fst        dword ptr [esi + 0x30c]
004025c6  fst        dword ptr [esi + 0x308]
004025cc  fst        dword ptr [esi + 0x304]
004025d2  fst        dword ptr [esi + 0x300]
004025d8  fxch       st(1)
004025da  fst        dword ptr [esi + 0x338]
004025e0  fst        dword ptr [esi + 0x324]
004025e6  fst        dword ptr [esi + 0x310]
004025ec  fstp       dword ptr [esi + 0x2fc]
004025f2  mov        word ptr [esi + 0x47c], cx
004025f9  mov        dword ptr [esi + 0x6c], edi
004025fc  fstp       dword ptr [esi + 0x70]
004025ff  mov        dword ptr [esi + 0x68], 0xfff0bdc1
00402606  mov        dword ptr [esi + 0xe0], edi
0040260c  mov        dword ptr [esi + 0x12c], edi
00402612  mov        dword ptr [esi + 0x158], edi
00402618  mov        dword ptr [esi + 0x1a4], edi
0040261e  mov        dword ptr [esi + 0x1e0], edi
00402624  mov        dword ptr [esi + 0x21c], edi
0040262a  mov        dword ptr [esi + 0x268], edi
00402630  mov        dword ptr [esi + 0x294], edi
00402636  mov        dword ptr [esi + 0x2c0], edi
0040263c  mov        dword ptr [esi + 0x2ec], edi
00402642  mov        dword ptr [esi + 8], edi
00402645  mov        dword ptr [esi + 0xc], edi
00402648  mov        dword ptr [esi + 4], esi
0040264b  mov        dword ptr [esi + 0x14], edi
0040264e  mov        dword ptr [esi + 0x18], edi
00402651  pop        edi
00402652  pop        ebp
00402653  mov        dword ptr [esi + 0x10], esi
00402656  pop        ebx
00402657  add        esp, 0x10
0040265a  ret        
