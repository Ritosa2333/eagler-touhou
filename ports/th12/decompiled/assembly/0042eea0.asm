
// block 0042eea0
0042eea0  test       byte ptr [esi], 2
0042eea3  je         0x42eeed

// block 0042eea5
0042eea5  mov        eax, 0x4ce8e8
0042eeaa  call       0x431630

// block 0042eeaf
0042eeaf  mov        eax, dword ptr [0x4b43b8]
0042eeb4  mov        dword ptr [0x4cf468], 1
0042eebe  mov        ecx, dword ptr [eax + 0xc]
0042eec1  or         dword ptr [ecx + 4], 2
0042eec5  mov        ecx, dword ptr [eax + 0x10]
0042eec8  or         dword ptr [ecx + 4], 2
0042eecc  mov        eax, dword ptr [eax + 0x18fc0]
0042eed2  or         dword ptr [eax + 4], 2
0042eed6  and        dword ptr [0x4cee78], 0xffffdfff
0042eee0  mov        dword ptr [0x4cee40], 4
0042eeea  and        dword ptr [esi], 0xfffffffd

// block 0042eeed
0042eeed  mov        eax, 1
0042eef2  ret        
