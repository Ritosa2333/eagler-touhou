
// block 0043c970
0043c970  mov        edx, dword ptr [ecx + 0x1518]
0043c976  sub        edx, ecx
0043c978  mov        eax, 0x2aaaaaab
0043c97d  imul       edx
0043c97f  mov        eax, edx
0043c981  shr        eax, 0x1f
0043c984  add        eax, edx
0043c986  mov        edx, dword ptr [ecx + 0x18a0]
0043c98c  lea        eax, [eax + eax*2]
0043c98f  lea        eax, [edx + eax*2]
0043c992  sub        eax, ecx
0043c994  sub        eax, 0x151c
0043c999  ret        
