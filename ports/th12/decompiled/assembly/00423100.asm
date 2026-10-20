
// block 00423100
00423100  lea        edx, [ecx + ecx]
00423103  xor        edx, dword ptr [eax + 0xa0]
00423109  and        edx, 2
0042310c  xor        dword ptr [eax + 0xa0], edx
00423112  ret        
