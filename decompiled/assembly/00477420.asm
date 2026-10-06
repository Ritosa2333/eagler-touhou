
// block 00477420
00477420  mov        edx, dword ptr [esp + 0xc]
00477424  mov        ecx, dword ptr [esp + 4]
00477428  test       edx, edx
0047742a  je         0x477495

// block 0047742c
0047742c  xor        eax, eax
0047742e  mov        al, byte ptr [esp + 8]
00477432  test       al, al
00477434  jne        0x47744c

// block 00477436
00477436  cmp        edx, 0x100
0047743c  jb         0x47744c

// block 0047743e
0047743e  cmp        dword ptr [0x4d52dc], 0
00477445  je         0x47744c

// block 00477447
00477447  jmp        0x48b39a

// block 0047744c
0047744c  push       edi
0047744d  mov        edi, ecx
0047744f  cmp        edx, 4
00477452  jb         0x477485

// block 00477454
00477454  neg        ecx
00477456  and        ecx, 3
00477459  je         0x477467

// block 0047745b
0047745b  sub        edx, ecx

// block 0047745d
0047745d  mov        byte ptr [edi], al
0047745f  add        edi, 1
00477462  sub        ecx, 1
00477465  jne        0x47745d

// block 00477467
00477467  mov        ecx, eax
00477469  shl        eax, 8
0047746c  add        eax, ecx
0047746e  mov        ecx, eax
00477470  shl        eax, 0x10
00477473  add        eax, ecx
00477475  mov        ecx, edx
00477477  and        edx, 3
0047747a  shr        ecx, 2
0047747d  je         0x477485

// block 0047747f
0047747f  rep stosd  dword ptr es:[edi], eax

// block 00477481
00477481  test       edx, edx
00477483  je         0x47748f

// block 00477485
00477485  mov        byte ptr [edi], al
00477487  add        edi, 1
0047748a  sub        edx, 1
0047748d  jne        0x477485

// block 0047748f
0047748f  mov        eax, dword ptr [esp + 8]
00477493  pop        edi
00477494  ret        

// block 00477495
00477495  mov        eax, dword ptr [esp + 4]
00477499  ret        
