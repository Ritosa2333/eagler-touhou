
// block 004480c0
004480c0  push       ecx
004480c1  push       edi
004480c2  mov        edi, eax
004480c4  mov        eax, dword ptr [edi + 0x24]
004480c7  cmp        eax, 3
004480ca  ja         0x448674

// block 004480d0
004480d0  push       ebx
004480d1  push       esi
004480d2  jmp        dword ptr [eax*4 + 0x44867c]

// block 004480d9
004480d9  push       0x4a1034
004480de  push       0
004480e0  mov        dword ptr [edi + 0x30], 0x1e
004480e7  call       0x4300d0

// block 004480ec
004480ec  push       0x11
004480ee  push       0
004480f0  call       0x430150

// block 004480f5
004480f5  cmp        dword ptr [edi + 0x66c], 0
004480fc  jne        0x44812a

// block 004480fe
004480fe  mov        ecx, dword ptr [0x4b43b8]
00448104  mov        edx, dword ptr [ecx + 0x18fb4]
0044810a  push       0
0044810c  push       0x14
0044810e  lea        eax, [esp + 0x14]
00448112  push       eax
00448113  push       edx
00448114  mov        eax, 0x17
00448119  xor        ecx, ecx
0044811b  call       0x4615a0

// block 00448120
00448120  mov        eax, dword ptr [esp + 0xc]
00448124  mov        dword ptr [edi + 0x66c], eax

// block 0044812a
0044812a  mov        edx, dword ptr [edi + 0x14]
0044812d  push       0
0044812f  push       0x74
00448131  lea        ecx, [esp + 0x14]
00448135  push       ecx
00448136  push       edx
00448137  mov        eax, 0x17
0044813c  xor        ecx, ecx
0044813e  call       0x4615a0

// block 00448143
00448143  mov        eax, dword ptr [esp + 0xc]
00448147  mov        dword ptr [edi + 0x498], eax
0044814d  mov        ecx, 1
00448152  mov        eax, edi
00448154  call       0x43ef40

// block 00448159
00448159  mov        esi, dword ptr [0x4b0c90]
0044815f  mov        edx, dword ptr [edi + 0x14]
00448162  push       0
00448164  add        esi, 0xc1
0044816a  push       esi
0044816b  lea        ecx, [esp + 0x14]
0044816f  push       ecx
00448170  push       edx
00448171  mov        eax, 0x17
00448176  xor        ecx, ecx
00448178  call       0x4615a0

// block 0044817d
0044817d  mov        eax, dword ptr [esp + 0xc]
00448181  mov        dword ptr [edi + esi*4 + 0x2c8], eax
00448188  mov        esi, dword ptr [0x4b0c94]
0044818e  mov        edx, dword ptr [edi + 0x14]
00448191  push       0
00448193  add        esi, 0xc4
00448199  push       esi
0044819a  lea        ecx, [esp + 0x14]
0044819e  push       ecx
0044819f  push       edx
004481a0  mov        eax, 0x17
004481a5  xor        ecx, ecx
004481a7  call       0x4615a0

// block 004481ac
004481ac  mov        eax, dword ptr [esp + 0xc]
004481b0  mov        dword ptr [edi + esi*4 + 0x2c8], eax
004481b7  mov        esi, dword ptr [0x4b0ca8]
004481bd  mov        edx, dword ptr [edi + 0x14]
004481c0  push       0
004481c2  add        esi, 0xc6
004481c8  push       esi
004481c9  lea        ecx, [esp + 0x14]
004481cd  push       ecx
004481ce  push       edx
004481cf  mov        eax, 0x17
004481d4  xor        ecx, ecx
004481d6  call       0x4615a0

// block 004481db
004481db  mov        eax, dword ptr [esp + 0xc]
004481df  mov        edx, dword ptr [0x4ce8cc]
004481e5  mov        dword ptr [edi + esi*4 + 0x2c8], eax
004481ec  mov        ecx, dword ptr [edi + 0x454]
004481f2  push       ecx
004481f3  call       0x461920

// block 004481f8
004481f8  test       eax, eax
004481fa  jne        0x448213

// block 004481fc
004481fc  lea        esi, [eax + 0x63]
004481ff  call       0x43efa0

// block 00448204
00448204  mov        edx, dword ptr [edi + 0x454]
0044820a  push       edx
0044820b  lea        ebx, [esi - 0x60]
0044820e  call       0x4619e0

// block 00448213
00448213  mov        ecx, dword ptr [0x4b0c94]
00448219  mov        eax, 8
0044821e  mov        dword ptr [0x4b0cb0], eax
00448223  mov        dword ptr [0x4b0cb4], eax
00448228  mov        eax, dword ptr [0x4b0c90]
0044822d  lea        edx, [ecx + eax*2]
00448230  mov        eax, dword ptr [0x4b451c]
00448235  imul       edx, edx, 0x45f4
0044823b  lea        eax, [edx + eax + 8]
0044823f  push       eax
00448240  mov        dword ptr [0x4b452c], 0x4aedf0
0044824a  call       0x431b90

// block 0044824f
0044824f  mov        esi, eax
00448251  mov        dword ptr [0x4b452c], 0x4aebf0
0044825b  mov        dword ptr [0x4b0cb0], 0
00448265  mov        dword ptr [0x4b0cb4], 0
0044826f  test       esi, esi
00448271  jl         0x448351

// block 00448277
00448277  push       0
00448279  lea        eax, [edi + 0x2b4]
0044827f  call       0x4067e0

// block 00448284
00448284  lea        edx, [edi + 0x28]
00448287  mov        ecx, esi
00448289  mov        dword ptr [edi + 0xf8], 1
00448293  call       0x40f790

// block 00448298
00448298  mov        eax, dword ptr [edi + 0x5998]
0044829e  lea        ebx, [edi + 0x5990]
004482a4  test       eax, eax
004482a6  je         0x4482b5

// block 004482a8
004482a8  jg         0x4482af

// block 004482aa
004482aa  dec        eax
004482ab  mov        dword ptr [ebx], eax
004482ad  jmp        0x4482bb

// block 004482af
004482af  xor        eax, eax
004482b1  mov        dword ptr [ebx], eax
004482b3  jmp        0x4482bb

// block 004482b5
004482b5  mov        dword ptr [ebx], 0

// block 004482bb
004482bb  mov        ecx, dword ptr [0x4b451c]
004482c1  lea        esi, [edi + 0x5978]
004482c7  add        ecx, 0x1e9c0
004482cd  mov        eax, esi
004482cf  mov        dword ptr [edi + 0x5998], 0x5b
004482d9  mov        dword ptr [edi + 0x5a60], 1
004482e3  sub        eax, ecx

// block 004482e5
004482e5  mov        dl, byte ptr [ecx]
004482e7  mov        byte ptr [eax + ecx], dl
004482ea  inc        ecx
004482eb  test       dl, dl
004482ed  jne        0x4482e5

// block 004482ef
004482ef  mov        edx, 0x4a0ee4
004482f4  mov        ecx, esi

// block 004482f6
004482f6  mov        al, byte ptr [ecx]
004482f8  cmp        al, byte ptr [edx]
004482fa  jne        0x448316

// block 004482fc
004482fc  test       al, al
004482fe  je         0x448312

// block 00448300
00448300  mov        al, byte ptr [ecx + 1]
00448303  cmp        al, byte ptr [edx + 1]
00448306  jne        0x448316

// block 00448308
00448308  add        ecx, 2
0044830b  add        edx, 2
0044830e  test       al, al
00448310  jne        0x4482f6

// block 00448312
00448312  xor        eax, eax
00448314  jmp        0x44831b

// block 00448316
00448316  sbb        eax, eax
00448318  sbb        eax, -1

// block 0044831b
0044831b  test       eax, eax
0044831d  je         0x448328

// block 0044831f
0044831f  push       -1
00448321  mov        eax, ebx
00448323  call       0x464970

// block 00448328
00448328  mov        eax, 8
0044832d  lea        ecx, [ecx]

// block 00448330
00448330  cmp        byte ptr [edi + eax + 0x5977], 0x20
00448338  jne        0x44833f

// block 0044833a
0044833a  dec        eax
0044833b  test       eax, eax
0044833d  jg         0x448330

// block 0044833f
0044833f  mov        dword ptr [edi + 0x5984], eax
00448345  mov        dword ptr [edi + 0x5988], 0
0044834f  jmp        0x44837b

// block 00448351
00448351  mov        eax, dword ptr [edi + 0x30]
00448354  test       eax, eax
00448356  je         0x44836a

// block 00448358
00448358  cmp        eax, -1
0044835b  jg         0x448363

// block 0044835d
0044835d  dec        eax
0044835e  mov        dword ptr [edi + 0x28], eax
00448361  jmp        0x448371

// block 00448363
00448363  xor        eax, eax
00448365  mov        dword ptr [edi + 0x28], eax
00448368  jmp        0x448371

// block 0044836a
0044836a  mov        dword ptr [edi + 0x28], 0xffffffff

// block 00448371
00448371  mov        dword ptr [edi + 0x5988], 1

// block 0044837b
0044837b  cmp        dword ptr [edi + 0x2b8], 6
00448382  jle        0x448672

// block 00448388
00448388  mov        ecx, 2
0044838d  mov        eax, edi
0044838f  call       0x43ef40

// block 00448394
00448394  pop        esi
00448395  pop        ebx
00448396  mov        eax, 1
0044839b  pop        edi
0044839c  pop        ecx
0044839d  ret        

// block 0044839e
0044839e  cmp        dword ptr [edi + 0x5988], 0
004483a5  jne        0x448476

// block 004483ab
004483ab  mov        ecx, dword ptr [edi + 0x5990]
004483b1  lea        esi, [edi + 0x5990]
004483b7  mov        al, 0x10
004483b9  mov        dword ptr [esi + 4], ecx
004483bc  test       byte ptr [0x4d48c4], al
004483c2  jne        0x4483cc

// block 004483c4
004483c4  test       byte ptr [0x4d48c0], al
004483ca  je         0x4483d5

// block 004483cc
004483cc  push       -0xd
004483ce  mov        eax, esi
004483d0  call       0x464970

// block 004483d5
004483d5  test       byte ptr [0x4d48c4], 0x20
004483dc  jne        0x4483e7

// block 004483de
004483de  test       byte ptr [0x4d48c0], 0x20
004483e5  je         0x4483f0

// block 004483e7
004483e7  push       0xd
004483e9  mov        eax, esi
004483eb  call       0x464970

// block 004483f0
004483f0  mov        al, 0x40
004483f2  test       byte ptr [0x4d48c4], al
004483f8  jne        0x448402

// block 004483fa
004483fa  test       byte ptr [0x4d48c0], al
00448400  je         0x448429

// block 00448402
00448402  mov        ecx, dword ptr [esi]
00448404  mov        eax, 0x4ec4ec4f
00448409  imul       ecx
0044840b  sar        edx, 2
0044840e  mov        eax, edx
00448410  shr        eax, 0x1f
00448413  add        eax, edx
00448415  imul       eax, eax, 0xd
00448418  sub        ecx, eax
0044841a  mov        eax, esi
0044841c  je         0x448422

// block 0044841e
0044841e  push       -1
00448420  jmp        0x448424

// block 00448422
00448422  push       0xc

// block 00448424
00448424  call       0x464970

// block 00448429
00448429  mov        al, 0x80
0044842b  test       byte ptr [0x4d48c4], al
00448431  jne        0x44843b

// block 00448433
00448433  test       byte ptr [0x4d48c0], al
00448439  je         0x448465

// block 0044843b
0044843b  mov        ecx, dword ptr [esi]
0044843d  mov        eax, 0x4ec4ec4f
00448442  imul       ecx
00448444  sar        edx, 2
00448447  mov        eax, edx
00448449  shr        eax, 0x1f
0044844c  add        eax, edx
0044844e  imul       eax, eax, 0xd
00448451  sub        ecx, eax
00448453  mov        eax, esi
00448455  cmp        ecx, 0xc
00448458  je         0x44845e

// block 0044845a
0044845a  push       1
0044845c  jmp        0x448460

// block 0044845e
0044845e  push       -0xc

// block 00448460
00448460  call       0x464970

// block 00448465
00448465  mov        ecx, dword ptr [esi + 4]
00448468  cmp        ecx, dword ptr [esi]
0044846a  je         0x448476

// block 0044846c
0044846c  mov        edx, 0xa
00448471  call       0x453d90

// block 00448476
00448476  test       dword ptr [0x4d48c4], 0x80001
00448480  je         0x4485b2

// block 00448486
00448486  cmp        dword ptr [edi + 0x5988], 0
0044848d  jne        0x44859c

// block 00448493
00448493  mov        eax, dword ptr [edi + 0x5990]
00448499  cmp        eax, 0x58
0044849c  lea        edx, [edi + 0x5990]
004484a2  jge        0x4484f0

// block 004484a4
004484a4  mov        ecx, dword ptr [edi + 0x5984]
004484aa  cmp        ecx, 8
004484ad  jge        0x4484de

// block 004484af
004484af  mov        al, byte ptr [eax + 0x4a0e88]
004484b5  mov        byte ptr [ecx + edi + 0x5978], al

// block 004484bc
004484bc  inc        dword ptr [edi + 0x5984]
004484c2  cmp        dword ptr [edi + 0x5984], 8
004484c9  jl         0x4485a8

// block 004484cf
004484cf  mov        ecx, 0x5a
004484d4  call       0x40f790

// block 004484d9
004484d9  jmp        0x4485a8

// block 004484de
004484de  mov        dl, byte ptr [eax + 0x4a0e88]
004484e4  mov        byte ptr [ecx + edi + 0x5977], dl
004484eb  jmp        0x4485a8

// block 004484f0
004484f0  jne        0x448514

// block 004484f2
004484f2  mov        eax, dword ptr [edi + 0x5984]
004484f8  cmp        eax, 8
004484fb  jge        0x448507

// block 004484fd
004484fd  mov        byte ptr [eax + edi + 0x5978], 0x20
00448505  jmp        0x4484bc

// block 00448507
00448507  mov        byte ptr [eax + edi + 0x5977], 0x20
0044850f  jmp        0x4485a8

// block 00448514
00448514  cmp        eax, 0x59
00448517  jne        0x448538

// block 00448519
00448519  mov        eax, dword ptr [edi + 0x5984]
0044851f  test       eax, eax
00448521  je         0x448672

// block 00448527
00448527  dec        eax
00448528  mov        dword ptr [edi + 0x5984], eax
0044852e  mov        byte ptr [eax + edi + 0x5978], 0x20
00448536  jmp        0x4485a8

// block 00448538
00448538  cmp        eax, 0x5a
0044853b  jne        0x4485a8

// block 0044853d
0044853d  mov        ecx, dword ptr [0x4b0ca8]
00448543  mov        esi, dword ptr [edi + 0x28]
00448546  mov        ebx, dword ptr [0x4b0c94]
0044854c  lea        ecx, [ecx + ecx*4]
0044854f  lea        ecx, [esi + ecx*2]
00448552  lea        esi, [ecx*8]
00448559  sub        esi, ecx
0044855b  mov        ecx, dword ptr [0x4b0c90]
00448561  lea        ecx, [ebx + ecx*2]
00448564  mov        ebx, dword ptr [0x4b451c]
0044856a  imul       ecx, ecx, 0x45f4
00448570  lea        eax, [edi + 0x5978]
00448576  add        ecx, ebx
00448578  mov        edx, eax
0044857a  lea        esi, [ecx + esi*4 + 0x1e]
0044857e  mov        edi, edi

// block 00448580
00448580  mov        cl, byte ptr [edx]
00448582  mov        byte ptr [esi], cl
00448584  inc        edx
00448585  inc        esi
00448586  test       cl, cl
00448588  jne        0x448580

// block 0044858a
0044858a  lea        edx, [ebx + 0x1e9c0]
00448590  sub        edx, eax

// block 00448592
00448592  mov        cl, byte ptr [eax]
00448594  mov        byte ptr [edx + eax], cl
00448597  inc        eax
00448598  test       cl, cl
0044859a  jne        0x448592

// block 0044859c
0044859c  mov        ecx, 3
004485a1  mov        eax, edi
004485a3  call       0x43ef40

// block 004485a8
004485a8  mov        edx, 7
004485ad  call       0x453d90

// block 004485b2
004485b2  test       dword ptr [0x4d48c4], 0x102
004485bc  je         0x448672

// block 004485c2
004485c2  cmp        dword ptr [edi + 0x5988], 0
004485c9  jne        0x448600

// block 004485cb
004485cb  cmp        dword ptr [edi + 0x5984], 0
004485d2  je         0x448672

// block 004485d8
004485d8  mov        edx, 9
004485dd  call       0x453d90

// block 004485e2
004485e2  dec        dword ptr [edi + 0x5984]
004485e8  mov        eax, dword ptr [edi + 0x5984]
004485ee  pop        esi
004485ef  mov        byte ptr [eax + edi + 0x5978], 0x20
004485f7  pop        ebx
004485f8  mov        eax, 1
004485fd  pop        edi
004485fe  pop        ecx
004485ff  ret        

// block 00448600
00448600  mov        ecx, 3
00448605  mov        eax, edi
00448607  call       0x43ef40

// block 0044860c
0044860c  mov        edx, 7
00448611  call       0x453d90

// block 00448616
00448616  pop        esi
00448617  pop        ebx
00448618  mov        eax, 1
0044861d  pop        edi
0044861e  pop        ecx
0044861f  ret        

// block 00448620
00448620  cmp        dword ptr [edi + 0x2b8], 6
00448627  jl         0x448672

// block 00448629
00448629  mov        esi, 0x74
0044862e  call       0x43efd0

// block 00448633
00448633  mov        esi, dword ptr [0x4b0c90]
00448639  add        esi, 0xc1
0044863f  call       0x43efd0

// block 00448644
00448644  mov        esi, dword ptr [0x4b0c94]
0044864a  add        esi, 0xc4
00448650  call       0x43efd0

// block 00448655
00448655  mov        esi, dword ptr [0x4b0ca8]
0044865b  add        esi, 0xc6
00448661  call       0x43efd0

// block 00448666
00448666  mov        edx, 0xf
0044866b  mov        eax, edi
0044866d  call       0x43eee0

// block 00448672
00448672  pop        esi
00448673  pop        ebx

// block 00448674
00448674  mov        eax, 1
00448679  pop        edi
0044867a  pop        ecx
0044867b  ret        
