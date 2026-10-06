
// block 00486204
00486204  movzx      eax, byte ptr [eax]
00486207  movzx      ecx, byte ptr [ecx]
0048620a  sub        eax, ecx
0048620c  je         0x48621b

// block 0048620e
0048620e  xor        ecx, ecx
00486210  test       eax, eax
00486212  setg       cl
00486215  lea        ecx, [ecx + ecx - 1]
00486219  mov        eax, ecx

// block 0048621b
0048621b  ret        
