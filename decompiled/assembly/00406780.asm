
// block 00406780
00406780  fld        dword ptr [eax]
00406782  fld        dword ptr [esp + 4]
00406786  fld        st(0)
00406788  fmulp      st(2)
0040678a  fxch       st(1)
0040678c  fstp       dword ptr [eax]
0040678e  fld        dword ptr [eax + 4]
00406791  fmul       st(1)
00406793  fstp       dword ptr [eax + 4]
00406796  fmul       dword ptr [eax + 8]
00406799  fstp       dword ptr [eax + 8]
0040679c  ret        4
