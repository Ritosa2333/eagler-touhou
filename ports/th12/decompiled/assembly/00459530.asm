
// block 00459530
00459530  fldz       
00459532  push       esi
00459533  mov        esi, dword ptr [esp + 8]
00459537  mov        dword ptr [eax + 0x21c], esi
0045953d  movzx      esi, byte ptr [esp + 0xc]
00459542  mov        dword ptr [eax + 0x220], esi
00459548  mov        esi, dword ptr [edx]
0045954a  mov        dword ptr [eax + 0x1e8], esi
00459550  mov        edx, dword ptr [edx + 4]
00459553  mov        dword ptr [eax + 0x1ec], edx
00459559  mov        edx, dword ptr [ecx]
0045955b  mov        dword ptr [eax + 0x1f0], edx
00459561  mov        ecx, dword ptr [ecx + 4]
00459564  mov        dword ptr [eax + 0x1f4], ecx
0045956a  mov        ecx, dword ptr [eax + 0x218]
00459570  xor        edx, edx
00459572  pop        esi
00459573  test       cl, 1
00459576  jne        0x4595a1

// block 00459578
00459578  or         ecx, 1
0045957b  fst        dword ptr [eax + 0x210]
00459581  mov        dword ptr [eax + 0x20c], edx
00459587  mov        dword ptr [eax + 0x208], 0xfff0bdc1
00459591  mov        dword ptr [eax + 0x214], 0x4b2ed0
0045959b  mov        dword ptr [eax + 0x218], ecx

// block 004595a1
004595a1  fstp       dword ptr [eax + 0x210]
004595a7  mov        dword ptr [eax + 0x20c], edx
004595ad  mov        dword ptr [eax + 0x208], 0xffffffff
004595b7  ret        8
