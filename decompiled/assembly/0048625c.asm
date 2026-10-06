
// block 0048625c
0048625c  mov        eax, dword ptr [esi]
0048625e  cmp        eax, dword ptr [ecx]
00486260  je         0x4862d1

// block 00486262
00486262  movzx      edx, byte ptr [ecx]
00486265  movzx      eax, al
00486268  sub        eax, edx
0048626a  je         0x48627d

// block 0048626c
0048626c  xor        edx, edx
0048626e  test       eax, eax
00486270  setg       dl
00486273  lea        edx, [edx + edx - 1]
00486277  mov        eax, edx
00486279  test       eax, eax
0048627b  jne        0x4862d3

// block 0048627d
0048627d  movzx      eax, byte ptr [esi + 1]
00486281  movzx      edx, byte ptr [ecx + 1]
00486285  sub        eax, edx
00486287  je         0x48629a

// block 00486289
00486289  xor        edx, edx
0048628b  test       eax, eax
0048628d  setg       dl
00486290  lea        edx, [edx + edx - 1]
00486294  mov        eax, edx
00486296  test       eax, eax
00486298  jne        0x4862d3

// block 0048629a
0048629a  movzx      eax, byte ptr [esi + 2]
0048629e  movzx      edx, byte ptr [ecx + 2]
004862a2  sub        eax, edx
004862a4  je         0x4862b7

// block 004862a6
004862a6  xor        edx, edx
004862a8  test       eax, eax
004862aa  setg       dl
004862ad  lea        edx, [edx + edx - 1]
004862b1  mov        eax, edx
004862b3  test       eax, eax
004862b5  jne        0x4862d3

// block 004862b7
004862b7  movzx      eax, byte ptr [esi + 3]
004862bb  movzx      ecx, byte ptr [ecx + 3]
004862bf  sub        eax, ecx
004862c1  je         0x4862d3

// block 004862c3
004862c3  xor        ecx, ecx
004862c5  test       eax, eax
004862c7  setg       cl
004862ca  lea        ecx, [ecx + ecx - 1]
004862ce  mov        eax, ecx
004862d0  ret        

// block 004862d1
004862d1  xor        eax, eax

// block 004862d3
004862d3  ret        
