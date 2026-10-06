
// block 0040a220
0040a220  mov        eax, dword ptr [0x4b44e8]
0040a225  push       edi
0040a226  mov        edi, ecx
0040a228  test       eax, eax
0040a22a  je         0x40a232

// block 0040a22c
0040a22c  test       byte ptr [eax + 0x60], 4
0040a230  jne        0x40a245

// block 0040a232
0040a232  push       esi
0040a233  xor        esi, esi

// block 0040a235
0040a235  mov        ecx, esi
0040a237  mov        eax, edi
0040a239  call       0x40a150

// block 0040a23e
0040a23e  inc        esi
0040a23f  cmp        esi, 6
0040a242  jl         0x40a235

// block 0040a244
0040a244  pop        esi

// block 0040a245
0040a245  mov        eax, 1
0040a24a  pop        edi
0040a24b  ret        
