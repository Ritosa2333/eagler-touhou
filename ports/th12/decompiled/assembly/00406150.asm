
// block 00406150
00406150  fld        dword ptr [ecx]
00406152  fld        dword ptr [esp + 4]
00406156  fld        st(0)
00406158  fmulp      st(2)
0040615a  fxch       st(1)
0040615c  fstp       dword ptr [eax]
0040615e  fld        dword ptr [ecx + 4]
00406161  fmul       st(1)
00406163  fstp       dword ptr [eax + 4]
00406166  fmul       dword ptr [ecx + 8]
00406169  fstp       dword ptr [eax + 8]
0040616c  ret        4
