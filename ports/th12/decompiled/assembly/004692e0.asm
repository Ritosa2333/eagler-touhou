
// block 004692e0
004692e0  sub        esp, 8
004692e3  push       esi
004692e4  mov        esi, ecx
004692e6  mov        eax, dword ptr [esi + 4]
004692e9  mov        ecx, dword ptr [esp + 0x10]
004692ed  mov        dword ptr [esi + eax*4 + 0xc], ecx
004692f1  mov        edx, dword ptr [esi + 4]
004692f4  mov        ecx, dword ptr [esi + edx*4 + 0xc]
004692f8  cmp        dword ptr [ecx], 0x54504353
004692fe  lea        eax, [esi + edx*4 + 0xc]
00469302  je         0x469314

// block 00469304
00469304  mov        dword ptr [eax], 0
0046930a  or         eax, 0xffffffff
0046930d  pop        esi
0046930e  add        esp, 8
00469311  ret        4

// block 00469314
00469314  cmp        word ptr [ecx + 4], 1
00469319  jne        0x469304

// block 0046931b
0046931b  mov        eax, ecx
0046931d  movzx      ecx, word ptr [eax + 6]
00469321  push       ebx
00469322  mov        ebx, dword ptr [esi + 0x8c]
00469328  push       ebp
00469329  lea        ebp, [ecx + eax + 0x24]
0046932d  movzx      eax, word ptr [eax + 0x10]
00469331  add        dword ptr [esi + 8], eax
00469334  push       edi
00469335  lea        edi, [ebp + eax*4]
00469339  mov        eax, dword ptr [esi + 8]
0046933c  add        eax, eax
0046933e  add        eax, eax
00469340  add        eax, eax
00469342  push       eax
00469343  mov        dword ptr [esp + 0x18], ebp
00469347  call       0x46d04a

// block 0046934c
0046934c  mov        dword ptr [esi + 0x8c], eax
00469352  test       ebx, ebx
00469354  jne        0x4693a1

// block 00469356
00469356  xor        ecx, ecx
00469358  add        esp, 4
0046935b  cmp        dword ptr [esi + 8], ecx
0046935e  jle        0x4694b4

// block 00469364
00469364  mov        edx, dword ptr [esi + 4]
00469367  mov        eax, dword ptr [esi + edx*4 + 0xc]
0046936b  add        eax, dword ptr [ebp]
0046936e  mov        edx, dword ptr [esi + 0x8c]
00469374  mov        dword ptr [edx + ecx*8 + 4], eax
00469378  mov        eax, dword ptr [esi + 0x8c]
0046937e  mov        dword ptr [eax + ecx*8], edi
00469381  mov        eax, edi
00469383  lea        ebx, [eax + 1]

// block 00469386
00469386  mov        dl, byte ptr [eax]
00469388  inc        eax
00469389  test       dl, dl
0046938b  jne        0x469386

// block 0046938d
0046938d  sub        eax, ebx
0046938f  inc        ecx
00469390  add        ebp, 4
00469393  cmp        ecx, dword ptr [esi + 8]
00469396  lea        edi, [edi + eax + 1]
0046939a  jl         0x469364

// block 0046939c
0046939c  jmp        0x4694b4

// block 004693a1
004693a1  mov        ecx, dword ptr [esi + 4]
004693a4  mov        edx, dword ptr [esi + ecx*4 + 0xc]
004693a8  movzx      edx, word ptr [edx + 0x10]
004693ac  mov        ecx, dword ptr [esi + 8]
004693af  sub        ecx, edx
004693b1  mov        dword ptr [esp + 0x20], ecx
004693b5  add        ecx, ecx
004693b7  add        ecx, ecx
004693b9  add        ecx, ecx
004693bb  push       ecx
004693bc  push       ebx
004693bd  push       eax
004693be  call       0x47a330

// block 004693c3
004693c3  add        esp, 0x10
004693c6  test       ebx, ebx
004693c8  je         0x4693d3

// block 004693ca
004693ca  push       ebx
004693cb  call       0x46c941

// block 004693d0
004693d0  add        esp, 4

// block 004693d3
004693d3  mov        edx, dword ptr [esi + 4]
004693d6  mov        eax, dword ptr [esi + edx*4 + 0xc]
004693da  xor        ecx, ecx
004693dc  mov        dword ptr [esp + 0x10], 0
004693e4  cmp        cx, word ptr [eax + 0x10]
004693e8  jae        0x4694b4

// block 004693ee
004693ee  mov        edi, edi

// block 004693f0
004693f0  xor        ebx, ebx
004693f2  cmp        dword ptr [esp + 0x1c], ebx
004693f6  jle        0x46943c

// block 004693f8
004693f8  mov        ebp, dword ptr [esi + 0x8c]
004693fe  mov        edi, edi

// block 00469400
00469400  mov        ecx, dword ptr [ebp]
00469403  mov        eax, edi

// block 00469405
00469405  mov        dl, byte ptr [eax]
00469407  cmp        dl, byte ptr [ecx]
00469409  jne        0x469425

// block 0046940b
0046940b  test       dl, dl
0046940d  je         0x469421

// block 0046940f
0046940f  mov        dl, byte ptr [eax + 1]
00469412  cmp        dl, byte ptr [ecx + 1]
00469415  jne        0x469425

// block 00469417
00469417  add        eax, 2
0046941a  add        ecx, 2
0046941d  test       dl, dl
0046941f  jne        0x469405

// block 00469421
00469421  xor        eax, eax
00469423  jmp        0x46942a

// block 00469425
00469425  sbb        eax, eax
00469427  sbb        eax, -1

// block 0046942a
0046942a  test       eax, eax
0046942c  jle        0x469438

// block 0046942e
0046942e  inc        ebx
0046942f  add        ebp, 8
00469432  cmp        ebx, dword ptr [esp + 0x1c]
00469436  jl         0x469400

// block 00469438
00469438  mov        ebp, dword ptr [esp + 0x14]

// block 0046943c
0046943c  mov        ecx, dword ptr [esi + 8]
0046943f  dec        ecx
00469440  cmp        ecx, ebx
00469442  jle        0x46945d

// block 00469444
00469444  mov        edx, dword ptr [esi + 0x8c]
0046944a  lea        eax, [edx + ecx*8]
0046944d  mov        edx, dword ptr [eax - 8]
00469450  mov        dword ptr [eax], edx
00469452  mov        edx, dword ptr [eax - 4]
00469455  dec        ecx
00469456  cmp        ecx, ebx
00469458  mov        dword ptr [eax + 4], edx
0046945b  jg         0x469444

// block 0046945d
0046945d  mov        eax, dword ptr [esi + 4]
00469460  mov        ecx, dword ptr [esi + eax*4 + 0xc]
00469464  add        ecx, dword ptr [ebp]
00469467  mov        edx, dword ptr [esi + 0x8c]
0046946d  mov        dword ptr [edx + ebx*8 + 4], ecx
00469471  mov        eax, dword ptr [esi + 0x8c]
00469477  mov        dword ptr [eax + ebx*8], edi
0046947a  mov        eax, edi
0046947c  lea        ecx, [eax + 1]
0046947f  nop        

// block 00469480
00469480  mov        dl, byte ptr [eax]
00469482  inc        eax
00469483  test       dl, dl
00469485  jne        0x469480

// block 00469487
00469487  inc        dword ptr [esp + 0x1c]
0046948b  sub        eax, ecx
0046948d  mov        ecx, dword ptr [esi + 4]
00469490  mov        edx, dword ptr [esi + ecx*4 + 0xc]
00469494  movzx      ecx, word ptr [edx + 0x10]
00469498  lea        edi, [edi + eax + 1]
0046949c  mov        eax, dword ptr [esp + 0x10]
004694a0  inc        eax
004694a1  add        ebp, 4
004694a4  cmp        eax, ecx
004694a6  mov        dword ptr [esp + 0x10], eax
004694aa  mov        dword ptr [esp + 0x14], ebp
004694ae  jl         0x4693f0

// block 004694b4
004694b4  mov        edi, dword ptr [esi + 4]
004694b7  lea        edx, [edi + 1]
004694ba  mov        dword ptr [esi + 4], edx
004694bd  mov        eax, dword ptr [esi + edi*4 + 0xc]
004694c1  cmp        word ptr [eax + 6], 0
004694c6  je         0x4694d5

// block 004694c8
004694c8  mov        edx, dword ptr [esi]
004694ca  add        eax, 0x24
004694cd  push       eax
004694ce  mov        eax, dword ptr [edx + 4]
004694d1  mov        ecx, esi
004694d3  call       eax

// block 004694d5
004694d5  mov        eax, edi
004694d7  pop        edi
004694d8  pop        ebp
004694d9  pop        ebx
004694da  pop        esi
004694db  add        esp, 8
004694de  ret        4
