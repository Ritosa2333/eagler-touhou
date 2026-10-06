
// block 0047e005
0047e005  xor        eax, eax
0047e007  cmp        byte ptr [ecx], al
0047e009  je         0x47e012

// block 0047e00b
0047e00b  inc        eax
0047e00c  inc        ecx
0047e00d  cmp        byte ptr [ecx], 0
0047e010  jne        0x47e00b

// block 0047e012
0047e012  ret        
