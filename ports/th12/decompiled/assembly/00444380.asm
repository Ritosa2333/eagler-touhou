
// block 00444380
00444380  push       ebx
00444381  push       ebp
00444382  mov        ebp, dword ptr [esp + 0xc]
00444386  push       esi
00444387  push       edi
00444388  xor        ebx, ebx
0044438a  xor        edi, edi
0044438c  cmp        dword ptr [ebp + 0x28], ebx
0044438f  jle        0x4447f6

// block 00444395
00444395  mov        ecx, dword ptr [ebp + 0x2d0]
0044439b  lea        esi, [edi + 0x2d]
0044439e  cmp        ecx, ebx
004443a0  jne        0x4443a6

// block 004443a2
004443a2  xor        eax, eax
004443a4  jmp        0x4443ef

// block 004443a6
004443a6  mov        edx, dword ptr [0x4ce8cc]
004443ac  mov        eax, dword ptr [edx + 0x8856b8]
004443b2  cmp        eax, ebx
004443b4  je         0x4443c4

// block 004443b6
004443b6  mov        ebp, dword ptr [eax]
004443b8  cmp        dword ptr [ebp], ecx
004443bb  je         0x4443e5

// block 004443bd
004443bd  mov        eax, dword ptr [eax + 4]
004443c0  cmp        eax, ebx
004443c2  jne        0x4443b6

// block 004443c4
004443c4  mov        eax, dword ptr [edx + 0x8856c0]
004443ca  cmp        eax, ebx
004443cc  je         0x4443e1

// block 004443ce
004443ce  mov        edi, edi

// block 004443d0
004443d0  mov        edx, dword ptr [eax]
004443d2  cmp        dword ptr [edx], ecx
004443d4  je         0x444459

// block 004443da
004443da  mov        eax, dword ptr [eax + 4]
004443dd  cmp        eax, ebx
004443df  jne        0x4443d0

// block 004443e1
004443e1  xor        eax, eax
004443e3  jmp        0x4443eb

// block 004443e5
004443e5  mov        eax, ebp

// block 004443e7
004443e7  cmp        eax, ebx
004443e9  jne        0x4443f5

// block 004443eb
004443eb  mov        ebp, dword ptr [esp + 0x14]

// block 004443ef
004443ef  mov        dword ptr [ebp + 0x2d0], ebx

// block 004443f5
004443f5  lea        ecx, [eax + 0x10]
004443f8  cmp        ecx, ebx
004443fa  je         0x44441d

// block 004443fc
004443fc  lea        esp, [esp]

// block 00444400
00444400  mov        edx, dword ptr [ecx]
00444402  movsx      ebp, word ptr [edx + 0x3ea]
00444409  cmp        ebp, esi
0044440b  je         0x44445d

// block 0044440d
0044440d  cmp        esi, -1
00444410  jne        0x444416

// block 00444412
00444412  cmp        edx, eax
00444414  jne        0x44445d

// block 00444416
00444416  mov        ecx, dword ptr [ecx + 4]
00444419  cmp        ecx, ebx
0044441b  jne        0x444400

// block 0044441d
0044441d  xor        esi, esi

// block 0044441f
0044441f  mov        eax, dword ptr [esi + 0x494]
00444425  cmp        eax, ebx
00444427  je         0x444432

// block 00444429
00444429  mov        edx, 0x1e
0044442e  mov        ecx, esi
00444430  call       eax

// block 00444432
00444432  mov        eax, 0x1e
00444437  push       esi
00444438  mov        word ptr [esi + 0x3c4], ax
0044443f  call       0x455630

// block 00444444
00444444  mov        ecx, dword ptr [esp + 0x14]
00444448  mov        ecx, dword ptr [ecx + 0x2d0]
0044444e  lea        esi, [edi + 0x34]
00444451  cmp        ecx, ebx
00444453  jne        0x444461

// block 00444455
00444455  xor        eax, eax
00444457  jmp        0x4444ab

// block 00444459
00444459  mov        eax, edx
0044445b  jmp        0x4443e7

// block 0044445d
0044445d  mov        esi, edx
0044445f  jmp        0x44441f

// block 00444461
00444461  mov        edx, dword ptr [0x4ce8cc]
00444467  mov        eax, dword ptr [edx + 0x8856b8]
0044446d  cmp        eax, ebx
0044446f  je         0x44447f

// block 00444471
00444471  mov        ebp, dword ptr [eax]
00444473  cmp        dword ptr [ebp], ecx
00444476  je         0x4444a1

// block 00444478
00444478  mov        eax, dword ptr [eax + 4]
0044447b  cmp        eax, ebx
0044447d  jne        0x444471

// block 0044447f
0044447f  mov        eax, dword ptr [edx + 0x8856c0]
00444485  cmp        eax, ebx
00444487  je         0x444455

// block 00444489
00444489  lea        esp, [esp]

// block 00444490
00444490  mov        edx, dword ptr [eax]
00444492  cmp        dword ptr [edx], ecx
00444494  je         0x4444a5

// block 00444496
00444496  mov        eax, dword ptr [eax + 4]
00444499  cmp        eax, ebx
0044449b  jne        0x444490

// block 0044449d
0044449d  xor        eax, eax
0044449f  jmp        0x4444ab

// block 004444a1
004444a1  mov        eax, ebp
004444a3  jmp        0x4444a7

// block 004444a5
004444a5  mov        eax, edx

// block 004444a7
004444a7  cmp        eax, ebx
004444a9  jne        0x4444b5

// block 004444ab
004444ab  mov        ecx, dword ptr [esp + 0x14]
004444af  mov        dword ptr [ecx + 0x2d0], ebx

// block 004444b5
004444b5  lea        ecx, [eax + 0x10]
004444b8  cmp        ecx, ebx
004444ba  je         0x4444dd

// block 004444bc
004444bc  lea        esp, [esp]

// block 004444c0
004444c0  mov        edx, dword ptr [ecx]
004444c2  movsx      ebp, word ptr [edx + 0x3ea]
004444c9  cmp        ebp, esi
004444cb  je         0x444523

// block 004444cd
004444cd  cmp        esi, -1
004444d0  jne        0x4444d6

// block 004444d2
004444d2  cmp        edx, eax
004444d4  jne        0x444523

// block 004444d6
004444d6  mov        ecx, dword ptr [ecx + 4]
004444d9  cmp        ecx, ebx
004444db  jne        0x4444c0

// block 004444dd
004444dd  xor        esi, esi

// block 004444df
004444df  mov        eax, dword ptr [esi + 0x494]
004444e5  cmp        eax, ebx
004444e7  je         0x4444f2

// block 004444e9
004444e9  mov        edx, 0x1e
004444ee  mov        ecx, esi
004444f0  call       eax

// block 004444f2
004444f2  mov        edx, 0x1e
004444f7  push       esi
004444f8  mov        word ptr [esi + 0x3c4], dx
004444ff  call       0x455630

// block 00444504
00444504  cmp        edi, 5
00444507  jge        0x4447e8

// block 0044450d
0044450d  mov        eax, dword ptr [esp + 0x14]
00444511  mov        ecx, dword ptr [eax + 0x2d0]
00444517  lea        esi, [edi + edi + 0x3b]
0044451b  cmp        ecx, ebx
0044451d  jne        0x444527

// block 0044451f
0044451f  xor        eax, eax
00444521  jmp        0x44456b

// block 00444523
00444523  mov        esi, edx
00444525  jmp        0x4444df

// block 00444527
00444527  mov        edx, dword ptr [0x4ce8cc]
0044452d  mov        eax, dword ptr [edx + 0x8856b8]
00444533  cmp        eax, ebx
00444535  je         0x444545

// block 00444537
00444537  mov        ebp, dword ptr [eax]
00444539  cmp        dword ptr [ebp], ecx
0044453c  je         0x444561

// block 0044453e
0044453e  mov        eax, dword ptr [eax + 4]
00444541  cmp        eax, ebx
00444543  jne        0x444537

// block 00444545
00444545  mov        eax, dword ptr [edx + 0x8856c0]
0044454b  cmp        eax, ebx
0044454d  je         0x44451f

// block 0044454f
0044454f  nop        

// block 00444550
00444550  mov        edx, dword ptr [eax]
00444552  cmp        dword ptr [edx], ecx
00444554  je         0x444565

// block 00444556
00444556  mov        eax, dword ptr [eax + 4]
00444559  cmp        eax, ebx
0044455b  jne        0x444550

// block 0044455d
0044455d  xor        eax, eax
0044455f  jmp        0x44456b

// block 00444561
00444561  mov        eax, ebp
00444563  jmp        0x444567

// block 00444565
00444565  mov        eax, edx

// block 00444567
00444567  cmp        eax, ebx
00444569  jne        0x444575

// block 0044456b
0044456b  mov        ecx, dword ptr [esp + 0x14]
0044456f  mov        dword ptr [ecx + 0x2d0], ebx

// block 00444575
00444575  lea        ecx, [eax + 0x10]
00444578  cmp        ecx, ebx
0044457a  je         0x44459d

// block 0044457c
0044457c  lea        esp, [esp]

// block 00444580
00444580  mov        edx, dword ptr [ecx]
00444582  movsx      ebp, word ptr [edx + 0x3ea]
00444589  cmp        ebp, esi
0044458b  je         0x4445da

// block 0044458d
0044458d  cmp        esi, -1
00444590  jne        0x444596

// block 00444592
00444592  cmp        edx, eax
00444594  jne        0x4445da

// block 00444596
00444596  mov        ecx, dword ptr [ecx + 4]
00444599  cmp        ecx, ebx
0044459b  jne        0x444580

// block 0044459d
0044459d  xor        esi, esi

// block 0044459f
0044459f  mov        eax, dword ptr [esi + 0x494]
004445a5  cmp        eax, ebx
004445a7  je         0x4445b2

// block 004445a9
004445a9  mov        edx, 0x1e
004445ae  mov        ecx, esi
004445b0  call       eax

// block 004445b2
004445b2  mov        edx, 0x1e
004445b7  push       esi
004445b8  mov        word ptr [esi + 0x3c4], dx
004445bf  call       0x455630

// block 004445c4
004445c4  mov        eax, dword ptr [esp + 0x14]
004445c8  mov        ecx, dword ptr [eax + 0x2d0]
004445ce  lea        esi, [edi + edi + 0x3c]
004445d2  cmp        ecx, ebx
004445d4  jne        0x4445de

// block 004445d6
004445d6  xor        eax, eax
004445d8  jmp        0x444623

// block 004445da
004445da  mov        esi, edx
004445dc  jmp        0x44459f

// block 004445de
004445de  mov        edx, dword ptr [0x4ce8cc]
004445e4  mov        eax, dword ptr [edx + 0x8856b8]
004445ea  cmp        eax, ebx
004445ec  je         0x4445fe

// block 004445ee
004445ee  mov        edi, edi

// block 004445f0
004445f0  mov        ebp, dword ptr [eax]
004445f2  cmp        dword ptr [ebp], ecx
004445f5  je         0x444619

// block 004445f7
004445f7  mov        eax, dword ptr [eax + 4]
004445fa  cmp        eax, ebx
004445fc  jne        0x4445f0

// block 004445fe
004445fe  mov        eax, dword ptr [edx + 0x8856c0]
00444604  cmp        eax, ebx
00444606  je         0x4445d6

// block 00444608
00444608  mov        edx, dword ptr [eax]
0044460a  cmp        dword ptr [edx], ecx
0044460c  je         0x44461d

// block 0044460e
0044460e  mov        eax, dword ptr [eax + 4]
00444611  cmp        eax, ebx
00444613  jne        0x444608

// block 00444615
00444615  xor        eax, eax
00444617  jmp        0x444623

// block 00444619
00444619  mov        eax, ebp
0044461b  jmp        0x44461f

// block 0044461d
0044461d  mov        eax, edx

// block 0044461f
0044461f  cmp        eax, ebx
00444621  jne        0x44462d

// block 00444623
00444623  mov        ecx, dword ptr [esp + 0x14]
00444627  mov        dword ptr [ecx + 0x2d0], ebx

// block 0044462d
0044462d  lea        ecx, [eax + 0x10]
00444630  cmp        ecx, ebx
00444632  je         0x444651

// block 00444634
00444634  mov        edx, dword ptr [ecx]
00444636  movsx      ebp, word ptr [edx + 0x3ea]
0044463d  cmp        ebp, esi
0044463f  je         0x44468e

// block 00444641
00444641  cmp        esi, -1
00444644  jne        0x44464a

// block 00444646
00444646  cmp        edx, eax
00444648  jne        0x44468e

// block 0044464a
0044464a  mov        ecx, dword ptr [ecx + 4]
0044464d  cmp        ecx, ebx
0044464f  jne        0x444634

// block 00444651
00444651  xor        esi, esi

// block 00444653
00444653  mov        eax, dword ptr [esi + 0x494]
00444659  cmp        eax, ebx
0044465b  je         0x444666

// block 0044465d
0044465d  mov        edx, 0x1e
00444662  mov        ecx, esi
00444664  call       eax

// block 00444666
00444666  mov        edx, 0x1e
0044466b  push       esi
0044466c  mov        word ptr [esi + 0x3c4], dx
00444673  call       0x455630

// block 00444678
00444678  mov        eax, dword ptr [esp + 0x14]
0044467c  mov        ecx, dword ptr [eax + 0x2d0]
00444682  lea        esi, [edi + edi + 0x45]
00444686  cmp        ecx, ebx
00444688  jne        0x444692

// block 0044468a
0044468a  xor        eax, eax
0044468c  jmp        0x4446db

// block 0044468e
0044468e  mov        esi, edx
00444690  jmp        0x444653

// block 00444692
00444692  mov        edx, dword ptr [0x4ce8cc]
00444698  mov        eax, dword ptr [edx + 0x8856b8]
0044469e  cmp        eax, ebx
004446a0  je         0x4446b0

// block 004446a2
004446a2  mov        ebp, dword ptr [eax]
004446a4  cmp        dword ptr [ebp], ecx
004446a7  je         0x4446d1

// block 004446a9
004446a9  mov        eax, dword ptr [eax + 4]
004446ac  cmp        eax, ebx
004446ae  jne        0x4446a2

// block 004446b0
004446b0  mov        eax, dword ptr [edx + 0x8856c0]
004446b6  cmp        eax, ebx
004446b8  je         0x44468a

// block 004446ba
004446ba  lea        ebx, [ebx]

// block 004446c0
004446c0  mov        edx, dword ptr [eax]
004446c2  cmp        dword ptr [edx], ecx
004446c4  je         0x4446d5

// block 004446c6
004446c6  mov        eax, dword ptr [eax + 4]
004446c9  cmp        eax, ebx
004446cb  jne        0x4446c0

// block 004446cd
004446cd  xor        eax, eax
004446cf  jmp        0x4446db

// block 004446d1
004446d1  mov        eax, ebp
004446d3  jmp        0x4446d7

// block 004446d5
004446d5  mov        eax, edx

// block 004446d7
004446d7  cmp        eax, ebx
004446d9  jne        0x4446e5

// block 004446db
004446db  mov        ecx, dword ptr [esp + 0x14]
004446df  mov        dword ptr [ecx + 0x2d0], ebx

// block 004446e5
004446e5  lea        ecx, [eax + 0x10]
004446e8  cmp        ecx, ebx
004446ea  je         0x44470d

// block 004446ec
004446ec  lea        esp, [esp]

// block 004446f0
004446f0  mov        edx, dword ptr [ecx]
004446f2  movsx      ebp, word ptr [edx + 0x3ea]
004446f9  cmp        ebp, esi
004446fb  je         0x44474a

// block 004446fd
004446fd  cmp        esi, -1
00444700  jne        0x444706

// block 00444702
00444702  cmp        edx, eax
00444704  jne        0x44474a

// block 00444706
00444706  mov        ecx, dword ptr [ecx + 4]
00444709  cmp        ecx, ebx
0044470b  jne        0x4446f0

// block 0044470d
0044470d  xor        esi, esi

// block 0044470f
0044470f  mov        eax, dword ptr [esi + 0x494]
00444715  cmp        eax, ebx
00444717  je         0x444722

// block 00444719
00444719  mov        edx, 0x1e
0044471e  mov        ecx, esi
00444720  call       eax

// block 00444722
00444722  mov        edx, 0x1e
00444727  push       esi
00444728  mov        word ptr [esi + 0x3c4], dx
0044472f  call       0x455630

// block 00444734
00444734  mov        eax, dword ptr [esp + 0x14]
00444738  mov        ecx, dword ptr [eax + 0x2d0]
0044473e  lea        esi, [edi + edi + 0x46]
00444742  cmp        ecx, ebx
00444744  jne        0x44474e

// block 00444746
00444746  xor        eax, eax
00444748  jmp        0x444793

// block 0044474a
0044474a  mov        esi, edx
0044474c  jmp        0x44470f

// block 0044474e
0044474e  mov        edx, dword ptr [0x4ce8cc]
00444754  mov        eax, dword ptr [edx + 0x8856b8]
0044475a  cmp        eax, ebx
0044475c  je         0x44476e

// block 0044475e
0044475e  mov        edi, edi

// block 00444760
00444760  mov        ebp, dword ptr [eax]
00444762  cmp        dword ptr [ebp], ecx
00444765  je         0x444789

// block 00444767
00444767  mov        eax, dword ptr [eax + 4]
0044476a  cmp        eax, ebx
0044476c  jne        0x444760

// block 0044476e
0044476e  mov        eax, dword ptr [edx + 0x8856c0]
00444774  cmp        eax, ebx
00444776  je         0x444746

// block 00444778
00444778  mov        edx, dword ptr [eax]
0044477a  cmp        dword ptr [edx], ecx
0044477c  je         0x44478d

// block 0044477e
0044477e  mov        eax, dword ptr [eax + 4]
00444781  cmp        eax, ebx
00444783  jne        0x444778

// block 00444785
00444785  xor        eax, eax
00444787  jmp        0x444793

// block 00444789
00444789  mov        eax, ebp
0044478b  jmp        0x44478f

// block 0044478d
0044478d  mov        eax, edx

// block 0044478f
0044478f  cmp        eax, ebx
00444791  jne        0x44479d

// block 00444793
00444793  mov        ecx, dword ptr [esp + 0x14]
00444797  mov        dword ptr [ecx + 0x2d0], ebx

// block 0044479d
0044479d  lea        ecx, [eax + 0x10]
004447a0  cmp        ecx, ebx
004447a2  je         0x4447c1

// block 004447a4
004447a4  mov        edx, dword ptr [ecx]
004447a6  movsx      ebp, word ptr [edx + 0x3ea]
004447ad  cmp        ebp, esi
004447af  je         0x444814

// block 004447b1
004447b1  cmp        esi, -1
004447b4  jne        0x4447ba

// block 004447b6
004447b6  cmp        edx, eax
004447b8  jne        0x444814

// block 004447ba
004447ba  mov        ecx, dword ptr [ecx + 4]
004447bd  cmp        ecx, ebx
004447bf  jne        0x4447a4

// block 004447c1
004447c1  xor        esi, esi

// block 004447c3
004447c3  mov        eax, dword ptr [esi + 0x494]
004447c9  cmp        eax, ebx
004447cb  je         0x4447d6

// block 004447cd
004447cd  mov        edx, 0x1e
004447d2  mov        ecx, esi
004447d4  call       eax

// block 004447d6
004447d6  mov        edx, 0x1e
004447db  push       esi
004447dc  mov        word ptr [esi + 0x3c4], dx
004447e3  call       0x455630

// block 004447e8
004447e8  mov        ebp, dword ptr [esp + 0x14]
004447ec  inc        edi
004447ed  cmp        edi, dword ptr [ebp + 0x28]
004447f0  jl         0x444395

// block 004447f6
004447f6  inc        edi
004447f7  cmp        edi, 7
004447fa  jge        0x44496e

// block 00444800
00444800  add        edi, 0x34

// block 00444803
00444803  mov        ecx, dword ptr [ebp + 0x2d0]
00444809  lea        esi, [edi - 7]
0044480c  cmp        ecx, ebx
0044480e  jne        0x444818

// block 00444810
00444810  xor        eax, eax
00444812  jmp        0x44485f

// block 00444814
00444814  mov        esi, edx
00444816  jmp        0x4447c3

// block 00444818
00444818  mov        edx, dword ptr [0x4ce8cc]
0044481e  mov        eax, dword ptr [edx + 0x8856b8]
00444824  cmp        eax, ebx
00444826  je         0x444836

// block 00444828
00444828  mov        ebp, dword ptr [eax]
0044482a  cmp        dword ptr [ebp], ecx
0044482d  je         0x444855

// block 0044482f
0044482f  mov        eax, dword ptr [eax + 4]
00444832  cmp        eax, ebx
00444834  jne        0x444828

// block 00444836
00444836  mov        eax, dword ptr [edx + 0x8856c0]
0044483c  cmp        eax, ebx
0044483e  je         0x444851

// block 00444840
00444840  mov        edx, dword ptr [eax]
00444842  cmp        dword ptr [edx], ecx
00444844  je         0x4448c6

// block 0044484a
0044484a  mov        eax, dword ptr [eax + 4]
0044484d  cmp        eax, ebx
0044484f  jne        0x444840

// block 00444851
00444851  xor        eax, eax
00444853  jmp        0x44485b

// block 00444855
00444855  mov        eax, ebp

// block 00444857
00444857  cmp        eax, ebx
00444859  jne        0x444865

// block 0044485b
0044485b  mov        ebp, dword ptr [esp + 0x14]

// block 0044485f
0044485f  mov        dword ptr [ebp + 0x2d0], ebx

// block 00444865
00444865  lea        ecx, [eax + 0x10]
00444868  cmp        ecx, ebx
0044486a  je         0x44488d

// block 0044486c
0044486c  lea        esp, [esp]

// block 00444870
00444870  mov        edx, dword ptr [ecx]
00444872  movsx      ebp, word ptr [edx + 0x3ea]
00444879  cmp        ebp, esi
0044487b  je         0x4448ca

// block 0044487d
0044487d  cmp        esi, -1
00444880  jne        0x444886

// block 00444882
00444882  cmp        edx, eax
00444884  jne        0x4448ca

// block 00444886
00444886  mov        ecx, dword ptr [ecx + 4]
00444889  cmp        ecx, ebx
0044488b  jne        0x444870

// block 0044488d
0044488d  xor        esi, esi

// block 0044488f
0044488f  mov        eax, dword ptr [esi + 0x494]
00444895  cmp        eax, ebx
00444897  je         0x4448a2

// block 00444899
00444899  mov        edx, 0x1f
0044489e  mov        ecx, esi
004448a0  call       eax

// block 004448a2
004448a2  mov        eax, 0x1f
004448a7  push       esi
004448a8  mov        word ptr [esi + 0x3c4], ax
004448af  call       0x455630

// block 004448b4
004448b4  mov        ebp, dword ptr [esp + 0x14]
004448b8  mov        ecx, dword ptr [ebp + 0x2d0]
004448be  cmp        ecx, ebx
004448c0  jne        0x4448ce

// block 004448c2
004448c2  xor        eax, eax
004448c4  jmp        0x444912

// block 004448c6
004448c6  mov        eax, edx
004448c8  jmp        0x444857

// block 004448ca
004448ca  mov        esi, edx
004448cc  jmp        0x44488f

// block 004448ce
004448ce  mov        edx, dword ptr [0x4ce8cc]
004448d4  mov        eax, dword ptr [edx + 0x8856b8]
004448da  cmp        eax, ebx
004448dc  je         0x4448ed

// block 004448de
004448de  mov        edi, edi

// block 004448e0
004448e0  mov        esi, dword ptr [eax]
004448e2  cmp        dword ptr [esi], ecx
004448e4  je         0x444908

// block 004448e6
004448e6  mov        eax, dword ptr [eax + 4]
004448e9  cmp        eax, ebx
004448eb  jne        0x4448e0

// block 004448ed
004448ed  mov        eax, dword ptr [edx + 0x8856c0]
004448f3  cmp        eax, ebx
004448f5  je         0x4448c2

// block 004448f7
004448f7  mov        edx, dword ptr [eax]
004448f9  cmp        dword ptr [edx], ecx
004448fb  je         0x44490c

// block 004448fd
004448fd  mov        eax, dword ptr [eax + 4]
00444900  cmp        eax, ebx
00444902  jne        0x4448f7

// block 00444904
00444904  xor        eax, eax
00444906  jmp        0x444912

// block 00444908
00444908  mov        eax, esi
0044490a  jmp        0x44490e

// block 0044490c
0044490c  mov        eax, edx

// block 0044490e
0044490e  cmp        eax, ebx
00444910  jne        0x444918

// block 00444912
00444912  mov        dword ptr [ebp + 0x2d0], ebx

// block 00444918
00444918  lea        ecx, [eax + 0x10]
0044491b  cmp        ecx, ebx
0044491d  je         0x44493d

// block 0044491f
0044491f  nop        

// block 00444920
00444920  mov        edx, dword ptr [ecx]
00444922  movsx      esi, word ptr [edx + 0x3ea]
00444929  cmp        esi, edi
0044492b  je         0x44497d

// block 0044492d
0044492d  cmp        edi, -1
00444930  jne        0x444936

// block 00444932
00444932  cmp        edx, eax
00444934  jne        0x44497d

// block 00444936
00444936  mov        ecx, dword ptr [ecx + 4]
00444939  cmp        ecx, ebx
0044493b  jne        0x444920

// block 0044493d
0044493d  xor        esi, esi

// block 0044493f
0044493f  mov        eax, dword ptr [esi + 0x494]
00444945  cmp        eax, ebx
00444947  je         0x444952

// block 00444949
00444949  mov        edx, 0x1f
0044494e  mov        ecx, esi
00444950  call       eax

// block 00444952
00444952  mov        eax, 0x1f
00444957  push       esi
00444958  mov        word ptr [esi + 0x3c4], ax
0044495f  call       0x455630

// block 00444964
00444964  inc        edi
00444965  cmp        edi, 0x3b
00444968  jl         0x444803

// block 0044496e
0044496e  mov        edi, dword ptr [ebp + 0x28]
00444971  inc        edi
00444972  cmp        edi, 5
00444975  jge        0x444c6e

// block 0044497b
0044497b  jmp        0x444985

// block 0044497d
0044497d  mov        esi, edx
0044497f  jmp        0x44493f

// block 00444981
00444981  mov        ebp, dword ptr [esp + 0x14]

// block 00444985
00444985  mov        ecx, dword ptr [ebp + 0x2d0]
0044498b  lea        esi, [edi + edi + 0x3b]
0044498f  cmp        ecx, ebx
00444991  jne        0x444997

// block 00444993
00444993  xor        eax, eax
00444995  jmp        0x4449df

// block 00444997
00444997  mov        edx, dword ptr [0x4ce8cc]
0044499d  mov        eax, dword ptr [edx + 0x8856b8]
004449a3  cmp        eax, ebx
004449a5  je         0x4449b5

// block 004449a7
004449a7  mov        ebp, dword ptr [eax]
004449a9  cmp        dword ptr [ebp], ecx
004449ac  je         0x4449d5

// block 004449ae
004449ae  mov        eax, dword ptr [eax + 4]
004449b1  cmp        eax, ebx
004449b3  jne        0x4449a7

// block 004449b5
004449b5  mov        eax, dword ptr [edx + 0x8856c0]
004449bb  cmp        eax, ebx
004449bd  je         0x4449d1

// block 004449bf
004449bf  nop        

// block 004449c0
004449c0  mov        edx, dword ptr [eax]
004449c2  cmp        dword ptr [edx], ecx
004449c4  je         0x444a4a

// block 004449ca
004449ca  mov        eax, dword ptr [eax + 4]
004449cd  cmp        eax, ebx
004449cf  jne        0x4449c0

// block 004449d1
004449d1  xor        eax, eax
004449d3  jmp        0x4449db

// block 004449d5
004449d5  mov        eax, ebp

// block 004449d7
004449d7  cmp        eax, ebx
004449d9  jne        0x4449e5

// block 004449db
004449db  mov        ebp, dword ptr [esp + 0x14]

// block 004449df
004449df  mov        dword ptr [ebp + 0x2d0], ebx

// block 004449e5
004449e5  lea        ecx, [eax + 0x10]
004449e8  cmp        ecx, ebx
004449ea  je         0x444a0d

// block 004449ec
004449ec  lea        esp, [esp]

// block 004449f0
004449f0  mov        edx, dword ptr [ecx]
004449f2  movsx      ebp, word ptr [edx + 0x3ea]
004449f9  cmp        ebp, esi
004449fb  je         0x444a4e

// block 004449fd
004449fd  cmp        esi, -1
00444a00  jne        0x444a06

// block 00444a02
00444a02  cmp        edx, eax
00444a04  jne        0x444a4e

// block 00444a06
00444a06  mov        ecx, dword ptr [ecx + 4]
00444a09  cmp        ecx, ebx
00444a0b  jne        0x4449f0

// block 00444a0d
00444a0d  xor        esi, esi

// block 00444a0f
00444a0f  mov        eax, dword ptr [esi + 0x494]
00444a15  cmp        eax, ebx
00444a17  je         0x444a22

// block 00444a19
00444a19  mov        edx, 0x1f
00444a1e  mov        ecx, esi
00444a20  call       eax

// block 00444a22
00444a22  mov        eax, 0x1f
00444a27  push       esi
00444a28  mov        word ptr [esi + 0x3c4], ax
00444a2f  call       0x455630

// block 00444a34
00444a34  mov        ecx, dword ptr [esp + 0x14]
00444a38  mov        ecx, dword ptr [ecx + 0x2d0]
00444a3e  lea        esi, [edi + edi + 0x3c]
00444a42  cmp        ecx, ebx
00444a44  jne        0x444a52

// block 00444a46
00444a46  xor        eax, eax
00444a48  jmp        0x444a9b

// block 00444a4a
00444a4a  mov        eax, edx
00444a4c  jmp        0x4449d7

// block 00444a4e
00444a4e  mov        esi, edx
00444a50  jmp        0x444a0f

// block 00444a52
00444a52  mov        edx, dword ptr [0x4ce8cc]
00444a58  mov        eax, dword ptr [edx + 0x8856b8]
00444a5e  cmp        eax, ebx
00444a60  je         0x444a70

// block 00444a62
00444a62  mov        ebp, dword ptr [eax]
00444a64  cmp        dword ptr [ebp], ecx
00444a67  je         0x444a91

// block 00444a69
00444a69  mov        eax, dword ptr [eax + 4]
00444a6c  cmp        eax, ebx
00444a6e  jne        0x444a62

// block 00444a70
00444a70  mov        eax, dword ptr [edx + 0x8856c0]
00444a76  cmp        eax, ebx
00444a78  je         0x444a46

// block 00444a7a
00444a7a  lea        ebx, [ebx]

// block 00444a80
00444a80  mov        edx, dword ptr [eax]
00444a82  cmp        dword ptr [edx], ecx
00444a84  je         0x444a95

// block 00444a86
00444a86  mov        eax, dword ptr [eax + 4]
00444a89  cmp        eax, ebx
00444a8b  jne        0x444a80

// block 00444a8d
00444a8d  xor        eax, eax
00444a8f  jmp        0x444a9b

// block 00444a91
00444a91  mov        eax, ebp
00444a93  jmp        0x444a97

// block 00444a95
00444a95  mov        eax, edx

// block 00444a97
00444a97  cmp        eax, ebx
00444a99  jne        0x444aa5

// block 00444a9b
00444a9b  mov        ecx, dword ptr [esp + 0x14]
00444a9f  mov        dword ptr [ecx + 0x2d0], ebx

// block 00444aa5
00444aa5  lea        ecx, [eax + 0x10]
00444aa8  cmp        ecx, ebx
00444aaa  je         0x444acd

// block 00444aac
00444aac  lea        esp, [esp]

// block 00444ab0
00444ab0  mov        edx, dword ptr [ecx]
00444ab2  movsx      ebp, word ptr [edx + 0x3ea]
00444ab9  cmp        ebp, esi
00444abb  je         0x444b0a

// block 00444abd
00444abd  cmp        esi, -1
00444ac0  jne        0x444ac6

// block 00444ac2
00444ac2  cmp        edx, eax
00444ac4  jne        0x444b0a

// block 00444ac6
00444ac6  mov        ecx, dword ptr [ecx + 4]
00444ac9  cmp        ecx, ebx
00444acb  jne        0x444ab0

// block 00444acd
00444acd  xor        esi, esi

// block 00444acf
00444acf  mov        eax, dword ptr [esi + 0x494]
00444ad5  cmp        eax, ebx
00444ad7  je         0x444ae2

// block 00444ad9
00444ad9  mov        edx, 0x1f
00444ade  mov        ecx, esi
00444ae0  call       eax

// block 00444ae2
00444ae2  mov        edx, 0x1f
00444ae7  push       esi
00444ae8  mov        word ptr [esi + 0x3c4], dx
00444aef  call       0x455630

// block 00444af4
00444af4  mov        eax, dword ptr [esp + 0x14]
00444af8  mov        ecx, dword ptr [eax + 0x2d0]
00444afe  lea        esi, [edi + edi + 0x45]
00444b02  cmp        ecx, ebx
00444b04  jne        0x444b0e

// block 00444b06
00444b06  xor        eax, eax
00444b08  jmp        0x444b53

// block 00444b0a
00444b0a  mov        esi, edx
00444b0c  jmp        0x444acf

// block 00444b0e
00444b0e  mov        edx, dword ptr [0x4ce8cc]
00444b14  mov        eax, dword ptr [edx + 0x8856b8]
00444b1a  cmp        eax, ebx
00444b1c  je         0x444b2e

// block 00444b1e
00444b1e  mov        edi, edi

// block 00444b20
00444b20  mov        ebp, dword ptr [eax]
00444b22  cmp        dword ptr [ebp], ecx
00444b25  je         0x444b49

// block 00444b27
00444b27  mov        eax, dword ptr [eax + 4]
00444b2a  cmp        eax, ebx
00444b2c  jne        0x444b20

// block 00444b2e
00444b2e  mov        eax, dword ptr [edx + 0x8856c0]
00444b34  cmp        eax, ebx
00444b36  je         0x444b06

// block 00444b38
00444b38  mov        edx, dword ptr [eax]
00444b3a  cmp        dword ptr [edx], ecx
00444b3c  je         0x444b4d

// block 00444b3e
00444b3e  mov        eax, dword ptr [eax + 4]
00444b41  cmp        eax, ebx
00444b43  jne        0x444b38

// block 00444b45
00444b45  xor        eax, eax
00444b47  jmp        0x444b53

// block 00444b49
00444b49  mov        eax, ebp
00444b4b  jmp        0x444b4f

// block 00444b4d
00444b4d  mov        eax, edx

// block 00444b4f
00444b4f  cmp        eax, ebx
00444b51  jne        0x444b5d

// block 00444b53
00444b53  mov        ecx, dword ptr [esp + 0x14]
00444b57  mov        dword ptr [ecx + 0x2d0], ebx

// block 00444b5d
00444b5d  lea        ecx, [eax + 0x10]
00444b60  cmp        ecx, ebx
00444b62  je         0x444b81

// block 00444b64
00444b64  mov        edx, dword ptr [ecx]
00444b66  movsx      ebp, word ptr [edx + 0x3ea]
00444b6d  cmp        ebp, esi
00444b6f  je         0x444bbe

// block 00444b71
00444b71  cmp        esi, -1
00444b74  jne        0x444b7a

// block 00444b76
00444b76  cmp        edx, eax
00444b78  jne        0x444bbe

// block 00444b7a
00444b7a  mov        ecx, dword ptr [ecx + 4]
00444b7d  cmp        ecx, ebx
00444b7f  jne        0x444b64

// block 00444b81
00444b81  xor        esi, esi

// block 00444b83
00444b83  mov        eax, dword ptr [esi + 0x494]
00444b89  cmp        eax, ebx
00444b8b  je         0x444b96

// block 00444b8d
00444b8d  mov        edx, 0x1f
00444b92  mov        ecx, esi
00444b94  call       eax

// block 00444b96
00444b96  mov        edx, 0x1f
00444b9b  push       esi
00444b9c  mov        word ptr [esi + 0x3c4], dx
00444ba3  call       0x455630

// block 00444ba8
00444ba8  mov        eax, dword ptr [esp + 0x14]
00444bac  mov        ecx, dword ptr [eax + 0x2d0]
00444bb2  lea        esi, [edi + edi + 0x46]
00444bb6  cmp        ecx, ebx
00444bb8  jne        0x444bc2

// block 00444bba
00444bba  xor        eax, eax
00444bbc  jmp        0x444c0b

// block 00444bbe
00444bbe  mov        esi, edx
00444bc0  jmp        0x444b83

// block 00444bc2
00444bc2  mov        edx, dword ptr [0x4ce8cc]
00444bc8  mov        eax, dword ptr [edx + 0x8856b8]
00444bce  cmp        eax, ebx
00444bd0  je         0x444be0

// block 00444bd2
00444bd2  mov        ebp, dword ptr [eax]
00444bd4  cmp        dword ptr [ebp], ecx
00444bd7  je         0x444c01

// block 00444bd9
00444bd9  mov        eax, dword ptr [eax + 4]
00444bdc  cmp        eax, ebx
00444bde  jne        0x444bd2

// block 00444be0
00444be0  mov        eax, dword ptr [edx + 0x8856c0]
00444be6  cmp        eax, ebx
00444be8  je         0x444bba

// block 00444bea
00444bea  lea        ebx, [ebx]

// block 00444bf0
00444bf0  mov        edx, dword ptr [eax]
00444bf2  cmp        dword ptr [edx], ecx
00444bf4  je         0x444c05

// block 00444bf6
00444bf6  mov        eax, dword ptr [eax + 4]
00444bf9  cmp        eax, ebx
00444bfb  jne        0x444bf0

// block 00444bfd
00444bfd  xor        eax, eax
00444bff  jmp        0x444c0b

// block 00444c01
00444c01  mov        eax, ebp
00444c03  jmp        0x444c07

// block 00444c05
00444c05  mov        eax, edx

// block 00444c07
00444c07  cmp        eax, ebx
00444c09  jne        0x444c15

// block 00444c0b
00444c0b  mov        ecx, dword ptr [esp + 0x14]
00444c0f  mov        dword ptr [ecx + 0x2d0], ebx

// block 00444c15
00444c15  lea        ecx, [eax + 0x10]
00444c18  cmp        ecx, ebx
00444c1a  je         0x444c3d

// block 00444c1c
00444c1c  lea        esp, [esp]

// block 00444c20
00444c20  mov        edx, dword ptr [ecx]
00444c22  movsx      ebp, word ptr [edx + 0x3ea]
00444c29  cmp        ebp, esi
00444c2b  je         0x444c75

// block 00444c2d
00444c2d  cmp        esi, -1
00444c30  jne        0x444c36

// block 00444c32
00444c32  cmp        edx, eax
00444c34  jne        0x444c75

// block 00444c36
00444c36  mov        ecx, dword ptr [ecx + 4]
00444c39  cmp        ecx, ebx
00444c3b  jne        0x444c20

// block 00444c3d
00444c3d  xor        esi, esi

// block 00444c3f
00444c3f  mov        eax, dword ptr [esi + 0x494]
00444c45  cmp        eax, ebx
00444c47  je         0x444c52

// block 00444c49
00444c49  mov        edx, 0x1f
00444c4e  mov        ecx, esi
00444c50  call       eax

// block 00444c52
00444c52  mov        edx, 0x1f
00444c57  push       esi
00444c58  mov        word ptr [esi + 0x3c4], dx
00444c5f  call       0x455630

// block 00444c64
00444c64  inc        edi
00444c65  cmp        edi, 5
00444c68  jl         0x444981

// block 00444c6e
00444c6e  pop        edi
00444c6f  pop        esi
00444c70  pop        ebp
00444c71  pop        ebx
00444c72  ret        4

// block 00444c75
00444c75  mov        esi, edx
00444c77  jmp        0x444c3f
