
// block 00406610
00406610  fld        dword ptr [edx]
00406612  fsub       dword ptr [ecx]
00406614  fstp       dword ptr [eax]
00406616  fld        dword ptr [edx + 4]
00406619  fsub       dword ptr [ecx + 4]
0040661c  fstp       dword ptr [eax + 4]
0040661f  fld        dword ptr [edx + 8]
00406622  fsub       dword ptr [ecx + 8]
00406625  fstp       dword ptr [eax + 8]
00406628  ret        
