
// block 00476077
00476077  mov        edi, edi
00476079  push       ebp
0047607a  mov        ebp, esp
0047607c  sub        esp, 0x7c
0047607f  mov        eax, dword ptr [0x4ad138]
00476084  xor        eax, ebp
00476086  mov        dword ptr [ebp - 4], eax
00476089  mov        eax, dword ptr [ebp + 8]
0047608c  push       ebx
0047608d  xor        ebx, ebx
0047608f  push       esi
00476090  xor        esi, esi
00476092  mov        dword ptr [ebp - 0x78], eax
00476095  mov        eax, dword ptr [ebp + 0xc]
00476098  inc        esi
00476099  xor        ecx, ecx
0047609b  push       edi
0047609c  mov        dword ptr [ebp - 0x70], eax
0047609f  lea        edi, [ebp - 0x20]
004760a2  mov        dword ptr [ebp - 0x74], ebx
004760a5  mov        dword ptr [ebp - 0x68], esi
004760a8  mov        dword ptr [ebp - 0x4c], ebx
004760ab  mov        dword ptr [ebp - 0x58], ebx
004760ae  mov        dword ptr [ebp - 0x5c], ebx
004760b1  mov        dword ptr [ebp - 0x60], ebx
004760b4  mov        dword ptr [ebp - 0x64], ebx
004760b7  mov        dword ptr [ebp - 0x50], ebx
004760ba  mov        dword ptr [ebp - 0x6c], ebx
004760bd  cmp        dword ptr [ebp + 0x24], ebx
004760c0  jne        0x4760e1

// block 004760c2
004760c2  call       0x46e96f

// block 004760c7
004760c7  push       ebx
004760c8  push       ebx
004760c9  push       ebx
004760ca  push       ebx
004760cb  push       ebx
004760cc  mov        dword ptr [eax], 0x16
004760d2  call       0x470f2d

// block 004760d7
004760d7  add        esp, 0x14
004760da  xor        eax, eax
004760dc  jmp        0x47672f

// block 004760e1
004760e1  mov        edx, dword ptr [ebp + 0x10]
004760e4  mov        dword ptr [ebp - 0x54], edx

// block 004760e7
004760e7  mov        al, byte ptr [edx]
004760e9  cmp        al, 0x20
004760eb  je         0x4760f9

// block 004760ed
004760ed  cmp        al, 9
004760ef  je         0x4760f9

// block 004760f1
004760f1  cmp        al, 0xa
004760f3  je         0x4760f9

// block 004760f5
004760f5  cmp        al, 0xd
004760f7  jne        0x4760fc

// block 004760f9
004760f9  inc        edx
004760fa  jmp        0x4760e7

// block 004760fc
004760fc  mov        bl, 0x30

// block 004760fe
004760fe  mov        al, byte ptr [edx]
00476100  inc        edx
00476101  cmp        ecx, 0xb
00476104  ja         0x476339

// block 0047610a
0047610a  jmp        dword ptr [ecx*4 + 0x47673f]

// block 00476111
00476111  mov        cl, al
00476113  sub        cl, 0x31
00476116  cmp        cl, 8
00476119  ja         0x476121

// block 0047611b
0047611b  push       3

// block 0047611d
0047611d  pop        ecx
0047611e  dec        edx
0047611f  jmp        0x4760fe

// block 00476121
00476121  mov        ecx, dword ptr [ebp + 0x24]
00476124  mov        ecx, dword ptr [ecx]
00476126  mov        ecx, dword ptr [ecx + 0xbc]
0047612c  mov        ecx, dword ptr [ecx]
0047612e  cmp        al, byte ptr [ecx]
00476130  jne        0x476137

// block 00476132
00476132  push       5

// block 00476134
00476134  pop        ecx
00476135  jmp        0x4760fe

// block 00476137
00476137  movsx      eax, al
0047613a  sub        eax, 0x2b
0047613d  je         0x47615c

// block 0047613f
0047613f  dec        eax
00476140  dec        eax
00476141  je         0x476150

// block 00476143
00476143  sub        eax, 3
00476146  jne        0x4762d7

// block 0047614c
0047614c  mov        ecx, esi
0047614e  jmp        0x4760fe

// block 00476150
00476150  push       2
00476152  pop        ecx
00476153  mov        dword ptr [ebp - 0x74], 0x8000
0047615a  jmp        0x4760fe

// block 0047615c
0047615c  and        dword ptr [ebp - 0x74], 0
00476160  push       2
00476162  pop        ecx
00476163  jmp        0x4760fe

// block 00476165
00476165  mov        cl, al
00476167  sub        cl, 0x31
0047616a  mov        dword ptr [ebp - 0x58], esi
0047616d  cmp        cl, 8
00476170  jbe        0x47611b

// block 00476172
00476172  mov        ecx, dword ptr [ebp + 0x24]
00476175  mov        ecx, dword ptr [ecx]
00476177  mov        ecx, dword ptr [ecx + 0xbc]
0047617d  mov        ecx, dword ptr [ecx]
0047617f  cmp        al, byte ptr [ecx]
00476181  jne        0x476187

// block 00476183
00476183  push       4
00476185  jmp        0x476134

// block 00476187
00476187  cmp        al, 0x2b
00476189  je         0x4761b3

// block 0047618b
0047618b  cmp        al, 0x2d
0047618d  je         0x4761b3

// block 0047618f
0047618f  cmp        al, bl
00476191  je         0x47614c

// block 00476193
00476193  cmp        al, 0x43
00476195  jle        0x4762d7

// block 0047619b
0047619b  cmp        al, 0x45
0047619d  jle        0x4761af

// block 0047619f
0047619f  cmp        al, 0x63
004761a1  jle        0x4762d7

// block 004761a7
004761a7  cmp        al, 0x65
004761a9  jg         0x4762d7

// block 004761af
004761af  push       6
004761b1  jmp        0x476134

// block 004761b3
004761b3  dec        edx
004761b4  push       0xb
004761b6  jmp        0x476134

// block 004761bb
004761bb  mov        cl, al
004761bd  sub        cl, 0x31
004761c0  cmp        cl, 8
004761c3  jbe        0x47611b

// block 004761c9
004761c9  mov        ecx, dword ptr [ebp + 0x24]
004761cc  mov        ecx, dword ptr [ecx]
004761ce  mov        ecx, dword ptr [ecx + 0xbc]
004761d4  mov        ecx, dword ptr [ecx]
004761d6  cmp        al, byte ptr [ecx]
004761d8  je         0x476132

// block 004761de
004761de  cmp        al, bl
004761e0  je         0x47614c

// block 004761e6
004761e6  mov        edx, dword ptr [ebp - 0x54]
004761e9  jmp        0x476302

// block 004761ee
004761ee  mov        dword ptr [ebp - 0x58], esi
004761f1  jmp        0x47620d

// block 004761f3
004761f3  cmp        al, 0x39
004761f5  jg         0x476211

// block 004761f7
004761f7  cmp        dword ptr [ebp - 0x4c], 0x19
004761fb  jae        0x476207

// block 004761fd
004761fd  inc        dword ptr [ebp - 0x4c]
00476200  sub        al, bl
00476202  mov        byte ptr [edi], al
00476204  inc        edi
00476205  jmp        0x47620a

// block 00476207
00476207  inc        dword ptr [ebp - 0x50]

// block 0047620a
0047620a  mov        al, byte ptr [edx]
0047620c  inc        edx

// block 0047620d
0047620d  cmp        al, bl
0047620f  jge        0x4761f3

// block 00476211
00476211  mov        ecx, dword ptr [ebp + 0x24]
00476214  mov        ecx, dword ptr [ecx]
00476216  mov        ecx, dword ptr [ecx + 0xbc]
0047621c  mov        ecx, dword ptr [ecx]
0047621e  cmp        al, byte ptr [ecx]
00476220  je         0x476183

// block 00476226
00476226  cmp        al, 0x2b
00476228  je         0x4761b3

// block 0047622a
0047622a  cmp        al, 0x2d
0047622c  je         0x4761b3

// block 0047622e
0047622e  jmp        0x476193

// block 00476233
00476233  cmp        dword ptr [ebp - 0x4c], 0
00476237  mov        dword ptr [ebp - 0x58], esi
0047623a  mov        dword ptr [ebp - 0x5c], esi
0047623d  jne        0x476265

// block 0047623f
0047623f  jmp        0x476247

// block 00476241
00476241  dec        dword ptr [ebp - 0x50]
00476244  mov        al, byte ptr [edx]
00476246  inc        edx

// block 00476247
00476247  cmp        al, bl
00476249  je         0x476241

// block 0047624b
0047624b  jmp        0x476265

// block 0047624d
0047624d  cmp        al, 0x39
0047624f  jg         0x476226

// block 00476251
00476251  cmp        dword ptr [ebp - 0x4c], 0x19
00476255  jae        0x476262

// block 00476257
00476257  inc        dword ptr [ebp - 0x4c]
0047625a  sub        al, bl
0047625c  mov        byte ptr [edi], al
0047625e  inc        edi
0047625f  dec        dword ptr [ebp - 0x50]

// block 00476262
00476262  mov        al, byte ptr [edx]
00476264  inc        edx

// block 00476265
00476265  cmp        al, bl
00476267  jge        0x47624d

// block 00476269
00476269  jmp        0x476226

// block 0047626b
0047626b  sub        al, bl
0047626d  mov        dword ptr [ebp - 0x5c], esi
00476270  cmp        al, 9
00476272  ja         0x4761e6

// block 00476278
00476278  push       4
0047627a  jmp        0x47611d

// block 0047627f
0047627f  lea        ecx, [edx - 2]
00476282  mov        dword ptr [ebp - 0x54], ecx
00476285  mov        cl, al
00476287  sub        cl, 0x31
0047628a  cmp        cl, 8
0047628d  ja         0x476296

// block 0047628f
0047628f  push       9
00476291  jmp        0x47611d

// block 00476296
00476296  movsx      eax, al
00476299  sub        eax, 0x2b
0047629c  je         0x4762be

// block 0047629e
0047629e  dec        eax
0047629f  dec        eax
004762a0  je         0x4762b2

// block 004762a2
004762a2  sub        eax, 3

// block 004762a5
004762a5  jne        0x4761e6

// block 004762ab
004762ab  push       8
004762ad  jmp        0x476134

// block 004762b2
004762b2  or         dword ptr [ebp - 0x68], 0xffffffff
004762b6  push       7
004762b8  pop        ecx
004762b9  jmp        0x4760fe

// block 004762be
004762be  push       7
004762c0  jmp        0x476134

// block 004762c5
004762c5  mov        dword ptr [ebp - 0x60], esi
004762c8  jmp        0x4762cd

// block 004762ca
004762ca  mov        al, byte ptr [edx]
004762cc  inc        edx

// block 004762cd
004762cd  cmp        al, bl
004762cf  je         0x4762ca

// block 004762d1
004762d1  sub        al, 0x31
004762d3  cmp        al, 8
004762d5  jbe        0x47628f

// block 004762d7
004762d7  dec        edx
004762d8  jmp        0x476302

// block 004762da
004762da  mov        cl, al
004762dc  sub        cl, 0x31
004762df  cmp        cl, 8
004762e2  jbe        0x47628f

// block 004762e4
004762e4  cmp        al, bl
004762e6  jmp        0x4762a5

// block 004762e8
004762e8  cmp        dword ptr [ebp + 0x20], 0
004762ec  je         0x476335

// block 004762ee
004762ee  movsx      eax, al
004762f1  sub        eax, 0x2b
004762f4  lea        ecx, [edx - 1]
004762f7  mov        dword ptr [ebp - 0x54], ecx
004762fa  je         0x4762be

// block 004762fc
004762fc  dec        eax
004762fd  dec        eax
004762fe  je         0x4762b2

// block 00476300
00476300  mov        edx, ecx

// block 00476302
00476302  cmp        dword ptr [ebp - 0x58], 0
00476306  mov        eax, dword ptr [ebp - 0x70]
00476309  mov        dword ptr [eax], edx
0047630b  je         0x4766ea

// block 00476311
00476311  push       0x18
00476313  pop        eax
00476314  cmp        dword ptr [ebp - 0x4c], eax
00476317  jbe        0x476329

// block 00476319
00476319  cmp        byte ptr [ebp - 9], 5
0047631d  jl         0x476322

// block 0047631f
0047631f  inc        byte ptr [ebp - 9]

// block 00476322
00476322  dec        edi
00476323  inc        dword ptr [ebp - 0x50]
00476326  mov        dword ptr [ebp - 0x4c], eax

// block 00476329
00476329  cmp        dword ptr [ebp - 0x4c], 0
0047632d  jbe        0x476711

// block 00476333
00476333  jmp        0x47638e

// block 00476335
00476335  push       0xa
00476337  pop        ecx
00476338  dec        edx

// block 00476339
00476339  cmp        ecx, 0xa
0047633c  jne        0x4760fe

// block 00476342
00476342  jmp        0x476302

// block 00476344
00476344  mov        dword ptr [ebp - 0x60], esi
00476347  xor        ecx, ecx
00476349  jmp        0x476364

// block 0047634b
0047634b  cmp        al, 0x39
0047634d  jg         0x47636f

// block 0047634f
0047634f  imul       ecx, ecx, 0xa
00476352  movsx      esi, al
00476355  lea        ecx, [ecx + esi - 0x30]
00476359  cmp        ecx, 0x1450
0047635f  jg         0x47636a

// block 00476361
00476361  mov        al, byte ptr [edx]
00476363  inc        edx

// block 00476364
00476364  cmp        al, bl
00476366  jge        0x47634b

// block 00476368
00476368  jmp        0x47636f

// block 0047636a
0047636a  mov        ecx, 0x1451

// block 0047636f
0047636f  mov        dword ptr [ebp - 0x64], ecx
00476372  jmp        0x47637f

// block 00476374
00476374  cmp        al, 0x39
00476376  jg         0x4762d7

// block 0047637c
0047637c  mov        al, byte ptr [edx]
0047637e  inc        edx

// block 0047637f
0047637f  cmp        al, bl
00476381  jge        0x476374

// block 00476383
00476383  jmp        0x4762d7

// block 00476388
00476388  dec        dword ptr [ebp - 0x4c]
0047638b  inc        dword ptr [ebp - 0x50]

// block 0047638e
0047638e  dec        edi
0047638f  cmp        byte ptr [edi], 0
00476392  je         0x476388

// block 00476394
00476394  lea        eax, [ebp - 0x3c]
00476397  push       eax
00476398  push       dword ptr [ebp - 0x4c]
0047639b  lea        eax, [ebp - 0x20]
0047639e  push       eax
0047639f  call       0x48ad74

// block 004763a4
004763a4  mov        eax, dword ptr [ebp - 0x64]
004763a7  xor        edx, edx
004763a9  add        esp, 0xc
004763ac  cmp        dword ptr [ebp - 0x68], edx
004763af  jge        0x4763b3

// block 004763b1
004763b1  neg        eax

// block 004763b3
004763b3  add        eax, dword ptr [ebp - 0x50]
004763b6  cmp        dword ptr [ebp - 0x60], edx
004763b9  jne        0x4763be

// block 004763bb
004763bb  add        eax, dword ptr [ebp + 0x18]

// block 004763be
004763be  cmp        dword ptr [ebp - 0x5c], edx
004763c1  jne        0x4763c6

// block 004763c3
004763c3  sub        eax, dword ptr [ebp + 0x1c]

// block 004763c6
004763c6  cmp        eax, 0x1450
004763cb  jg         0x4766f3

// block 004763d1
004763d1  cmp        eax, 0xffffebb0
004763d6  jl         0x47670a

// block 004763dc
004763dc  mov        ecx, 0x4adfe0
004763e1  sub        ecx, 0x60
004763e4  mov        dword ptr [ebp - 0x54], eax
004763e7  cmp        eax, edx
004763e9  je         0x4766d8

// block 004763ef
004763ef  jge        0x4763fe

// block 004763f1
004763f1  neg        eax
004763f3  mov        ecx, 0x4ae140
004763f8  mov        dword ptr [ebp - 0x54], eax
004763fb  sub        ecx, 0x60

// block 004763fe
004763fe  cmp        dword ptr [ebp + 0x14], edx
00476401  jne        0x476409

// block 00476403
00476403  xor        eax, eax
00476405  mov        word ptr [ebp - 0x3c], ax

// block 00476409
00476409  cmp        dword ptr [ebp - 0x54], edx
0047640c  je         0x4766d8

// block 00476412
00476412  jmp        0x476419

// block 00476414
00476414  mov        ecx, dword ptr [ebp - 0x7c]
00476417  xor        edx, edx

// block 00476419
00476419  mov        eax, dword ptr [ebp - 0x54]
0047641c  sar        dword ptr [ebp - 0x54], 3
00476420  add        ecx, 0x54
00476423  and        eax, 7
00476426  mov        dword ptr [ebp - 0x7c], ecx
00476429  cmp        eax, edx
0047642b  je         0x4766ce

// block 00476431
00476431  imul       eax, eax, 0xc
00476434  add        eax, ecx
00476436  mov        ebx, eax
00476438  mov        eax, 0x8000
0047643d  cmp        word ptr [ebx], ax
00476440  jb         0x476450

// block 00476442
00476442  mov        esi, ebx
00476444  lea        edi, [ebp - 0x48]
00476447  movsd      dword ptr es:[edi], dword ptr [esi]
00476448  movsd      dword ptr es:[edi], dword ptr [esi]
00476449  movsd      dword ptr es:[edi], dword ptr [esi]
0047644a  dec        dword ptr [ebp - 0x46]
0047644d  lea        ebx, [ebp - 0x48]

// block 00476450
00476450  movzx      ecx, word ptr [ebx + 0xa]
00476454  xor        eax, eax
00476456  mov        dword ptr [ebp - 0x50], eax
00476459  mov        dword ptr [ebp - 0x2c], eax
0047645c  mov        dword ptr [ebp - 0x28], eax
0047645f  mov        dword ptr [ebp - 0x24], eax
00476462  mov        eax, dword ptr [ebp - 0x32]
00476465  mov        esi, ecx
00476467  mov        edx, 0x7fff
0047646c  xor        esi, eax
0047646e  and        eax, edx
00476470  and        ecx, edx
00476472  and        esi, 0x8000
00476478  mov        edi, 0x7fff
0047647d  lea        edx, [ecx + eax]
00476480  mov        dword ptr [ebp - 0x70], esi
00476483  movzx      edx, dx
00476486  cmp        ax, di
00476489  jae        0x4766b0

// block 0047648f
0047648f  cmp        cx, di
00476492  jae        0x4766b0

// block 00476498
00476498  mov        edi, 0xbffd
0047649d  cmp        dx, di
004764a0  ja         0x4766b0

// block 004764a6
004764a6  mov        esi, 0x3fbf
004764ab  cmp        dx, si
004764ae  ja         0x4764bd

// block 004764b0
004764b0  xor        eax, eax
004764b2  mov        dword ptr [ebp - 0x38], eax
004764b5  mov        dword ptr [ebp - 0x3c], eax
004764b8  jmp        0x4766cb

// block 004764bd
004764bd  xor        esi, esi
004764bf  cmp        ax, si
004764c2  jne        0x4764e3

// block 004764c4
004764c4  inc        edx
004764c5  test       dword ptr [ebp - 0x34], 0x7fffffff
004764cc  jne        0x4764e3

// block 004764ce
004764ce  cmp        dword ptr [ebp - 0x38], esi
004764d1  jne        0x4764e3

// block 004764d3
004764d3  cmp        dword ptr [ebp - 0x3c], esi
004764d6  jne        0x4764e3

// block 004764d8
004764d8  xor        eax, eax
004764da  mov        word ptr [ebp - 0x32], ax
004764de  jmp        0x4766ce

// block 004764e3
004764e3  cmp        cx, si
004764e6  jne        0x476509

// block 004764e8
004764e8  inc        edx
004764e9  test       dword ptr [ebx + 8], 0x7fffffff
004764f0  jne        0x476509

// block 004764f2
004764f2  cmp        dword ptr [ebx + 4], esi
004764f5  jne        0x476509

// block 004764f7
004764f7  cmp        dword ptr [ebx], esi
004764f9  jne        0x476509

// block 004764fb
004764fb  mov        dword ptr [ebp - 0x34], esi
004764fe  mov        dword ptr [ebp - 0x38], esi
00476501  mov        dword ptr [ebp - 0x3c], esi
00476504  jmp        0x4766ce

// block 00476509
00476509  mov        dword ptr [ebp - 0x68], esi
0047650c  lea        edi, [ebp - 0x28]
0047650f  mov        dword ptr [ebp - 0x58], 5

// block 00476516
00476516  mov        eax, dword ptr [ebp - 0x68]
00476519  mov        ecx, dword ptr [ebp - 0x58]
0047651c  add        eax, eax
0047651e  mov        dword ptr [ebp - 0x64], ecx
00476521  test       ecx, ecx
00476523  jle        0x476577

// block 00476525
00476525  lea        eax, [ebp + eax - 0x3c]
00476529  mov        dword ptr [ebp - 0x5c], eax
0047652c  lea        eax, [ebx + 8]
0047652f  mov        dword ptr [ebp - 0x60], eax

// block 00476532
00476532  mov        eax, dword ptr [ebp - 0x60]
00476535  mov        ecx, dword ptr [ebp - 0x5c]
00476538  movzx      ecx, word ptr [ecx]
0047653b  movzx      eax, word ptr [eax]
0047653e  and        dword ptr [ebp - 0x4c], 0
00476542  imul       eax, ecx
00476545  mov        ecx, dword ptr [edi - 4]
00476548  lea        esi, [ecx + eax]
0047654b  cmp        esi, ecx
0047654d  jb         0x476553

// block 0047654f
0047654f  cmp        esi, eax
00476551  jae        0x47655a

// block 00476553
00476553  mov        dword ptr [ebp - 0x4c], 1

// block 0047655a
0047655a  cmp        dword ptr [ebp - 0x4c], 0
0047655e  mov        dword ptr [edi - 4], esi
00476561  je         0x476566

// block 00476563
00476563  inc        word ptr [edi]

// block 00476566
00476566  add        dword ptr [ebp - 0x5c], 2
0047656a  sub        dword ptr [ebp - 0x60], 2
0047656e  dec        dword ptr [ebp - 0x64]
00476571  cmp        dword ptr [ebp - 0x64], 0
00476575  jg         0x476532

// block 00476577
00476577  inc        edi
00476578  inc        edi
00476579  inc        dword ptr [ebp - 0x68]
0047657c  dec        dword ptr [ebp - 0x58]
0047657f  cmp        dword ptr [ebp - 0x58], 0
00476583  jg         0x476516

// block 00476585
00476585  add        edx, 0xc002
0047658b  test       dx, dx
0047658e  jle        0x4765c7

// block 00476590
00476590  mov        edi, dword ptr [ebp - 0x24]
00476593  test       edi, edi
00476595  js         0x4765c2

// block 00476597
00476597  mov        esi, dword ptr [ebp - 0x28]
0047659a  mov        eax, dword ptr [ebp - 0x2c]
0047659d  shl        dword ptr [ebp - 0x2c], 1
004765a0  shr        eax, 0x1f
004765a3  mov        ecx, esi
004765a5  add        esi, esi
004765a7  or         esi, eax
004765a9  shr        ecx, 0x1f
004765ac  lea        eax, [edi + edi]
004765af  or         eax, ecx
004765b1  add        edx, 0xffff
004765b7  mov        dword ptr [ebp - 0x28], esi
004765ba  mov        dword ptr [ebp - 0x24], eax
004765bd  test       dx, dx
004765c0  jg         0x476590

// block 004765c2
004765c2  test       dx, dx
004765c5  jg         0x476614

// block 004765c7
004765c7  add        edx, 0xffff
004765cd  test       dx, dx
004765d0  jge        0x476614

// block 004765d2
004765d2  mov        eax, edx
004765d4  neg        eax
004765d6  movzx      esi, ax
004765d9  add        edx, esi

// block 004765db
004765db  test       byte ptr [ebp - 0x2c], 1
004765df  je         0x4765e4

// block 004765e1
004765e1  inc        dword ptr [ebp - 0x50]

// block 004765e4
004765e4  mov        eax, dword ptr [ebp - 0x24]
004765e7  mov        edi, dword ptr [ebp - 0x28]
004765ea  mov        ecx, dword ptr [ebp - 0x28]
004765ed  shr        dword ptr [ebp - 0x24], 1
004765f0  shl        eax, 0x1f
004765f3  shr        edi, 1
004765f5  or         edi, eax
004765f7  mov        eax, dword ptr [ebp - 0x2c]
004765fa  shl        ecx, 0x1f
004765fd  shr        eax, 1
004765ff  or         eax, ecx
00476601  dec        esi
00476602  mov        dword ptr [ebp - 0x28], edi
00476605  mov        dword ptr [ebp - 0x2c], eax
00476608  jne        0x4765db

// block 0047660a
0047660a  cmp        dword ptr [ebp - 0x50], esi
0047660d  je         0x476614

// block 0047660f
0047660f  or         word ptr [ebp - 0x2c], 1

// block 00476614
00476614  mov        eax, 0x8000
00476619  mov        ecx, eax
0047661b  cmp        word ptr [ebp - 0x2c], cx
0047661f  ja         0x476632

// block 00476621
00476621  mov        ecx, dword ptr [ebp - 0x2c]
00476624  and        ecx, 0x1ffff
0047662a  cmp        ecx, 0x18000
00476630  jne        0x476666

// block 00476632
00476632  cmp        dword ptr [ebp - 0x2a], -1
00476636  jne        0x476663

// block 00476638
00476638  and        dword ptr [ebp - 0x2a], 0
0047663c  cmp        dword ptr [ebp - 0x26], -1
00476640  jne        0x47665e

// block 00476642
00476642  and        dword ptr [ebp - 0x26], 0
00476646  mov        ecx, 0xffff
0047664b  cmp        word ptr [ebp - 0x22], cx
0047664f  jne        0x476658

// block 00476651
00476651  mov        word ptr [ebp - 0x22], ax
00476655  inc        edx
00476656  jmp        0x476666

// block 00476658
00476658  inc        word ptr [ebp - 0x22]
0047665c  jmp        0x476666

// block 0047665e
0047665e  inc        dword ptr [ebp - 0x26]
00476661  jmp        0x476666

// block 00476663
00476663  inc        dword ptr [ebp - 0x2a]

// block 00476666
00476666  mov        eax, 0x7fff
0047666b  cmp        dx, ax
0047666e  jb         0x476693

// block 00476670
00476670  xor        eax, eax
00476672  xor        ecx, ecx
00476674  cmp        word ptr [ebp - 0x70], ax
00476678  mov        dword ptr [ebp - 0x38], eax
0047667b  sete       cl
0047667e  mov        dword ptr [ebp - 0x3c], eax
00476681  dec        ecx
00476682  and        ecx, 0x80000000
00476688  add        ecx, 0x7fff8000
0047668e  mov        dword ptr [ebp - 0x34], ecx
00476691  jmp        0x4766ce

// block 00476693
00476693  mov        ax, word ptr [ebp - 0x2a]
00476697  or         edx, dword ptr [ebp - 0x70]
0047669a  mov        word ptr [ebp - 0x3c], ax
0047669e  mov        eax, dword ptr [ebp - 0x28]
004766a1  mov        dword ptr [ebp - 0x3a], eax
004766a4  mov        eax, dword ptr [ebp - 0x24]
004766a7  mov        dword ptr [ebp - 0x36], eax
004766aa  mov        word ptr [ebp - 0x32], dx
004766ae  jmp        0x4766ce

// block 004766b0
004766b0  xor        eax, eax
004766b2  test       si, si
004766b5  sete       al
004766b8  and        dword ptr [ebp - 0x38], 0
004766bc  dec        eax
004766bd  and        eax, 0x80000000
004766c2  add        eax, 0x7fff8000
004766c7  and        dword ptr [ebp - 0x3c], 0

// block 004766cb
004766cb  mov        dword ptr [ebp - 0x34], eax

// block 004766ce
004766ce  cmp        dword ptr [ebp - 0x54], 0
004766d2  jne        0x476414

// block 004766d8
004766d8  mov        eax, dword ptr [ebp - 0x34]
004766db  movzx      ecx, word ptr [ebp - 0x3c]
004766df  mov        esi, dword ptr [ebp - 0x3a]
004766e2  mov        edx, dword ptr [ebp - 0x36]
004766e5  shr        eax, 0x10
004766e8  jmp        0x476719

// block 004766ea
004766ea  mov        dword ptr [ebp - 0x6c], 4
004766f1  jmp        0x476711

// block 004766f3
004766f3  xor        esi, esi
004766f5  mov        eax, 0x7fff
004766fa  mov        edx, 0x80000000
004766ff  xor        ecx, ecx
00476701  mov        dword ptr [ebp - 0x6c], 2
00476708  jmp        0x476719

// block 0047670a
0047670a  mov        dword ptr [ebp - 0x6c], 1

// block 00476711
00476711  xor        ecx, ecx
00476713  xor        eax, eax
00476715  xor        edx, edx
00476717  xor        esi, esi

// block 00476719
00476719  mov        edi, dword ptr [ebp - 0x78]
0047671c  or         eax, dword ptr [ebp - 0x74]
0047671f  mov        word ptr [edi], cx
00476722  mov        word ptr [edi + 0xa], ax
00476726  mov        eax, dword ptr [ebp - 0x6c]
00476729  mov        dword ptr [edi + 2], esi
0047672c  mov        dword ptr [edi + 6], edx

// block 0047672f
0047672f  mov        ecx, dword ptr [ebp - 4]
00476732  pop        edi
00476733  pop        esi
00476734  xor        ecx, ebp
00476736  pop        ebx
00476737  call       0x46c932

// block 0047673c
0047673c  leave      
0047673d  ret        
