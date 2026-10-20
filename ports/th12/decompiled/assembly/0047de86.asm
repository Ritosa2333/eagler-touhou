
// block 0047de86
0047de86  mov        eax, dword ptr [ecx + 4]
0047de89  test       eax, eax
0047de8b  je         0x47de9a

// block 0047de8d
0047de8d  mov        eax, dword ptr [eax]
0047de8f  test       eax, eax
0047de91  je         0x47de9a

// block 0047de93
0047de93  mov        edx, dword ptr [eax]
0047de95  mov        ecx, eax
0047de97  jmp        dword ptr [edx + 4]

// block 0047de9a
0047de9a  xor        al, al
0047de9c  ret        
