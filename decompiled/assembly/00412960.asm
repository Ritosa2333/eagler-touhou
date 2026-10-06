
// block 00412960
00412960  mov        eax, dword ptr [0x4b44f4]
00412965  mov        eax, dword ptr [eax + 0x18]
00412968  test       ecx, ecx
0041296a  je         0x41297f

// block 0041296c
0041296c  test       eax, eax
0041296e  je         0x41297f

// block 00412970
00412970  cmp        dword ptr [eax + 0x80], ecx
00412976  je         0x412981

// block 00412978
00412978  mov        eax, dword ptr [eax + 8]
0041297b  test       eax, eax
0041297d  jne        0x412970

// block 0041297f
0041297f  xor        eax, eax

// block 00412981
00412981  ret        
