
// block 00402410
00402410  mov        eax, ebx
00402412  imul       eax, eax, 0x118
00402418  push       edi
00402419  lea        edi, [eax + esi + 0x204]
00402420  mov        dword ptr [esi + 0x54c], edi
00402426  call       0x430a70

// block 0040242b
0040242b  mov        edx, dword ptr [esi + 0x54c]
00402431  mov        eax, dword ptr [esi + 8]
00402434  mov        ecx, dword ptr [eax]
00402436  add        edx, 0xcc
0040243c  push       edx
0040243d  push       eax
0040243e  mov        eax, dword ptr [ecx + 0xbc]
00402444  call       eax

// block 00402446
00402446  mov        dword ptr [esi + 0x550], ebx
0040244c  pop        edi
0040244d  ret        
