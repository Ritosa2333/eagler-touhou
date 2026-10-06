
// block 004822f0
004822f0  mov        edi, edi
004822f2  push       ebp
004822f3  mov        ebp, esp
004822f5  mov        eax, dword ptr [0x4b4318]
004822fa  mov        al, byte ptr [eax]
004822fc  sub        esp, 0x24
004822ff  push       ebx
00482300  push       esi
00482301  push       edi
00482302  test       al, al
00482304  je         0x48266c

// block 0048230a
0048230a  inc        dword ptr [0x4b4318]
00482310  and        dword ptr [ebp - 0xc], 0
00482314  movzx      edi, al
00482317  mov        esi, 0xffff0000
0048231c  and        dword ptr [ebp - 8], esi
0048231f  mov        eax, edi
00482321  or         ebx, 0xffffffff
00482324  cmp        eax, 0x4e
00482327  mov        byte ptr [ebp - 1], 0
0048232b  jg         0x48237f

// block 0048232d
0048232d  je         0x482564

// block 00482333
00482333  sub        eax, 0x43
00482336  cmp        eax, 0xa
00482339  ja         0x48254f

// block 0048233f
0048233f  movzx      eax, byte ptr [eax + 0x48269c]
00482346  jmp        dword ptr [eax*4 + 0x482684]

// block 0048234d
0048234d  push       0x49e12c
00482352  jmp        0x48253e

// block 00482357
00482357  push       0x49e124
0048235c  jmp        0x48253e

// block 00482361
00482361  push       0x49e120
00482366  jmp        0x48253e

// block 0048236b
0048236b  push       0x49e118
00482370  jmp        0x48253e

// block 00482375
00482375  push       0x49e110
0048237a  jmp        0x48253e

// block 0048237f
0048237f  cmp        eax, 0x4f
00482382  je         0x482557

// block 00482388
00482388  jle        0x48254f

// block 0048238e
0048238e  cmp        eax, 0x53
00482391  jle        0x482548

// block 00482397
00482397  cmp        eax, 0x58
0048239a  je         0x482539

// block 004823a0
004823a0  cmp        eax, 0x5f
004823a3  jne        0x48254f

// block 004823a9
004823a9  mov        eax, dword ptr [0x4b4318]
004823ae  mov        al, byte ptr [eax]
004823b0  inc        dword ptr [0x4b4318]
004823b6  mov        byte ptr [ebp - 1], al
004823b9  movzx      eax, al
004823bc  cmp        eax, 0x4b
004823bf  jg         0x48245b

// block 004823c5
004823c5  cmp        eax, 0x4a
004823c8  jge        0x482451

// block 004823ce
004823ce  cmp        eax, 0x45
004823d1  jg         0x482426

// block 004823d3
004823d3  cmp        eax, 0x44
004823d6  jge        0x48241c

// block 004823d8
004823d8  test       eax, eax
004823da  je         0x482407

// block 004823dc
004823dc  cmp        eax, 0x24
004823df  jne        0x482532

// block 004823e5
004823e5  push       dword ptr [ebp + 0xc]
004823e8  lea        eax, [ebp - 0x1c]
004823eb  push       eax
004823ec  call       0x4822f0

// block 004823f1
004823f1  push       eax
004823f2  push       0x49e108
004823f7  push       dword ptr [ebp + 8]
004823fa  call       0x47ec4a

// block 004823ff
004823ff  add        esp, 0x14
00482402  jmp        0x48267c

// block 00482407
00482407  dec        dword ptr [0x4b4318]
0048240d  push       1
0048240f  lea        ecx, [ebp - 0xc]
00482412  call       0x47e2f7

// block 00482417
00482417  jmp        0x48257a

// block 0048241c
0048241c  push       0x49e100
00482421  jmp        0x48253e

// block 00482426
00482426  cmp        eax, 0x46
00482429  jl         0x482532

// block 0048242f
0048242f  cmp        eax, 0x47
00482432  jle        0x482447

// block 00482434
00482434  cmp        eax, 0x49
00482437  jg         0x482532

// block 0048243d
0048243d  push       0x49e0f8
00482442  jmp        0x48253e

// block 00482447
00482447  push       0x49e0f0
0048244c  jmp        0x48253e

// block 00482451
00482451  push       0x49e0e8
00482456  jmp        0x48253e

// block 0048245b
0048245b  cmp        eax, 0x4c
0048245e  jl         0x482532

// block 00482464
00482464  cmp        eax, 0x4d
00482467  jle        0x48252b

// block 0048246d
0048246d  cmp        eax, 0x4e
00482470  je         0x482524

// block 00482476
00482476  cmp        eax, 0x4f
00482479  je         0x4824c3

// block 0048247b
0048247b  cmp        eax, 0x57
0048247e  je         0x4824bc

// block 00482480
00482480  add        eax, -0x58
00482483  cmp        eax, 1
00482486  ja         0x482532

// block 0048248c
0048248c  lea        eax, [ebp - 0x14]

// block 0048248f
0048248f  dec        dword ptr [0x4b4318]
00482495  push       eax
00482496  call       0x480514

// block 0048249b
0048249b  mov        edx, dword ptr [eax + 4]
0048249e  pop        ecx
0048249f  mov        ecx, dword ptr [eax]
004824a1  mov        dword ptr [ebp - 0xc], ecx
004824a4  mov        dword ptr [ebp - 8], edx
004824a7  test       ecx, ecx
004824a9  jne        0x48257a

// block 004824af
004824af  mov        eax, dword ptr [ebp + 8]
004824b2  mov        dword ptr [eax], ecx
004824b4  mov        dword ptr [eax + 4], edx
004824b7  jmp        0x48267f

// block 004824bc
004824bc  push       0x49e0e0
004824c1  jmp        0x48253e

// block 004824c3
004824c3  push       -2
004824c5  pop        ebx

// block 004824c6
004824c6  mov        eax, dword ptr [ebp + 0xc]
004824c9  mov        edx, dword ptr [eax]
004824cb  and        dword ptr [ebp - 8], esi
004824ce  xor        ecx, ecx
004824d0  mov        dword ptr [ebp - 0x14], edx
004824d3  mov        edx, dword ptr [eax + 4]
004824d6  mov        dword ptr [ebp - 0xc], ecx
004824d9  mov        dword ptr [ebp - 0x10], edx
004824dc  cmp        ebx, -2
004824df  jne        0x48261e

// block 004824e5
004824e5  push       ecx
004824e6  lea        eax, [ebp - 0x14]
004824e9  push       eax
004824ea  lea        eax, [ebp - 0xc]
004824ed  push       eax
004824ee  lea        eax, [ebp - 0x1c]
004824f1  mov        esi, 0x800
004824f6  or         dword ptr [ebp - 0x10], esi
004824f9  push       eax
004824fa  call       0x48206c

// block 004824ff
004824ff  add        esp, 0x10
00482502  test       dword ptr [ebp - 0x18], esi
00482505  jne        0x482514

// block 00482507
00482507  push       0x49db94
0048250c  lea        ecx, [ebp - 0x1c]
0048250f  call       0x47ea48

// block 00482514
00482514  mov        ecx, dword ptr [ebp - 0x1c]
00482517  mov        eax, dword ptr [ebp + 8]
0048251a  mov        dword ptr [eax], ecx
0048251c  mov        ecx, dword ptr [ebp - 0x18]
0048251f  jmp        0x482619

// block 00482524
00482524  push       0x49e0d8
00482529  jmp        0x48253e

// block 0048252b
0048252b  push       0x49e0cc
00482530  jmp        0x48253e

// block 00482532
00482532  push       0x49e0c4
00482537  jmp        0x48253e

// block 00482539
00482539  push       0x49de5c

// block 0048253e
0048253e  lea        ecx, [ebp - 0xc]
00482541  call       0x47e896

// block 00482546
00482546  jmp        0x48257a

// block 00482548
00482548  mov        ebx, edi
0048254a  and        ebx, 3
0048254d  jmp        0x482571

// block 0048254f
0048254f  lea        eax, [ebp - 0x24]
00482552  jmp        0x48248f

// block 00482557
00482557  push       0x49de3c
0048255c  lea        ecx, [ebp - 0xc]
0048255f  call       0x47e896

// block 00482564
00482564  push       0x49e0bc
00482569  lea        ecx, [ebp - 0xc]
0048256c  call       0x47ea48

// block 00482571
00482571  cmp        ebx, -1
00482574  jne        0x4824c6

// block 0048257a
0048257a  mov        eax, edi
0048257c  sub        eax, 0x43
0048257f  je         0x4825ce

// block 00482581
00482581  push       2
00482583  pop        ecx
00482584  sub        eax, ecx
00482586  je         0x4825c0

// block 00482588
00482588  sub        eax, ecx
0048258a  je         0x4825c0

// block 0048258c
0048258c  sub        eax, ecx
0048258e  je         0x4825c0

// block 00482590
00482590  sub        eax, ecx
00482592  je         0x4825c0

// block 00482594
00482594  sub        eax, 0x14
00482597  jne        0x4825ee

// block 00482599
00482599  movzx      eax, byte ptr [ebp - 1]
0048259d  sub        eax, 0x45
004825a0  je         0x4825b2

// block 004825a2
004825a2  sub        eax, ecx
004825a4  je         0x4825b2

// block 004825a6
004825a6  sub        eax, ecx
004825a8  je         0x4825b2

// block 004825aa
004825aa  sub        eax, ecx
004825ac  je         0x4825b2

// block 004825ae
004825ae  sub        eax, ecx
004825b0  jne        0x4825ee

// block 004825b2
004825b2  lea        eax, [ebp - 0xc]
004825b5  push       eax
004825b6  push       0x49de30
004825bb  lea        eax, [ebp - 0x24]
004825be  jmp        0x4825da

// block 004825c0
004825c0  lea        eax, [ebp - 0xc]
004825c3  push       eax
004825c4  push       0x49de30
004825c9  lea        eax, [ebp - 0x1c]
004825cc  jmp        0x4825da

// block 004825ce
004825ce  lea        eax, [ebp - 0xc]
004825d1  push       eax
004825d2  push       0x49e0b4
004825d7  lea        eax, [ebp - 0x14]

// block 004825da
004825da  push       eax
004825db  call       0x47ec4a

// block 004825e0
004825e0  mov        ecx, dword ptr [eax]
004825e2  mov        eax, dword ptr [eax + 4]
004825e5  add        esp, 0xc
004825e8  mov        dword ptr [ebp - 0xc], ecx
004825eb  mov        dword ptr [ebp - 8], eax

// block 004825ee
004825ee  mov        eax, dword ptr [ebp + 0xc]
004825f1  cmp        dword ptr [eax], 0
004825f4  je         0x48260e

// block 004825f6
004825f6  push       eax
004825f7  lea        eax, [ebp - 0x24]
004825fa  push       0x20
004825fc  push       eax
004825fd  call       0x47ec02

// block 00482602
00482602  add        esp, 0xc
00482605  push       eax
00482606  lea        ecx, [ebp - 0xc]
00482609  call       0x47e7bf

// block 0048260e
0048260e  mov        ecx, dword ptr [ebp - 0xc]
00482611  mov        eax, dword ptr [ebp + 8]
00482614  mov        dword ptr [eax], ecx
00482616  mov        ecx, dword ptr [ebp - 8]

// block 00482619
00482619  mov        dword ptr [eax + 4], ecx
0048261c  jmp        0x48267f

// block 0048261e
0048261e  cmp        dword ptr [eax], ecx
00482620  jne        0x48265a

// block 00482622
00482622  test       bl, 1
00482625  je         0x482648

// block 00482627
00482627  push       0x49e0ac
0048262c  lea        ecx, [ebp - 0xc]
0048262f  call       0x47e896

// block 00482634
00482634  test       bl, 2
00482637  je         0x48265a

// block 00482639
00482639  push       0x49e0a0
0048263e  lea        ecx, [ebp - 0xc]
00482641  call       0x47ea48

// block 00482646
00482646  jmp        0x48265a

// block 00482648
00482648  test       bl, 2
0048264b  je         0x48265a

// block 0048264d
0048264d  push       0x49e094
00482652  lea        ecx, [ebp - 0xc]
00482655  call       0x47e896

// block 0048265a
0048265a  lea        eax, [ebp - 0x14]
0048265d  push       eax
0048265e  lea        eax, [ebp - 0xc]
00482661  push       eax
00482662  push       dword ptr [ebp + 8]
00482665  call       0x482165

// block 0048266a
0048266a  jmp        0x482679

// block 0048266c
0048266c  push       dword ptr [ebp + 0xc]
0048266f  push       1
00482671  push       dword ptr [ebp + 8]
00482674  call       0x47ec26

// block 00482679
00482679  add        esp, 0xc

// block 0048267c
0048267c  mov        eax, dword ptr [ebp + 8]

// block 0048267f
0048267f  pop        edi
00482680  pop        esi
00482681  pop        ebx
00482682  leave      
00482683  ret        
