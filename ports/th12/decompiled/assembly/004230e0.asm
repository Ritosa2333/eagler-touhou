
// block 004230e0
004230e0  lea        edx, [ecx*8]
004230e7  xor        edx, dword ptr [eax + 0xa0]
004230ed  and        edx, 8
004230f0  xor        dword ptr [eax + 0xa0], edx
004230f6  ret        
