
// block 0048621c
0048621c  mov        ax, word ptr [esi]
0048621f  cmp        ax, word ptr [ecx]
00486222  je         0x486259

// block 00486224
00486224  movzx      edx, byte ptr [ecx]
00486227  movzx      eax, al
0048622a  sub        eax, edx
0048622c  je         0x48623f

// block 0048622e
0048622e  xor        edx, edx
00486230  test       eax, eax
00486232  setg       dl
00486235  lea        edx, [edx + edx - 1]
00486239  mov        eax, edx
0048623b  test       eax, eax
0048623d  jne        0x48625b

// block 0048623f
0048623f  movzx      eax, byte ptr [esi + 1]
00486243  movzx      ecx, byte ptr [ecx + 1]
00486247  sub        eax, ecx
00486249  je         0x48625b

// block 0048624b
0048624b  xor        ecx, ecx
0048624d  test       eax, eax
0048624f  setg       cl
00486252  lea        ecx, [ecx + ecx - 1]
00486256  mov        eax, ecx
00486258  ret        

// block 00486259
00486259  xor        eax, eax

// block 0048625b
0048625b  ret        
