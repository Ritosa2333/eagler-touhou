
// block 00424240
00424240  sub        esp, 0x28
00424243  push       esi
00424244  push       1
00424246  lea        eax, [esp + 0xc]
0042424a  push       eax
0042424b  mov        eax, dword ptr [esp + 0x3c]
0042424f  mov        dword ptr [esp + 0xc], 0xffffffff
00424257  mov        dword ptr [esp + 0x18], 0
0042425f  call       0x463c10

// block 00424264
00424264  mov        esi, eax
00424266  mov        dword ptr [esp + 0x1c], esi
0042426a  test       esi, esi
0042426c  jne        0x424278

// block 0042426e
0042426e  or         eax, 0xffffffff
00424271  pop        esi
00424272  add        esp, 0x28
00424275  ret        0xc

// block 00424278
00424278  push       ebx
00424279  push       ebp
0042427a  push       0x1000
0042427f  call       0x46d04a

// block 00424284
00424284  mov        ebx, eax
00424286  push       0x1000
0042428b  mov        dword ptr [esp + 0x1c], ebx
0042428f  call       0x46d04a

// block 00424294
00424294  mov        ebp, eax
00424296  push       0x1000
0042429b  mov        dword ptr [esp + 0x2c], ebp
0042429f  call       0x46d04a

// block 004242a4
004242a4  add        esp, 0xc
004242a7  cmp        dword ptr [esp + 0x10], 0
004242ac  mov        dword ptr [esp + 0x3c], eax
004242b0  mov        eax, esi
004242b2  jle        0x4243fe

// block 004242b8
004242b8  push       edi
004242b9  mov        esi, ebx
004242bb  sub        esi, ebp
004242bd  mov        edi, ebp
004242bf  sub        edi, ebx
004242c1  mov        dword ptr [esp + 0x30], esi
004242c5  mov        dword ptr [esp + 0x2c], edi
004242c9  jmp        0x4242dc

// block 004242d0
004242d0  mov        edi, dword ptr [esp + 0x2c]
004242d4  mov        esi, dword ptr [esp + 0x30]
004242d8  mov        eax, dword ptr [esp + 0x20]

// block 004242dc
004242dc  mov        ecx, dword ptr [esp + 0x18]
004242e0  push       ecx
004242e1  lea        ebx, [esp + 0x18]
004242e5  call       0x424a20

// block 004242ea
004242ea  mov        dword ptr [esp + 0x20], eax
004242ee  mov        eax, ebp

// block 004242f0
004242f0  mov        cl, byte ptr [esi + eax]
004242f3  mov        byte ptr [eax], cl
004242f5  inc        eax
004242f6  test       cl, cl
004242f8  jne        0x4242f0

// block 004242fa
004242fa  push       0x3a
004242fc  push       ebp
004242fd  call       0x46d1a0

// block 00424302
00424302  add        esp, 8
00424305  test       eax, eax
00424307  jne        0x424316

// block 00424309
00424309  mov        eax, ebp
0042430b  call       0x424b50

// block 00424310
00424310  mov        ebx, dword ptr [esp + 0x40]
00424314  jmp        0x42433b

// block 00424316
00424316  mov        ebx, dword ptr [esp + 0x40]
0042431a  lea        ecx, [eax + 1]
0042431d  mov        esi, ebx
0042431f  sub        esi, ecx

// block 00424321
00424321  mov        dl, byte ptr [ecx]
00424323  mov        byte ptr [esi + ecx], dl
00424326  inc        ecx
00424327  test       dl, dl
00424329  jne        0x424321

// block 0042432b
0042432b  mov        byte ptr [eax], dl
0042432d  mov        eax, ebp
0042432f  call       0x424b50

// block 00424334
00424334  mov        eax, ebx
00424336  call       0x424b50

// block 0042433b
0042433b  mov        eax, dword ptr [esp + 0x18]
0042433f  nop        

// block 00424340
00424340  mov        cl, byte ptr [eax]
00424342  mov        byte ptr [edi + eax], cl
00424345  inc        eax
00424346  test       cl, cl
00424348  jne        0x424340

// block 0042434a
0042434a  push       0x3a
0042434c  push       ebp
0042434d  call       0x46d1a0

// block 00424352
00424352  add        esp, 8
00424355  test       eax, eax
00424357  jne        0x42435d

// block 00424359
00424359  mov        eax, ebp
0042435b  jmp        0x424379

// block 0042435d
0042435d  lea        ecx, [eax + 1]
00424360  mov        esi, ebx
00424362  sub        esi, ecx

// block 00424364
00424364  mov        dl, byte ptr [ecx]
00424366  mov        byte ptr [esi + ecx], dl
00424369  inc        ecx
0042436a  test       dl, dl
0042436c  jne        0x424364

// block 0042436e
0042436e  mov        byte ptr [eax], dl
00424370  mov        eax, ebp
00424372  call       0x424b50

// block 00424377
00424377  mov        eax, ebx

// block 00424379
00424379  call       0x424b50

// block 0042437e
0042437e  mov        ecx, 0x4a0154
00424383  mov        eax, ebp

// block 00424385
00424385  mov        dl, byte ptr [eax]
00424387  cmp        dl, byte ptr [ecx]
00424389  jne        0x4243a8

// block 0042438b
0042438b  test       dl, dl
0042438d  je         0x4243a1

// block 0042438f
0042438f  mov        dl, byte ptr [eax + 1]
00424392  cmp        dl, byte ptr [ecx + 1]
00424395  jne        0x4243a8

// block 00424397
00424397  add        eax, 2
0042439a  add        ecx, 2
0042439d  test       dl, dl
0042439f  jne        0x424385

// block 004243a1
004243a1  xor        eax, eax
004243a3  or         esi, 0xffffffff
004243a6  jmp        0x4243b1

// block 004243a8
004243a8  sbb        eax, eax
004243aa  mov        esi, 0xffffffff
004243af  sbb        eax, esi

// block 004243b1
004243b1  test       eax, eax
004243b3  jne        0x424428

// block 004243b5
004243b5  mov        ecx, 0x4a015c
004243ba  mov        eax, ebx
004243bc  lea        esp, [esp]

// block 004243c0
004243c0  mov        dl, byte ptr [eax]
004243c2  cmp        dl, byte ptr [ecx]
004243c4  jne        0x4243e0

// block 004243c6
004243c6  test       dl, dl
004243c8  je         0x4243dc

// block 004243ca
004243ca  mov        dl, byte ptr [eax + 1]
004243cd  cmp        dl, byte ptr [ecx + 1]
004243d0  jne        0x4243e0

// block 004243d2
004243d2  add        eax, 2
004243d5  add        ecx, 2
004243d8  test       dl, dl
004243da  jne        0x4243c0

// block 004243dc
004243dc  xor        eax, eax
004243de  jmp        0x4243e4

// block 004243e0
004243e0  sbb        eax, eax
004243e2  sbb        eax, esi

// block 004243e4
004243e4  test       eax, eax
004243e6  je         0x4249fe

// block 004243ec
004243ec  mov        eax, dword ptr [esp + 0x3c]
004243f0  call       0x4236f0

// block 004243f5
004243f5  mov        ebx, dword ptr [esp + 0x18]
004243f9  mov        esi, dword ptr [esp + 0x28]
004243fd  pop        edi

// block 004243fe
004243fe  push       ebx
004243ff  call       0x46c941

// block 00424404
00424404  push       ebp
00424405  call       0x46c941

// block 0042440a
0042440a  mov        edx, dword ptr [esp + 0x44]
0042440e  push       edx
0042440f  call       0x46c941

// block 00424414
00424414  push       esi
00424415  call       0x46c941

// block 0042441a
0042441a  add        esp, 0x10
0042441d  pop        ebp
0042441e  pop        ebx
0042441f  xor        eax, eax
00424421  pop        esi
00424422  add        esp, 0x28
00424425  ret        0xc

// block 00424428
00424428  mov        ecx, 0x4a0160
0042442d  mov        eax, ebp
0042442f  nop        

// block 00424430
00424430  mov        dl, byte ptr [eax]
00424432  cmp        dl, byte ptr [ecx]
00424434  jne        0x424450

// block 00424436
00424436  test       dl, dl
00424438  je         0x42444c

// block 0042443a
0042443a  mov        dl, byte ptr [eax + 1]
0042443d  cmp        dl, byte ptr [ecx + 1]
00424440  jne        0x424450

// block 00424442
00424442  add        eax, 2
00424445  add        ecx, 2
00424448  test       dl, dl
0042444a  jne        0x424430

// block 0042444c
0042444c  xor        eax, eax
0042444e  jmp        0x424454

// block 00424450
00424450  sbb        eax, eax
00424452  sbb        eax, esi

// block 00424454
00424454  test       eax, eax
00424456  jne        0x424483

// block 00424458
00424458  push       ebx
00424459  call       0x46d143

// block 0042445e
0042445e  add        esp, 4
00424461  mov        dword ptr [esp + 0x10], eax
00424465  mov        dword ptr [esp + 0x1c], 0
0042446d  test       eax, eax
0042446f  jle        0x42447a

// block 00424471
00424471  cmp        eax, 8
00424474  jl         0x4249fe

// block 0042447a
0042447a  mov        dword ptr [esp + 0x10], esi
0042447e  jmp        0x4249fe

// block 00424483
00424483  mov        ecx, 0x4a0168
00424488  mov        eax, ebp
0042448a  lea        ebx, [ebx]

// block 00424490
00424490  mov        dl, byte ptr [eax]
00424492  cmp        dl, byte ptr [ecx]
00424494  jne        0x4244b0

// block 00424496
00424496  test       dl, dl
00424498  je         0x4244ac

// block 0042449a
0042449a  mov        dl, byte ptr [eax + 1]
0042449d  cmp        dl, byte ptr [ecx + 1]
004244a0  jne        0x4244b0

// block 004244a2
004244a2  add        eax, 2
004244a5  add        ecx, 2
004244a8  test       dl, dl
004244aa  jne        0x424490

// block 004244ac
004244ac  xor        eax, eax
004244ae  jmp        0x4244b4

// block 004244b0
004244b0  sbb        eax, eax
004244b2  sbb        eax, esi

// block 004244b4
004244b4  test       eax, eax
004244b6  jne        0x4244c1

// block 004244b8
004244b8  mov        dword ptr [esp + 0x10], esi
004244bc  jmp        0x4249fe

// block 004244c1
004244c1  mov        ecx, 0x4a0174
004244c6  mov        eax, ebp

// block 004244c8
004244c8  mov        dl, byte ptr [eax]
004244ca  cmp        dl, byte ptr [ecx]
004244cc  jne        0x4244e8

// block 004244ce
004244ce  test       dl, dl
004244d0  je         0x4244e4

// block 004244d2
004244d2  mov        dl, byte ptr [eax + 1]
004244d5  cmp        dl, byte ptr [ecx + 1]
004244d8  jne        0x4244e8

// block 004244da
004244da  add        eax, 2
004244dd  add        ecx, 2
004244e0  test       dl, dl
004244e2  jne        0x4244c8

// block 004244e4
004244e4  xor        eax, eax
004244e6  jmp        0x4244ec

// block 004244e8
004244e8  sbb        eax, eax
004244ea  sbb        eax, esi

// block 004244ec
004244ec  test       eax, eax
004244ee  jne        0x4249fe

// block 004244f4
004244f4  cmp        dword ptr [esp + 0x10], eax
004244f8  jle        0x4249fe

// block 004244fe
004244fe  cmp        dword ptr [esp + 0x1c], 0xff
00424506  jl         0x424511

// block 00424508
00424508  mov        dword ptr [esp + 0x10], esi
0042450c  jmp        0x4249fe

// block 00424511
00424511  push       0x88
00424516  call       0x46c9ea

// block 0042451b
0042451b  add        esp, 4
0042451e  test       eax, eax
00424520  je         0x42452d

// block 00424522
00424522  mov        esi, eax
00424524  call       0x423340

// block 00424529
00424529  mov        ebp, eax
0042452b  jmp        0x42452f

// block 0042452d
0042452d  xor        ebp, ebp

// block 0042452f
0042452f  inc        dword ptr [esp + 0x1c]
00424533  cmp        dword ptr [esp + 0x14], 0
00424538  mov        dword ptr [ebp + 0x18], 0
0042453f  jle        0x424960

// block 00424545
00424545  mov        esi, dword ptr [esp + 0x18]
00424549  mov        eax, dword ptr [esp + 0x20]
0042454d  push       esi
0042454e  lea        ebx, [esp + 0x18]
00424552  call       0x424a20

// block 00424557
00424557  mov        ebx, dword ptr [esp + 0x40]
0042455b  mov        edi, dword ptr [esp + 0x24]
0042455f  mov        dword ptr [esp + 0x20], eax
00424563  mov        eax, esi
00424565  call       0x424b00

// block 0042456a
0042456a  mov        ecx, 0x4a01a0
0042456f  mov        eax, edi

// block 00424571
00424571  mov        dl, byte ptr [eax]
00424573  cmp        dl, byte ptr [ecx]
00424575  jne        0x424591

// block 00424577
00424577  test       dl, dl
00424579  je         0x42458d

// block 0042457b
0042457b  mov        dl, byte ptr [eax + 1]
0042457e  cmp        dl, byte ptr [ecx + 1]
00424581  jne        0x424591

// block 00424583
00424583  add        eax, 2
00424586  add        ecx, 2
00424589  test       dl, dl
0042458b  jne        0x424571

// block 0042458d
0042458d  xor        eax, eax
0042458f  jmp        0x424596

// block 00424591
00424591  sbb        eax, eax
00424593  sbb        eax, -1

// block 00424596
00424596  test       eax, eax
00424598  jne        0x4245ef

// block 0042459a
0042459a  mov        edx, ebx
0042459c  push       0x2c
0042459e  push       edx
0042459f  call       0x46d1a0

// block 004245a4
004245a4  mov        esi, eax
004245a6  add        esp, 8
004245a9  test       esi, esi
004245ab  je         0x424955

// block 004245b1
004245b1  mov        edi, ebx
004245b3  mov        byte ptr [esi], 0
004245b6  mov        eax, edi
004245b8  inc        esi
004245b9  call       0x424b50

// block 004245be
004245be  mov        eax, esi
004245c0  call       0x424b50

// block 004245c5
004245c5  push       edi
004245c6  call       0x46d143

// block 004245cb
004245cb  mov        dword ptr [esp + 0x38], eax
004245cf  fild       dword ptr [esp + 0x38]
004245d3  push       esi
004245d4  fstp       dword ptr [ebp]
004245d7  call       0x46d143

// block 004245dc
004245dc  mov        dword ptr [esp + 0x3c], eax
004245e0  fild       dword ptr [esp + 0x3c]
004245e4  add        esp, 8
004245e7  fstp       dword ptr [ebp + 4]
004245ea  jmp        0x424955

// block 004245ef
004245ef  mov        ecx, 0x4a01a4
004245f4  mov        eax, edi

// block 004245f6
004245f6  mov        dl, byte ptr [eax]
004245f8  cmp        dl, byte ptr [ecx]
004245fa  jne        0x424616

// block 004245fc
004245fc  test       dl, dl
004245fe  je         0x424612

// block 00424600
00424600  mov        dl, byte ptr [eax + 1]
00424603  cmp        dl, byte ptr [ecx + 1]
00424606  jne        0x424616

// block 00424608
00424608  add        eax, 2
0042460b  add        ecx, 2
0042460e  test       dl, dl
00424610  jne        0x4245f6

// block 00424612
00424612  xor        eax, eax
00424614  jmp        0x42461b

// block 00424616
00424616  sbb        eax, eax
00424618  sbb        eax, -1

// block 0042461b
0042461b  test       eax, eax
0042461d  jne        0x424661

// block 0042461f
0042461f  mov        eax, ebx
00424621  push       0x22
00424623  push       eax
00424624  call       0x46d1a0

// block 00424629
00424629  mov        esi, eax
0042462b  add        esp, 8
0042462e  test       esi, esi
00424630  je         0x424955

// block 00424636
00424636  inc        esi
00424637  push       0x22
00424639  push       esi
0042463a  call       0x46d260

// block 0042463f
0042463f  add        esp, 8
00424642  test       eax, eax
00424644  je         0x424960

// block 0042464a
0042464a  push       0x41
0042464c  lea        ecx, [ebp + 0x1c]
0042464f  push       esi
00424650  push       ecx
00424651  mov        byte ptr [eax], 0
00424654  call       0x46d290

// block 00424659
00424659  add        esp, 0xc
0042465c  jmp        0x424955

// block 00424661
00424661  mov        ecx, 0x4a01ac
00424666  mov        eax, edi

// block 00424668
00424668  mov        dl, byte ptr [eax]
0042466a  cmp        dl, byte ptr [ecx]
0042466c  jne        0x424688

// block 0042466e
0042466e  test       dl, dl
00424670  je         0x424684

// block 00424672
00424672  mov        dl, byte ptr [eax + 1]
00424675  cmp        dl, byte ptr [ecx + 1]
00424678  jne        0x424688

// block 0042467a
0042467a  add        eax, 2
0042467d  add        ecx, 2
00424680  test       dl, dl
00424682  jne        0x424668

// block 00424684
00424684  xor        eax, eax
00424686  jmp        0x42468d

// block 00424688
00424688  sbb        eax, eax
0042468a  sbb        eax, -1

// block 0042468d
0042468d  test       eax, eax
0042468f  jne        0x4246a2

// block 00424691
00424691  push       ebx
00424692  call       0x46d143

// block 00424697
00424697  add        esp, 4
0042469a  mov        dword ptr [ebp + 0x60], eax
0042469d  jmp        0x424955

// block 004246a2
004246a2  mov        ecx, 0x4a01b4
004246a7  mov        eax, edi
004246a9  lea        esp, [esp]

// block 004246b0
004246b0  mov        dl, byte ptr [eax]
004246b2  cmp        dl, byte ptr [ecx]
004246b4  jne        0x4246d0

// block 004246b6
004246b6  test       dl, dl
004246b8  je         0x4246cc

// block 004246ba
004246ba  mov        dl, byte ptr [eax + 1]
004246bd  cmp        dl, byte ptr [ecx + 1]
004246c0  jne        0x4246d0

// block 004246c2
004246c2  add        eax, 2
004246c5  add        ecx, 2
004246c8  test       dl, dl
004246ca  jne        0x4246b0

// block 004246cc
004246cc  xor        eax, eax
004246ce  jmp        0x4246d5

// block 004246d0
004246d0  sbb        eax, eax
004246d2  sbb        eax, -1

// block 004246d5
004246d5  test       eax, eax
004246d7  jne        0x4246ea

// block 004246d9
004246d9  push       ebx
004246da  call       0x46d143

// block 004246df
004246df  add        esp, 4
004246e2  mov        dword ptr [ebp + 0x6c], eax
004246e5  jmp        0x424955

// block 004246ea
004246ea  mov        ecx, 0x4a01bc
004246ef  mov        eax, edi

// block 004246f1
004246f1  mov        dl, byte ptr [eax]
004246f3  cmp        dl, byte ptr [ecx]
004246f5  jne        0x424711

// block 004246f7
004246f7  test       dl, dl
004246f9  je         0x42470d

// block 004246fb
004246fb  mov        dl, byte ptr [eax + 1]
004246fe  cmp        dl, byte ptr [ecx + 1]
00424701  jne        0x424711

// block 00424703
00424703  add        eax, 2
00424706  add        ecx, 2
00424709  test       dl, dl
0042470b  jne        0x4246f1

// block 0042470d
0042470d  xor        eax, eax
0042470f  jmp        0x424716

// block 00424711
00424711  sbb        eax, eax
00424713  sbb        eax, -1

// block 00424716
00424716  test       eax, eax
00424718  jne        0x42472e

// block 0042471a
0042471a  push       0x3d
0042471c  mov        edi, 0x4aef10
00424721  call       0x4241b0

// block 00424726
00424726  mov        dword ptr [ebp + 0x68], eax
00424729  jmp        0x424955

// block 0042472e
0042472e  mov        ecx, 0x4a01c4
00424733  mov        eax, edi

// block 00424735
00424735  mov        dl, byte ptr [eax]
00424737  cmp        dl, byte ptr [ecx]
00424739  jne        0x424755

// block 0042473b
0042473b  test       dl, dl
0042473d  je         0x424751

// block 0042473f
0042473f  mov        dl, byte ptr [eax + 1]
00424742  cmp        dl, byte ptr [ecx + 1]
00424745  jne        0x424755

// block 00424747
00424747  add        eax, 2
0042474a  add        ecx, 2
0042474d  test       dl, dl
0042474f  jne        0x424735

// block 00424751
00424751  xor        eax, eax
00424753  jmp        0x42475a

// block 00424755
00424755  sbb        eax, eax
00424757  sbb        eax, -1

// block 0042475a
0042475a  test       eax, eax
0042475c  jne        0x424772

// block 0042475e
0042475e  push       3
00424760  mov        edi, 0x4aeef8
00424765  call       0x4241b0

// block 0042476a
0042476a  mov        dword ptr [ebp + 0x64], eax
0042476d  jmp        0x424955

// block 00424772
00424772  mov        ecx, 0x4a01cc
00424777  mov        eax, edi
00424779  lea        esp, [esp]

// block 00424780
00424780  mov        dl, byte ptr [eax]
00424782  cmp        dl, byte ptr [ecx]
00424784  jne        0x4247a0

// block 00424786
00424786  test       dl, dl
00424788  je         0x42479c

// block 0042478a
0042478a  mov        dl, byte ptr [eax + 1]
0042478d  cmp        dl, byte ptr [ecx + 1]
00424790  jne        0x4247a0

// block 00424792
00424792  add        eax, 2
00424795  add        ecx, 2
00424798  test       dl, dl
0042479a  jne        0x424780

// block 0042479c
0042479c  xor        eax, eax
0042479e  jmp        0x4247a5

// block 004247a0
004247a0  sbb        eax, eax
004247a2  sbb        eax, -1

// block 004247a5
004247a5  test       eax, eax
004247a7  jne        0x4247ba

// block 004247a9
004247a9  push       ebx
004247aa  call       0x46d143

// block 004247af
004247af  add        esp, 4
004247b2  mov        dword ptr [ebp + 0x70], eax
004247b5  jmp        0x424955

// block 004247ba
004247ba  mov        ecx, 0x4a01d4
004247bf  mov        eax, edi

// block 004247c1
004247c1  mov        dl, byte ptr [eax]
004247c3  cmp        dl, byte ptr [ecx]
004247c5  jne        0x4247e1

// block 004247c7
004247c7  test       dl, dl
004247c9  je         0x4247dd

// block 004247cb
004247cb  mov        dl, byte ptr [eax + 1]
004247ce  cmp        dl, byte ptr [ecx + 1]
004247d1  jne        0x4247e1

// block 004247d3
004247d3  add        eax, 2
004247d6  add        ecx, 2
004247d9  test       dl, dl
004247db  jne        0x4247c1

// block 004247dd
004247dd  xor        eax, eax
004247df  jmp        0x4247e6

// block 004247e1
004247e1  sbb        eax, eax
004247e3  sbb        eax, -1

// block 004247e6
004247e6  test       eax, eax
004247e8  jne        0x424836

// block 004247ea
004247ea  push       ebx
004247eb  call       0x46d4f7

// block 004247f0
004247f0  fstp       dword ptr [esp + 0x38]
004247f4  fld        dword ptr [esp + 0x38]
004247f8  add        esp, 4
004247fb  fst        dword ptr [ebp + 0x7c]
004247fe  fld        dword ptr [0x4a3fb0]
00424804  fcom       st(1)
00424806  fnstsw     ax
00424808  test       ah, 5
0042480b  jp         0x424817

// block 0042480d
0042480d  fstp       st(1)
0042480f  fstp       dword ptr [ebp + 0x7c]
00424812  jmp        0x424955

// block 00424817
00424817  fstp       st(0)
00424819  fld        dword ptr [0x4a3ff8]
0042481f  fcom       st(1)
00424821  fnstsw     ax
00424823  fstp       st(1)
00424825  test       ah, 0x41
00424828  jne        0x424953

// block 0042482e
0042482e  fstp       dword ptr [ebp + 0x7c]
00424831  jmp        0x424955

// block 00424836
00424836  mov        ecx, 0x4a01dc
0042483b  mov        eax, edi
0042483d  lea        ecx, [ecx]

// block 00424840
00424840  mov        dl, byte ptr [eax]
00424842  cmp        dl, byte ptr [ecx]
00424844  jne        0x424860

// block 00424846
00424846  test       dl, dl
00424848  je         0x42485c

// block 0042484a
0042484a  mov        dl, byte ptr [eax + 1]
0042484d  cmp        dl, byte ptr [ecx + 1]
00424850  jne        0x424860

// block 00424852
00424852  add        eax, 2
00424855  add        ecx, 2
00424858  test       dl, dl
0042485a  jne        0x424840

// block 0042485c
0042485c  xor        eax, eax
0042485e  jmp        0x424865

// block 00424860
00424860  sbb        eax, eax
00424862  sbb        eax, -1

// block 00424865
00424865  test       eax, eax
00424867  jne        0x4248e0

// block 00424869
00424869  mov        edx, ebx
0042486b  push       0x2c
0042486d  push       edx
0042486e  call       0x46d1a0

// block 00424873
00424873  mov        esi, eax
00424875  add        esp, 8
00424878  test       esi, esi
0042487a  je         0x424955

// block 00424880
00424880  mov        byte ptr [esi], 0
00424883  mov        eax, ebx
00424885  inc        esi
00424886  call       0x424b50

// block 0042488b
0042488b  push       eax
0042488c  call       0x46d143

// block 00424891
00424891  push       0x2c
00424893  push       esi
00424894  mov        byte ptr [ebp + 0x86], al
0042489a  mov        edi, esi
0042489c  call       0x46d1a0

// block 004248a1
004248a1  mov        esi, eax
004248a3  add        esp, 0xc
004248a6  test       esi, esi
004248a8  je         0x424955

// block 004248ae
004248ae  mov        eax, edi
004248b0  mov        byte ptr [esi], 0
004248b3  call       0x424b50

// block 004248b8
004248b8  push       edi
004248b9  call       0x46d143

// block 004248be
004248be  mov        byte ptr [ebp + 0x85], al
004248c4  add        esp, 4
004248c7  lea        eax, [esi + 1]
004248ca  call       0x424b50

// block 004248cf
004248cf  push       eax
004248d0  call       0x46d143

// block 004248d5
004248d5  add        esp, 4
004248d8  mov        byte ptr [ebp + 0x84], al
004248de  jmp        0x424955

// block 004248e0
004248e0  mov        ecx, 0x4a01e4
004248e5  mov        eax, edi

// block 004248e7
004248e7  mov        dl, byte ptr [eax]
004248e9  cmp        dl, byte ptr [ecx]
004248eb  jne        0x424907

// block 004248ed
004248ed  test       dl, dl
004248ef  je         0x424903

// block 004248f1
004248f1  mov        dl, byte ptr [eax + 1]
004248f4  cmp        dl, byte ptr [ecx + 1]
004248f7  jne        0x424907

// block 004248f9
004248f9  add        eax, 2
004248fc  add        ecx, 2
004248ff  test       dl, dl
00424901  jne        0x4248e7

// block 00424903
00424903  xor        eax, eax
00424905  jmp        0x42490c

// block 00424907
00424907  sbb        eax, eax
00424909  sbb        eax, -1

// block 0042490c
0042490c  test       eax, eax
0042490e  jne        0x424921

// block 00424910
00424910  push       ebx
00424911  call       0x46d143

// block 00424916
00424916  add        esp, 4
00424919  mov        byte ptr [ebp + 0x87], al
0042491f  jmp        0x424955

// block 00424921
00424921  mov        ecx, 0x4a01ec
00424926  mov        eax, edi

// block 00424928
00424928  mov        dl, byte ptr [eax]
0042492a  cmp        dl, byte ptr [ecx]
0042492c  jne        0x424948

// block 0042492e
0042492e  test       dl, dl
00424930  je         0x424944

// block 00424932
00424932  mov        dl, byte ptr [eax + 1]
00424935  cmp        dl, byte ptr [ecx + 1]
00424938  jne        0x424948

// block 0042493a
0042493a  add        eax, 2
0042493d  add        ecx, 2
00424940  test       dl, dl
00424942  jne        0x424928

// block 00424944
00424944  xor        eax, eax
00424946  jmp        0x42494d

// block 00424948
00424948  sbb        eax, eax
0042494a  sbb        eax, -1

// block 0042494d
0042494d  test       eax, eax
0042494f  je         0x424960

// block 00424951
00424951  jmp        0x424955

// block 00424953
00424953  fstp       st(0)

// block 00424955
00424955  cmp        dword ptr [esp + 0x14], 0
0042495a  jg         0x424545

// block 00424960
00424960  lea        eax, [ebp + 0x1c]
00424963  lea        edx, [eax + 1]

// block 00424966
00424966  mov        cl, byte ptr [eax]
00424968  inc        eax
00424969  test       cl, cl
0042496b  jne        0x424966

// block 0042496d
0042496d  fld        dword ptr [ebp + 0x7c]
00424970  sub        eax, edx
00424972  mov        dword ptr [esp + 0x34], eax
00424976  fild       dword ptr [esp + 0x34]
0042497a  test       eax, eax
0042497c  jge        0x424984

// block 0042497e
0042497e  fadd       dword ptr [0x4a3e10]

// block 00424984
00424984  mov        eax, dword ptr [esp + 0x10]
00424988  fmulp      st(1)
0042498a  mov        ecx, dword ptr [esp + 0x3c]
0042498e  lea        eax, [eax + eax*2 + 6]
00424992  fmul       qword ptr [0x4a3ef8]
00424998  lea        edx, [ebp + 0xc]
0042499b  lea        eax, [ecx + eax*4]
0042499e  fmul       qword ptr [0x4a3cf8]
004249a4  fstp       dword ptr [ebp + 0x80]
004249aa  call       0x4109f0

// block 004249af
004249af  cmp        dword ptr [esp + 0x44], 0
004249b4  jne        0x4249fe

// block 004249b6
004249b6  push       0x88
004249bb  call       0x46c9ea

// block 004249c0
004249c0  xor        ebx, ebx
004249c2  add        esp, 4
004249c5  cmp        eax, ebx
004249c7  je         0x4249d2

// block 004249c9
004249c9  mov        esi, eax
004249cb  call       0x423340

// block 004249d0
004249d0  jmp        0x4249d4

// block 004249d2
004249d2  xor        eax, eax

// block 004249d4
004249d4  lea        edx, [eax + 0xc]
004249d7  mov        edi, eax
004249d9  mov        ecx, 0x22
004249de  mov        esi, ebp

// block 004249e0
004249e0  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 004249e2
004249e2  mov        ecx, dword ptr [esp + 0x3c]
004249e6  mov        dword ptr [edx], eax
004249e8  mov        eax, dword ptr [esp + 0x10]
004249ec  lea        eax, [eax + eax*2 + 0x1e]
004249f0  lea        eax, [ecx + eax*4]
004249f3  mov        dword ptr [edx + 4], ebx
004249f6  mov        dword ptr [edx + 8], ebx
004249f9  call       0x4109f0

// block 004249fe
004249fe  cmp        dword ptr [esp + 0x14], 0
00424a03  mov        ebp, dword ptr [esp + 0x24]
00424a07  jg         0x4242d0

// block 00424a0d
00424a0d  jmp        0x4243f5
