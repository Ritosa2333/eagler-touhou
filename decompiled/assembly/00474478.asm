
// block 00474478
00474478  mov        edi, edi
0047447a  push       ebp
0047447b  mov        ebp, esp
0047447d  push       ebx
0047447e  push       esi
0047447f  push       edi
00474480  mov        edi, dword ptr [ebp + 8]
00474483  push       0x90
00474488  xor        ebx, ebx
0047448a  push       ebx
0047448b  push       edi
0047448c  call       0x477420

// block 00474491
00474491  mov        esi, dword ptr [ebp + 0xc]
00474494  mov        al, byte ptr [esi]
00474496  add        esp, 0xc
00474499  test       al, al
0047449b  jne        0x4744a4

// block 0047449d
0047449d  xor        eax, eax
0047449f  jmp        0x47459c

// block 004744a4
004744a4  cmp        al, 0x2e
004744a6  jne        0x4744dc

// block 004744a8
004744a8  lea        eax, [esi + 1]
004744ab  cmp        byte ptr [eax], bl
004744ad  je         0x4744dc

// block 004744af
004744af  push       0xf
004744b1  push       eax
004744b2  lea        eax, [edi + 0x80]
004744b8  push       0x10
004744ba  push       eax
004744bb  call       0x4830fd

// block 004744c0
004744c0  add        esp, 0x10
004744c3  test       eax, eax
004744c5  je         0x4744d4

// block 004744c7
004744c7  push       ebx
004744c8  push       ebx
004744c9  push       ebx
004744ca  push       ebx
004744cb  push       ebx
004744cc  call       0x470dc6

// block 004744d1
004744d1  add        esp, 0x14

// block 004744d4
004744d4  mov        byte ptr [edi + 0x8f], bl
004744da  jmp        0x47449d

// block 004744dc
004744dc  push       0x49d4f8
004744e1  push       esi
004744e2  mov        dword ptr [ebp + 0xc], ebx
004744e5  call       0x4859c0

// block 004744ea
004744ea  cmp        eax, ebx
004744ec  jmp        0x474591

// block 004744f1
004744f1  cmp        dword ptr [ebp + 0xc], 0
004744f5  lea        edi, [eax + esi]
004744f8  mov        bl, byte ptr [edi]
004744fa  jne        0x474517

// block 004744fc
004744fc  cmp        eax, 0x40
004744ff  jae        0x474599

// block 00474505
00474505  cmp        bl, 0x2e
00474508  je         0x474599

// block 0047450e
0047450e  push       eax
0047450f  push       esi
00474510  push       0x40
00474512  push       dword ptr [ebp + 8]
00474515  jmp        0x474552

// block 00474517
00474517  cmp        dword ptr [ebp + 0xc], 1
0047451b  jne        0x474533

// block 0047451d
0047451d  cmp        eax, 0x40
00474520  jae        0x474599

// block 00474522
00474522  cmp        bl, 0x5f
00474525  je         0x474599

// block 00474527
00474527  push       eax
00474528  mov        eax, dword ptr [ebp + 8]
0047452b  push       esi
0047452c  push       0x40
0047452e  add        eax, 0x40
00474531  jmp        0x474551

// block 00474533
00474533  cmp        dword ptr [ebp + 0xc], 2
00474537  jne        0x474599

// block 00474539
00474539  cmp        eax, 0x10
0047453c  jae        0x474599

// block 0047453e
0047453e  test       bl, bl
00474540  je         0x474547

// block 00474542
00474542  cmp        bl, 0x2c
00474545  jne        0x474599

// block 00474547
00474547  push       eax
00474548  mov        eax, dword ptr [ebp + 8]
0047454b  push       esi
0047454c  push       0x10
0047454e  sub        eax, -0x80

// block 00474551
00474551  push       eax

// block 00474552
00474552  call       0x4830fd

// block 00474557
00474557  add        esp, 0x10
0047455a  test       eax, eax
0047455c  je         0x47456d

// block 0047455e
0047455e  xor        eax, eax
00474560  push       eax
00474561  push       eax
00474562  push       eax
00474563  push       eax
00474564  push       eax
00474565  call       0x470dc6

// block 0047456a
0047456a  add        esp, 0x14

// block 0047456d
0047456d  cmp        bl, 0x2c
00474570  je         0x47449d

// block 00474576
00474576  test       bl, bl
00474578  je         0x47449d

// block 0047457e
0047457e  inc        dword ptr [ebp + 0xc]
00474581  lea        esi, [edi + 1]
00474584  push       0x49d4f8
00474589  push       esi
0047458a  call       0x4859c0

// block 0047458f
0047458f  test       eax, eax

// block 00474591
00474591  pop        ecx
00474592  pop        ecx
00474593  jne        0x4744f1

// block 00474599
00474599  or         eax, 0xffffffff

// block 0047459c
0047459c  pop        edi
0047459d  pop        esi
0047459e  pop        ebx
0047459f  pop        ebp
004745a0  ret        
