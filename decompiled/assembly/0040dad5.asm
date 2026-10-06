
// block 0040dad5
0040dad5  push       edi
0040dad6  mov        edi, esi
0040dad8  call       0x40d880

// block 0040dadd
0040dadd  pop        edi
0040dade  test       eax, eax
0040dae0  je         0x40dafa

// block 0040dae2
0040dae2  test       esi, esi
0040dae4  je         0x40daf6

// block 0040dae6
0040dae6  mov        eax, esi
0040dae8  call       0x40d9c0

// block 0040daed
0040daed  push       esi
0040daee  call       0x46ca4f

// block 0040daf3
0040daf3  add        esp, 4

// block 0040daf6
0040daf6  xor        eax, eax
0040daf8  pop        esi
0040daf9  ret        

// block 0040dafa
0040dafa  mov        eax, esi
0040dafc  pop        esi
0040dafd  ret        
