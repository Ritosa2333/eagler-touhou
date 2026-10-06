
// block 00462020
00462020  lea        eax, [esi + 0x10]
00462023  push       edi
00462024  test       eax, eax
00462026  je         0x46204d

// block 00462028
00462028  jmp        0x462030

// block 00462030
00462030  mov        ecx, dword ptr [eax]
00462032  movsx      edi, word ptr [ecx + 0x3ea]
00462039  cmp        edi, edx
0046203b  je         0x462051

// block 0046203d
0046203d  cmp        edx, -1
00462040  jne        0x462046

// block 00462042
00462042  cmp        ecx, esi
00462044  jne        0x462051

// block 00462046
00462046  mov        eax, dword ptr [eax + 4]
00462049  test       eax, eax
0046204b  jne        0x462030

// block 0046204d
0046204d  xor        eax, eax
0046204f  pop        edi
00462050  ret        

// block 00462051
00462051  mov        eax, ecx
00462053  pop        edi
00462054  ret        
