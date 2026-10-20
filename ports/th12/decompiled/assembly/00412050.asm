
// block 00412050
00412050  test       byte ptr [ecx + 0x14], 1
00412054  je         0x412088

// block 00412056
00412056  sub        dword ptr [ecx + 0xc], eax
00412059  mov        eax, dword ptr [ecx + 0xc]
0041205c  push       esi
0041205d  mov        esi, dword ptr [ecx + 0x10]
00412060  lea        edx, [esi*8]
00412067  sub        edx, esi
00412069  sub        eax, edx
0041206b  push       edi
0041206c  mov        edi, eax
0041206e  mov        eax, 0x92492493
00412073  imul       edi
00412075  add        edx, edi
00412077  sar        edx, 2
0041207a  mov        eax, edx
0041207c  shr        eax, 0x1f
0041207f  add        eax, edx
00412081  add        eax, esi
00412083  pop        edi
00412084  mov        dword ptr [ecx], eax
00412086  pop        esi
00412087  ret        

// block 00412088
00412088  sub        dword ptr [ecx], eax
0041208a  mov        eax, dword ptr [ecx]
0041208c  ret        
