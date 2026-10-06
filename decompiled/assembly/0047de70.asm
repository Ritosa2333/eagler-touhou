
// block 0047de70
0047de70  mov        eax, dword ptr [ecx + 4]
0047de73  test       eax, eax
0047de75  je         0x47de83

// block 0047de77
0047de77  mov        eax, dword ptr [eax]
0047de79  test       eax, eax
0047de7b  je         0x47de83

// block 0047de7d
0047de7d  mov        edx, dword ptr [eax]
0047de7f  mov        ecx, eax
0047de81  jmp        dword ptr [edx]

// block 0047de83
0047de83  xor        eax, eax
0047de85  ret        
