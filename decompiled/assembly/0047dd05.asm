
// block 0047dd05
0047dd05  xor        eax, eax
0047dd07  cmp        dword ptr [ecx], eax
0047dd09  je         0x47dd15

// block 0047dd0b
0047dd0b  test       dword ptr [ecx + 4], 0x200
0047dd12  je         0x47dd15

// block 0047dd14
0047dd14  inc        eax

// block 0047dd15
0047dd15  ret        
