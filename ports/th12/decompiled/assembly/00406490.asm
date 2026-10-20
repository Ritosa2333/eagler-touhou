
// block 00406490
00406490  push       ecx
00406491  fld        dword ptr [eax + 4]
00406494  fld        dword ptr [eax]
00406496  fld        dword ptr [eax + 8]
00406499  fld        st(1)
0040649b  fmulp      st(2)
0040649d  fld        st(2)
0040649f  fmulp      st(3)
004064a1  fxch       st(1)
004064a3  faddp      st(2)
004064a5  fmul       st(0), st(0)
004064a7  faddp      st(1)
004064a9  fstp       dword ptr [esp]
004064ac  fld        dword ptr [esp]
004064af  pop        ecx
004064b0  ret        
