
// block 0047dd23
0047dd23  xor        eax, eax
0047dd25  cmp        dword ptr [ecx], eax
0047dd27  je         0x47dd33

// block 0047dd29
0047dd29  test       dword ptr [ecx + 4], 0x400
0047dd30  je         0x47dd33

// block 0047dd32
0047dd32  inc        eax

// block 0047dd33
0047dd33  ret        
