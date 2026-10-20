
// block 0047b508
0047b508  mov        ecx, dword ptr [esp + 4]
0047b50c  test       dword ptr [ecx + 4], 6
0047b513  mov        eax, 1
0047b518  je         0x47b54d

// block 0047b51a
0047b51a  mov        eax, dword ptr [esp + 8]
0047b51e  mov        ecx, dword ptr [eax + 8]
0047b521  xor        ecx, eax
0047b523  call       0x46c932

// block 0047b54d
0047b54d  ret        
