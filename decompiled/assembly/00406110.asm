
// block 00406110
00406110  fld        dword ptr [eax]
00406112  fadd       dword ptr [ecx]
00406114  fstp       dword ptr [esi]
00406116  fld        dword ptr [eax + 4]
00406119  fadd       dword ptr [ecx + 4]
0040611c  fstp       dword ptr [esi + 4]
0040611f  fld        dword ptr [eax + 8]
00406122  fadd       dword ptr [ecx + 8]
00406125  fstp       dword ptr [esi + 8]
00406128  fld        dword ptr [eax + 0xc]
0040612b  fadd       dword ptr [ecx + 0xc]
0040612e  fstp       dword ptr [esi + 0xc]
00406131  fld        dword ptr [eax + 0x10]
00406134  fadd       dword ptr [ecx + 0x10]
00406137  fstp       dword ptr [esi + 0x10]
0040613a  fld        dword ptr [eax + 0x14]
0040613d  fadd       dword ptr [ecx + 0x14]
00406140  mov        ecx, esi
00406142  fstp       dword ptr [esi + 0x14]
00406145  call       0x406250

// block 0040614a
0040614a  mov        eax, esi
0040614c  ret        
