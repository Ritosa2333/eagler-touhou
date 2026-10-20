
// block 00406300
00406300  fld        dword ptr [esp + 4]
00406304  mov        ecx, esi
00406306  fstp       dword ptr [esi]
00406308  fld        dword ptr [esp + 8]
0040630c  fstp       dword ptr [esi + 4]
0040630f  fld        dword ptr [esp + 0xc]
00406313  fstp       dword ptr [esi + 8]
00406316  fld        dword ptr [esp + 0x10]
0040631a  fstp       dword ptr [esi + 0xc]
0040631d  fld        dword ptr [esp + 0x14]
00406321  fstp       dword ptr [esi + 0x10]
00406324  fld        dword ptr [esp + 0x18]
00406328  fstp       dword ptr [esi + 0x14]
0040632b  call       0x406250

// block 00406330
00406330  mov        eax, esi
00406332  ret        0x18
