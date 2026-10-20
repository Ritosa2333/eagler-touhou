
// block 004452d0
004452d0  push       ebp
004452d1  mov        ebp, dword ptr [esp + 8]
004452d5  mov        eax, dword ptr [ebp + 0x24]
004452d8  cmp        eax, 4
004452db  ja         0x445a14

// block 004452e1
004452e1  push       ebx
004452e2  push       esi
004452e3  push       edi
004452e4  jmp        dword ptr [eax*4 + 0x445a20]

// block 004452eb
004452eb  mov        dword ptr [ebp + 0x30], 3
004452f2  xor        edi, edi
004452f4  cmp        dword ptr [0x4b0ca8], 4
004452fb  jne        0x44543f

// block 00445301
00445301  mov        ecx, dword ptr [0x4b451c]
00445307  mov        dl, 0x10
00445309  test       byte ptr [ecx + 0x1e9d0], dl
0044530f  jne        0x445351

// block 00445311
00445311  test       byte ptr [ecx + 0x1e9d1], dl
00445317  jne        0x445351

// block 00445319
00445319  lea        esi, [edi + 1]
0044531c  cmp        dword ptr [ebp + 0x28], edi
0044531f  jne        0x44533c

// block 00445321
00445321  mov        eax, dword ptr [ebp + 0x30]
00445324  cmp        eax, edi
00445326  je         0x445339

// block 00445328
00445328  cmp        eax, esi
0044532a  jg         0x445332

// block 0044532c
0044532c  dec        eax
0044532d  mov        dword ptr [ebp + 0x28], eax
00445330  jmp        0x44533c

// block 00445332
00445332  mov        eax, esi
00445334  mov        dword ptr [ebp + 0x28], eax
00445337  jmp        0x44533c

// block 00445339
00445339  mov        dword ptr [ebp + 0x28], esi

// block 0044533c
0044533c  mov        eax, dword ptr [ebp + 0xfc]
00445342  mov        dword ptr [ebp + eax*4 + 0xb8], edi
00445349  add        dword ptr [ebp + 0xfc], esi
0044534f  jmp        0x445356

// block 00445351
00445351  mov        esi, 1

// block 00445356
00445356  test       byte ptr [ecx + 0x1e9d2], dl
0044535c  jne        0x4453cc

// block 0044535e
0044535e  test       byte ptr [ecx + 0x1e9d3], dl
00445364  jne        0x4453cc

// block 00445366
00445366  cmp        dword ptr [ebp + 0x28], esi
00445369  jne        0x4453b9

// block 0044536b
0044536b  test       byte ptr [ecx + 0x1e9d0], dl
00445371  jne        0x4453a0

// block 00445373
00445373  test       byte ptr [ecx + 0x1e9d1], dl
00445379  jne        0x4453a0

// block 0044537b
0044537b  mov        eax, dword ptr [ebp + 0x30]
0044537e  cmp        eax, edi
00445380  je         0x445397

// block 00445382
00445382  cmp        eax, 2
00445385  jg         0x44538d

// block 00445387
00445387  dec        eax
00445388  mov        dword ptr [ebp + 0x28], eax
0044538b  jmp        0x4453b9

// block 0044538d
0044538d  mov        eax, 2
00445392  mov        dword ptr [ebp + 0x28], eax
00445395  jmp        0x4453b9

// block 00445397
00445397  mov        dword ptr [ebp + 0x28], 2
0044539e  jmp        0x4453b9

// block 004453a0
004453a0  mov        eax, dword ptr [ebp + 0x30]
004453a3  cmp        eax, edi
004453a5  je         0x4453b6

// block 004453a7
004453a7  jg         0x4453af

// block 004453a9
004453a9  dec        eax
004453aa  mov        dword ptr [ebp + 0x28], eax
004453ad  jmp        0x4453b9

// block 004453af
004453af  xor        eax, eax
004453b1  mov        dword ptr [ebp + 0x28], eax
004453b4  jmp        0x4453b9

// block 004453b6
004453b6  mov        dword ptr [ebp + 0x28], edi

// block 004453b9
004453b9  mov        eax, dword ptr [ebp + 0xfc]
004453bf  mov        dword ptr [ebp + eax*4 + 0xb8], esi
004453c6  add        dword ptr [ebp + 0xfc], esi

// block 004453cc
004453cc  test       byte ptr [ecx + 0x1e9d4], dl
004453d2  jne        0x44543f

// block 004453d4
004453d4  test       byte ptr [ecx + 0x1e9d5], dl
004453da  jne        0x44543f

// block 004453dc
004453dc  mov        ebx, 2
004453e1  cmp        dword ptr [ebp + 0x28], ebx
004453e4  jne        0x44542c

// block 004453e6
004453e6  test       byte ptr [ecx + 0x1e9d0], dl
004453ec  jne        0x445413

// block 004453ee
004453ee  test       byte ptr [ecx + 0x1e9d1], dl
004453f4  jne        0x445413

// block 004453f6
004453f6  mov        eax, dword ptr [ebp + 0x30]
004453f9  cmp        eax, edi
004453fb  je         0x44540e

// block 004453fd
004453fd  cmp        eax, esi
004453ff  jg         0x445407

// block 00445401
00445401  dec        eax
00445402  mov        dword ptr [ebp + 0x28], eax
00445405  jmp        0x44542c

// block 00445407
00445407  mov        eax, esi
00445409  mov        dword ptr [ebp + 0x28], eax
0044540c  jmp        0x44542c

// block 0044540e
0044540e  mov        dword ptr [ebp + 0x28], esi
00445411  jmp        0x44542c

// block 00445413
00445413  mov        eax, dword ptr [ebp + 0x30]
00445416  cmp        eax, edi
00445418  je         0x445429

// block 0044541a
0044541a  jg         0x445422

// block 0044541c
0044541c  dec        eax
0044541d  mov        dword ptr [ebp + 0x28], eax
00445420  jmp        0x44542c

// block 00445422
00445422  xor        eax, eax
00445424  mov        dword ptr [ebp + 0x28], eax
00445427  jmp        0x44542c

// block 00445429
00445429  mov        dword ptr [ebp + 0x28], edi

// block 0044542c
0044542c  mov        ecx, dword ptr [ebp + 0xfc]
00445432  mov        dword ptr [ebp + ecx*4 + 0xb8], ebx
00445439  add        dword ptr [ebp + 0xfc], esi

// block 0044543f
0044543f  mov        eax, dword ptr [ebp + 0x14]
00445442  push       edi
00445443  push       0x6f
00445445  lea        edx, [esp + 0x1c]
00445449  push       edx
0044544a  push       eax
0044544b  mov        eax, 0x17
00445450  xor        ecx, ecx
00445452  call       0x4615a0

// block 00445457
00445457  mov        edx, dword ptr [ebp + 0x504]
0044545d  mov        ecx, dword ptr [esp + 0x14]
00445461  push       edx
00445462  mov        ebx, 1
00445467  mov        dword ptr [ebp + 0x484], ecx
0044546d  call       0x461970

// block 00445472
00445472  push       edi
00445473  push       0x8f
00445478  lea        eax, [esp + 0x1c]
0044547c  mov        dword ptr [ebp + 0x504], edi
00445482  mov        ecx, dword ptr [ebp + 0x14]
00445485  push       eax
00445486  push       ecx
00445487  lea        eax, [ebx + 0x16]
0044548a  xor        ecx, ecx
0044548c  call       0x4615a0

// block 00445491
00445491  mov        eax, dword ptr [esp + 0x14]
00445495  push       eax
00445496  mov        ebx, 3
0044549b  mov        dword ptr [ebp + 0x504], eax
004454a1  call       0x4619e0

// block 004454a6
004454a6  movzx      ebx, word ptr [ebp + 0x28]
004454aa  mov        edx, dword ptr [ebp + 0x504]
004454b0  add        bx, 0x11
004454b4  push       edx
004454b5  call       0x461970

// block 004454ba
004454ba  mov        ecx, 1
004454bf  mov        eax, ebp
004454c1  call       0x43ef40

// block 004454c6
004454c6  mov        esi, dword ptr [0x4b451c]
004454cc  mov        eax, dword ptr [0x4b0ca8]
004454d1  mov        ebx, dword ptr [0x4ce8cc]
004454d7  cmp        dword ptr [esi + eax*4 + 0x598], edi
004454de  je         0x4454e9

// block 004454e0
004454e0  cmp        dword ptr [esi + eax*4 + 0x4b8c], edi
004454e7  jne        0x44553d

// block 004454e9
004454e9  mov        eax, dword ptr [ebp + 0x504]
004454ef  push       eax
004454f0  mov        edx, ebx
004454f2  call       0x461920

// block 004454f7
004454f7  cmp        eax, edi
004454f9  jne        0x445501

// block 004454fb
004454fb  mov        dword ptr [ebp + 0x504], edi

// block 00445501
00445501  add        eax, 0x10
00445504  cmp        eax, edi
00445506  je         0x44552b

// block 00445508
00445508  jmp        0x445510

// block 00445510
00445510  mov        ecx, dword ptr [eax]
00445512  mov        edx, 0x88
00445517  cmp        word ptr [ecx + 0x3ea], dx
0044551e  je         0x445619

// block 00445524
00445524  mov        eax, dword ptr [eax + 4]
00445527  cmp        eax, edi
00445529  jne        0x445510

// block 0044552b
0044552b  mov        dword ptr [esp + 0x14], edi

// block 0044552f
0044552f  lea        eax, [esp + 0x14]
00445533  call       0x461d80

// block 00445538
00445538  mov        eax, dword ptr [0x4b0ca8]

// block 0044553d
0044553d  cmp        dword ptr [esi + eax*4 + 0x9180], edi
00445544  je         0x44554f

// block 00445546
00445546  cmp        dword ptr [esi + eax*4 + 0xd774], edi
0044554d  jne        0x44559d

// block 0044554f
0044554f  mov        edx, dword ptr [ebp + 0x504]
00445555  push       edx
00445556  mov        edx, ebx
00445558  call       0x461920

// block 0044555d
0044555d  cmp        eax, edi
0044555f  jne        0x445567

// block 00445561
00445561  mov        dword ptr [ebp + 0x504], edi

// block 00445567
00445567  add        eax, 0x10
0044556a  cmp        eax, edi
0044556c  je         0x44558b

// block 0044556e
0044556e  mov        edi, edi

// block 00445570
00445570  mov        ecx, dword ptr [eax]
00445572  mov        edx, 0x89
00445577  cmp        word ptr [ecx + 0x3ea], dx
0044557e  je         0x445626

// block 00445584
00445584  mov        eax, dword ptr [eax + 4]
00445587  cmp        eax, edi
00445589  jne        0x445570

// block 0044558b
0044558b  mov        dword ptr [esp + 0x14], edi

// block 0044558f
0044558f  lea        eax, [esp + 0x14]
00445593  call       0x461d80

// block 00445598
00445598  mov        eax, dword ptr [0x4b0ca8]

// block 0044559d
0044559d  cmp        dword ptr [esi + eax*4 + 0x11d68], edi
004455a4  je         0x4455af

// block 004455a6
004455a6  cmp        dword ptr [esi + eax*4 + 0x1635c], edi
004455ad  jne        0x4455f4

// block 004455af
004455af  mov        edx, dword ptr [ebp + 0x504]
004455b5  push       edx
004455b6  mov        edx, ebx
004455b8  call       0x461920

// block 004455bd
004455bd  cmp        eax, edi
004455bf  jne        0x4455c7

// block 004455c1
004455c1  mov        dword ptr [ebp + 0x504], edi

// block 004455c7
004455c7  add        eax, 0x10
004455ca  cmp        eax, edi
004455cc  je         0x4455e7

// block 004455ce
004455ce  mov        edi, edi

// block 004455d0
004455d0  mov        ecx, dword ptr [eax]
004455d2  mov        edx, 0x8a
004455d7  cmp        word ptr [ecx + 0x3ea], dx
004455de  je         0x445633

// block 004455e0
004455e0  mov        eax, dword ptr [eax + 4]
004455e3  cmp        eax, edi
004455e5  jne        0x4455d0

// block 004455e7
004455e7  mov        dword ptr [esp + 0x14], edi

// block 004455eb
004455eb  lea        eax, [esp + 0x14]
004455ef  call       0x461d80

// block 004455f4
004455f4  cmp        dword ptr [ebp + 0x2b8], 6
004455fb  jle        0x445a11

// block 00445601
00445601  mov        ecx, 2
00445606  mov        eax, ebp
00445608  call       0x43ef40

// block 0044560d
0044560d  pop        edi
0044560e  pop        esi
0044560f  pop        ebx
00445610  mov        eax, 1
00445615  pop        ebp
00445616  ret        4

// block 00445619
00445619  mov        eax, ecx
0044561b  mov        ecx, dword ptr [eax]
0044561d  mov        dword ptr [esp + 0x14], ecx
00445621  jmp        0x44552f

// block 00445626
00445626  mov        eax, ecx
00445628  mov        ecx, dword ptr [eax]
0044562a  mov        dword ptr [esp + 0x14], ecx
0044562e  jmp        0x44558f

// block 00445633
00445633  mov        eax, ecx
00445635  mov        ecx, dword ptr [eax]
00445637  mov        dword ptr [esp + 0x14], ecx
0044563b  jmp        0x4455eb

// block 0044563d
0044563d  mov        edx, dword ptr [ebp + 0x28]
00445640  lea        esi, [ebp + 0x28]
00445643  mov        dword ptr [esi + 4], edx
00445646  mov        dl, 0x10
00445648  test       byte ptr [0x4d48c4], dl
0044564e  jne        0x445658

// block 00445650
00445650  test       byte ptr [0x4d48c0], dl
00445656  je         0x445661

// block 00445658
00445658  push       -1
0044565a  mov        eax, esi
0044565c  call       0x464970

// block 00445661
00445661  mov        al, 0x20
00445663  test       byte ptr [0x4d48c4], al
00445669  jne        0x445673

// block 0044566b
0044566b  test       byte ptr [0x4d48c0], al
00445671  je         0x44567c

// block 00445673
00445673  push       1
00445675  mov        eax, esi
00445677  call       0x464970

// block 0044567c
0044567c  mov        eax, dword ptr [esi + 4]
0044567f  cmp        eax, dword ptr [esi]
00445681  je         0x4456b1

// block 00445683
00445683  mov        edx, 0xa
00445688  call       0x453d90

// block 0044568d
0044568d  mov        ecx, dword ptr [ebp + 0x504]
00445693  push       ecx
00445694  mov        ebx, 3
00445699  call       0x4619e0

// block 0044569e
0044569e  movzx      ebx, word ptr [esi]
004456a1  mov        edx, dword ptr [ebp + 0x504]
004456a7  add        bx, 7
004456ab  push       edx
004456ac  call       0x461970

// block 004456b1
004456b1  mov        eax, dword ptr [0x4d48c4]
004456b6  test       eax, 0x102
004456bb  je         0x4456df

// block 004456bd
004456bd  mov        ecx, 4
004456c2  mov        eax, ebp
004456c4  call       0x43ef40

// block 004456c9
004456c9  mov        edx, 9
004456ce  call       0x453d90

// block 004456d3
004456d3  pop        edi
004456d4  pop        esi
004456d5  pop        ebx
004456d6  mov        eax, 1
004456db  pop        ebp
004456dc  ret        4

// block 004456df
004456df  test       eax, 0x80001
004456e4  je         0x445a11

// block 004456ea
004456ea  mov        edi, dword ptr [ebp + 0x28]
004456ed  lea        esi, [ebp + 0x504]
004456f3  add        edi, 0x8c
004456f9  lea        ebx, [esp + 0x14]
004456fd  call       0x462060

// block 00445702
00445702  mov        eax, dword ptr [esp + 0x14]
00445706  push       eax
00445707  mov        ebx, 6
0044570c  call       0x461970

// block 00445711
00445711  mov        edi, dword ptr [ebp + 0x28]
00445714  add        edi, 0x85
0044571a  lea        ebx, [esp + 0x14]
0044571e  call       0x462060

// block 00445723
00445723  mov        ecx, dword ptr [esp + 0x14]
00445727  push       ecx
00445728  mov        ebx, 6
0044572d  call       0x461970

// block 00445732
00445732  mov        edx, dword ptr [esi]
00445734  push       edx
00445735  mov        edx, dword ptr [0x4ce8cc]
0044573b  call       0x461920

// block 00445740
00445740  test       eax, eax
00445742  jne        0x445746

// block 00445744
00445744  mov        dword ptr [esi], eax

// block 00445746
00445746  add        eax, 0x10
00445749  je         0x445766

// block 0044574b
0044574b  mov        ecx, 0x5f

// block 00445750
00445750  mov        edx, dword ptr [eax]
00445752  cmp        word ptr [edx + 0x3ea], cx
00445759  je         0x4458e1

// block 0044575f
0044575f  mov        eax, dword ptr [eax + 4]
00445762  test       eax, eax
00445764  jne        0x445750

// block 00445766
00445766  xor        eax, eax

// block 00445768
00445768  push       eax
00445769  call       0x461970

// block 0044576e
0044576e  mov        ecx, dword ptr [esi]
00445770  mov        edx, dword ptr [0x4ce8cc]
00445776  push       ecx
00445777  call       0x461920

// block 0044577c
0044577c  test       eax, eax
0044577e  jne        0x445782

// block 00445780
00445780  mov        dword ptr [esi], eax

// block 00445782
00445782  add        eax, 0x10
00445785  je         0x4457a6

// block 00445787
00445787  mov        ecx, 0x60
0044578c  lea        esp, [esp]

// block 00445790
00445790  mov        edx, dword ptr [eax]
00445792  cmp        word ptr [edx + 0x3ea], cx
00445799  je         0x4458ea

// block 0044579f
0044579f  mov        eax, dword ptr [eax + 4]
004457a2  test       eax, eax
004457a4  jne        0x445790

// block 004457a6
004457a6  xor        eax, eax

// block 004457a8
004457a8  push       eax
004457a9  call       0x461970

// block 004457ae
004457ae  mov        eax, dword ptr [ebp + 0x28]
004457b1  inc        eax
004457b2  cdq        
004457b3  mov        ecx, 3
004457b8  idiv       ecx
004457ba  lea        ebx, [esp + 0x14]
004457be  mov        edi, edx
004457c0  add        edi, 0x85
004457c6  call       0x462060

// block 004457cb
004457cb  mov        edx, dword ptr [esp + 0x14]
004457cf  push       edx
004457d0  mov        ebx, 1
004457d5  call       0x461970

// block 004457da
004457da  mov        eax, dword ptr [ebp + 0x28]
004457dd  add        eax, 2
004457e0  cdq        
004457e1  mov        ecx, 3
004457e6  idiv       ecx
004457e8  lea        ebx, [esp + 0x14]
004457ec  mov        edi, edx
004457ee  add        edi, 0x85
004457f4  call       0x462060

// block 004457f9
004457f9  mov        edx, dword ptr [esp + 0x14]
004457fd  push       edx
004457fe  mov        ebx, 1
00445803  call       0x461970

// block 00445808
00445808  mov        eax, dword ptr [esi]
0044580a  mov        edx, dword ptr [0x4ce8cc]
00445810  push       eax
00445811  call       0x461920

// block 00445816
00445816  test       eax, eax
00445818  jne        0x44581c

// block 0044581a
0044581a  mov        dword ptr [esi], eax

// block 0044581c
0044581c  add        eax, 0x10
0044581f  je         0x44583c

// block 00445821
00445821  mov        ecx, dword ptr [eax]
00445823  mov        edx, 0x88
00445828  cmp        word ptr [ecx + 0x3ea], dx
0044582f  je         0x4458f3

// block 00445835
00445835  mov        eax, dword ptr [eax + 4]
00445838  test       eax, eax
0044583a  jne        0x445821

// block 0044583c
0044583c  xor        eax, eax

// block 0044583e
0044583e  push       eax
0044583f  call       0x461970

// block 00445844
00445844  mov        ecx, dword ptr [esi]
00445846  mov        edx, dword ptr [0x4ce8cc]
0044584c  push       ecx
0044584d  call       0x461920

// block 00445852
00445852  test       eax, eax
00445854  jne        0x445858

// block 00445856
00445856  mov        dword ptr [esi], eax

// block 00445858
00445858  add        eax, 0x10
0044585b  je         0x44587b

// block 0044585d
0044585d  lea        ecx, [ecx]

// block 00445860
00445860  mov        edx, dword ptr [eax]
00445862  mov        ecx, 0x89
00445867  cmp        word ptr [edx + 0x3ea], cx
0044586e  je         0x4458fc

// block 00445874
00445874  mov        eax, dword ptr [eax + 4]
00445877  test       eax, eax
00445879  jne        0x445860

// block 0044587b
0044587b  xor        eax, eax

// block 0044587d
0044587d  push       eax
0044587e  call       0x461970

// block 00445883
00445883  mov        eax, dword ptr [esi]
00445885  mov        edx, dword ptr [0x4ce8cc]
0044588b  push       eax
0044588c  call       0x461920

// block 00445891
00445891  test       eax, eax
00445893  jne        0x445897

// block 00445895
00445895  mov        dword ptr [esi], eax

// block 00445897
00445897  add        eax, 0x10
0044589a  je         0x4458b7

// block 0044589c
0044589c  lea        esp, [esp]

// block 004458a0
004458a0  mov        ecx, dword ptr [eax]
004458a2  mov        edx, 0x8a
004458a7  cmp        word ptr [ecx + 0x3ea], dx
004458ae  je         0x445903

// block 004458b0
004458b0  mov        eax, dword ptr [eax + 4]
004458b3  test       eax, eax
004458b5  jne        0x4458a0

// block 004458b7
004458b7  xor        eax, eax

// block 004458b9
004458b9  push       eax
004458ba  call       0x461970

// block 004458bf
004458bf  mov        edx, 7
004458c4  call       0x453d90

// block 004458c9
004458c9  mov        ecx, 3
004458ce  mov        eax, ebp
004458d0  call       0x43ef40

// block 004458d5
004458d5  pop        edi
004458d6  pop        esi
004458d7  pop        ebx
004458d8  mov        eax, 1
004458dd  pop        ebp
004458de  ret        4

// block 004458e1
004458e1  mov        eax, edx
004458e3  mov        eax, dword ptr [eax]
004458e5  jmp        0x445768

// block 004458ea
004458ea  mov        eax, edx
004458ec  mov        eax, dword ptr [eax]
004458ee  jmp        0x4457a8

// block 004458f3
004458f3  mov        eax, ecx
004458f5  mov        eax, dword ptr [eax]
004458f7  jmp        0x44583e

// block 004458fc
004458fc  mov        eax, dword ptr [edx]
004458fe  jmp        0x44587d

// block 00445903
00445903  mov        eax, ecx
00445905  mov        eax, dword ptr [eax]
00445907  jmp        0x4458b9

// block 00445909
00445909  cmp        dword ptr [ebp + 0x2b8], 0xe
00445910  jl         0x445a11

// block 00445916
00445916  mov        esi, 0x6f
0044591b  mov        edi, ebp
0044591d  call       0x43efd0

// block 00445922
00445922  lea        edx, [esi - 0x68]
00445925  mov        eax, ebp
00445927  call       0x43eee0

// block 0044592c
0044592c  mov        esi, dword ptr [ebp + 0x28]
0044592f  mov        edi, dword ptr [0x4b0c90]
00445935  lea        eax, [ebp + 0x28]
00445938  mov        ecx, esi
0044593a  mov        dword ptr [0x4b0c90], ecx
00445940  mov        dword ptr [0x4ce8bc], ecx
00445946  call       0x464900

// block 0044594b
0044594b  mov        dword ptr [ebp + 0x30], 3
00445952  cmp        edi, esi
00445954  je         0x4459aa

// block 00445956
00445956  mov        ecx, dword ptr [eax + 8]
00445959  test       ecx, ecx
0044595b  je         0x44598e

// block 0044595d
0044595d  jg         0x445978

// block 0044595f
0044595f  pop        edi
00445960  pop        esi
00445961  dec        ecx
00445962  pop        ebx
00445963  mov        dword ptr [eax], ecx
00445965  mov        dword ptr [0x4ce8c0], 0
0044596f  mov        eax, 1
00445974  pop        ebp
00445975  ret        4

// block 00445978
00445978  pop        edi
00445979  pop        esi
0044597a  xor        ecx, ecx
0044597c  pop        ebx
0044597d  mov        dword ptr [eax], ecx
0044597f  mov        dword ptr [0x4ce8c0], ecx
00445985  mov        eax, 1
0044598a  pop        ebp
0044598b  ret        4

// block 0044598e
0044598e  pop        edi
0044598f  pop        esi
00445990  pop        ebx
00445991  mov        dword ptr [eax], 0
00445997  mov        dword ptr [0x4ce8c0], 0
004459a1  mov        eax, 1
004459a6  pop        ebp
004459a7  ret        4

// block 004459aa
004459aa  mov        esi, dword ptr [0x4ce8c0]
004459b0  mov        ecx, esi
004459b2  mov        edx, eax
004459b4  call       0x40f790

// block 004459b9
004459b9  pop        edi
004459ba  mov        dword ptr [0x4b0c94], esi
004459c0  pop        esi
004459c1  pop        ebx
004459c2  mov        eax, 1
004459c7  pop        ebp
004459c8  ret        4

// block 004459cb
004459cb  cmp        dword ptr [ebp + 0x2b8], 6
004459d2  jl         0x445a11

// block 004459d4
004459d4  mov        esi, 0x8f
004459d9  mov        edi, ebp
004459db  call       0x43efd0

// block 004459e0
004459e0  mov        esi, 0x6f
004459e5  call       0x43efd0

// block 004459ea
004459ea  lea        edx, [esi - 0x6a]
004459ed  mov        eax, ebp
004459ef  call       0x43eee0

// block 004459f4
004459f4  mov        ecx, dword ptr [ebp + 0x28]
004459f7  lea        eax, [ebp + 0x28]
004459fa  mov        dword ptr [0x4b0c90], ecx
00445a00  call       0x464940

// block 00445a05
00445a05  mov        edx, dword ptr [0x4b0c90]
00445a0b  mov        dword ptr [0x4ce8bc], edx

// block 00445a11
00445a11  pop        edi
00445a12  pop        esi
00445a13  pop        ebx

// block 00445a14
00445a14  mov        eax, 1
00445a19  pop        ebp
00445a1a  ret        4
