
// block 00458960
00458960  push       esi
00458961  push       edi
00458962  mov        edi, eax
00458964  mov        esi, ecx
00458966  test       edi, edi
00458968  je         0x45898b

// block 0045896a
0045896a  lea        ebx, [ebx]

// block 00458970
00458970  cmp        word ptr [esi + 0x3ea], 0
00458978  jl         0x458980

// block 0045897a
0045897a  push       esi
0045897b  call       0x455630

// block 00458980
00458980  add        esi, 0x4b4
00458986  sub        edi, 1
00458989  jne        0x458970

// block 0045898b
0045898b  pop        edi
0045898c  pop        esi
0045898d  ret        
