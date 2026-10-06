
// block 00406460
00406460  fld        dword ptr [esp + 4]
00406464  fld1       
00406466  fdivrp     st(1)
00406468  fstp       dword ptr [esp + 4]
0040646c  fld        dword ptr [ecx]
0040646e  fld        dword ptr [esp + 4]
00406472  fld        st(0)
00406474  fmulp      st(2)
00406476  fxch       st(1)
00406478  fstp       dword ptr [eax]
0040647a  fld        dword ptr [ecx + 4]
0040647d  fmul       st(1)
0040647f  fstp       dword ptr [eax + 4]
00406482  fmul       dword ptr [ecx + 8]
00406485  fstp       dword ptr [eax + 8]
00406488  ret        4
