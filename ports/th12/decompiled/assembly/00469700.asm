
// block 00469700
00469700  mov        ecx, dword ptr [eax + 0x1000]
00469706  add        ecx, -4
00469709  mov        edx, dword ptr [eax + 0x1004]
0046970f  js         0x469720

// block 00469711
00469711  mov        dword ptr [eax + 0x1000], ecx
00469717  mov        ecx, dword ptr [ecx + eax]
0046971a  mov        dword ptr [eax + 0x1004], ecx

// block 00469720
00469720  mov        dword ptr [eax + 0x1000], edx
00469726  xor        eax, eax
00469728  ret        
