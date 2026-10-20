
// block 004101f0
004101f0  push       ecx
004101f1  mov        eax, dword ptr [ebx]
004101f3  test       eax, eax
004101f5  je         0x410269

// block 004101f7
004101f7  mov        edx, dword ptr [ebx + 0x10]
004101fa  push       esi
004101fb  xor        esi, esi
004101fd  dec        eax
004101fe  mov        dword ptr [esp + 4], esi
00410202  test       eax, eax
00410204  jle        0x410268

// block 00410206
00410206  mov        ecx, dword ptr [ebx + 4]
00410209  push       ebp
0041020a  push       edi
0041020b  jmp        0x410210

// block 00410210
00410210  mov        eax, dword ptr [ebx + 0xc]
00410213  mov        eax, dword ptr [eax + esi*4]
00410216  mov        eax, dword ptr [eax + 0x478]
0041021c  xor        ebp, ebp
0041021e  test       ecx, ecx
00410220  jle        0x41025a

// block 00410222
00410222  mov        edi, eax
00410224  mov        esi, edx
00410226  mov        ecx, 7

// block 0041022b
0041022b  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0041022d
0041022d  mov        ecx, dword ptr [ebx + 4]
00410230  lea        esi, [ecx*8]
00410237  sub        esi, ecx
00410239  add        eax, 0x1c
0041023c  lea        esi, [edx + esi*4]
0041023f  mov        edi, eax
00410241  mov        ecx, 7

// block 00410246
00410246  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00410248
00410248  mov        ecx, dword ptr [ebx + 4]
0041024b  inc        ebp
0041024c  add        eax, 0x1c
0041024f  add        edx, 0x1c
00410252  cmp        ebp, ecx
00410254  jl         0x410222

// block 00410256
00410256  mov        esi, dword ptr [esp + 0xc]

// block 0041025a
0041025a  mov        eax, dword ptr [ebx]
0041025c  inc        esi
0041025d  dec        eax
0041025e  cmp        esi, eax
00410260  mov        dword ptr [esp + 0xc], esi
00410264  jl         0x410210

// block 00410266
00410266  pop        edi
00410267  pop        ebp

// block 00410268
00410268  pop        esi

// block 00410269
00410269  pop        ecx
0041026a  ret        
