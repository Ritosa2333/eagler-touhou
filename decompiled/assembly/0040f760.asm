
// block 0040f760
0040f760  test       dword ptr [ecx + 0xc], eax
0040f763  jne        0x40f76d

// block 0040f765
0040f765  test       dword ptr [ecx + 8], eax
0040f768  jne        0x40f76d

// block 0040f76a
0040f76a  xor        eax, eax
0040f76c  ret        

// block 0040f76d
0040f76d  mov        eax, 1
0040f772  ret        
