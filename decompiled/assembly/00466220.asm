
// block 00466220
00466220  push       ecx
00466221  test       ebx, ebx
00466223  je         0x46622b

// block 00466225
00466225  mov        dword ptr [ebx], 0

// block 0046622b
0046622b  mov        eax, dword ptr [esi]
0046622d  mov        edx, dword ptr [eax + 0x24]
00466230  lea        ecx, [esp]
00466233  push       ecx
00466234  push       esi
00466235  call       edx

// block 00466237
00466237  test       eax, eax
00466239  jl         0x46627b

// block 0046623b
0046623b  test       byte ptr [esp], 2
0046623f  je         0x466276

// block 00466241
00466241  push       edi
00466242  mov        edi, dword ptr [0x498084]

// block 00466248
00466248  mov        eax, dword ptr [esi]
0046624a  mov        ecx, dword ptr [eax + 0x50]
0046624d  push       esi
0046624e  call       ecx

// block 00466250
00466250  cmp        eax, 0x88780096
00466255  jne        0x46625b

// block 00466257
00466257  push       0xa
00466259  call       edi

// block 0046625b
0046625b  mov        edx, dword ptr [esi]
0046625d  mov        eax, dword ptr [edx + 0x50]
00466260  push       esi
00466261  call       eax

// block 00466263
00466263  test       eax, eax
00466265  jne        0x466248

// block 00466267
00466267  pop        edi
00466268  test       ebx, ebx
0046626a  je         0x466272

// block 0046626c
0046626c  mov        dword ptr [ebx], 1

// block 00466272
00466272  xor        eax, eax
00466274  pop        ecx
00466275  ret        

// block 00466276
00466276  mov        eax, 1

// block 0046627b
0046627b  pop        ecx
0046627c  ret        
