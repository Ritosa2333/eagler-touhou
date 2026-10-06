
// block 00407700
00407700  mov        eax, dword ptr [ecx + 4]
00407703  cmp        eax, dword ptr [ecx]
00407705  je         0x407718

// block 00407707
00407707  cdq        
00407708  idiv       dword ptr [esp + 4]
0040770c  test       edx, edx
0040770e  jne        0x407718

// block 00407710
00407710  mov        eax, 1
00407715  ret        4

// block 00407718
00407718  xor        eax, eax
0040771a  ret        4
