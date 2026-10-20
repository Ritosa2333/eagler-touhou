
// block 00406760
00406760  fld        dword ptr [ecx]
00406762  fadd       dword ptr [eax]
00406764  fstp       dword ptr [eax]
00406766  fld        dword ptr [ecx + 4]
00406769  fadd       dword ptr [eax + 4]
0040676c  fstp       dword ptr [eax + 4]
0040676f  fld        dword ptr [ecx + 8]
00406772  fadd       dword ptr [eax + 8]
00406775  fstp       dword ptr [eax + 8]
00406778  ret        
