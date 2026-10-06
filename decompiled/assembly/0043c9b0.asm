
// block 0043c9b0
0043c9b0  mov        ecx, dword ptr [eax + 0x1518]
0043c9b6  mov        word ptr [ecx], dx
0043c9b9  mov        ecx, dword ptr [eax + 0x1518]
0043c9bf  mov        dx, word ptr [esp + 4]
0043c9c4  mov        word ptr [ecx + 2], dx
0043c9c8  mov        ecx, dword ptr [eax + 0x1518]
0043c9ce  mov        dx, word ptr [esp + 8]
0043c9d3  mov        word ptr [ecx + 4], dx
0043c9d7  add        dword ptr [eax + 0x1518], 6
0043c9de  mov        ecx, dword ptr [eax + 0x1518]
0043c9e4  sub        ecx, eax
0043c9e6  mov        eax, 0x2aaaaaab
0043c9eb  imul       ecx
0043c9ed  mov        eax, edx
0043c9ef  shr        eax, 0x1f
0043c9f2  add        eax, edx
0043c9f4  xor        ecx, ecx
0043c9f6  cmp        eax, 0x384
0043c9fb  setge      cl
0043c9fe  mov        eax, ecx
0043ca00  ret        8
