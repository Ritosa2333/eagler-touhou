
// block 00485336
00485336  mov        edi, edi
00485338  push       ebp
00485339  mov        ebp, esp
0048533b  sub        esp, 0x20
0048533e  mov        eax, dword ptr [0x4ad138]
00485343  xor        eax, ebp
00485345  mov        dword ptr [ebp - 4], eax
00485348  mov        eax, dword ptr [ebp + 0xc]
0048534b  push       ebx
0048534c  push       esi
0048534d  mov        esi, dword ptr [ebp + 0x1c]
00485350  xor        ebx, ebx
00485352  sub        eax, ebx
00485354  push       edi
00485355  je         0x48536a

// block 00485357
00485357  dec        eax
00485358  je         0x485362

// block 0048535a
0048535a  mov        edi, dword ptr [esi + 0xa8]
00485360  jmp        0x485370

// block 00485362
00485362  mov        edi, dword ptr [esi + 0xa4]
00485368  jmp        0x485370

// block 0048536a
0048536a  mov        edi, dword ptr [esi + 0xa0]

// block 00485370
00485370  cmp        dword ptr [esi + 0xb0], 1
00485377  mov        eax, dword ptr [ebp + 0x10]
0048537a  je         0x48547e

// block 00485380
00485380  cmp        dword ptr [ebp + 0xc], 2
00485384  mov        ecx, dword ptr [0x498040]
0048538a  jne        0x485392

// block 0048538c
0048538c  mov        ecx, dword ptr [0x498044]

// block 00485392
00485392  mov        edx, 0x76c
00485397  add        dx, word ptr [eax + 0x14]
0048539b  push       ebx
0048539c  mov        word ptr [ebp - 0x20], dx
004853a0  mov        dx, word ptr [eax + 0x10]
004853a4  inc        dx
004853a6  mov        word ptr [ebp - 0x1e], dx
004853aa  mov        dx, word ptr [eax + 0xc]
004853ae  mov        word ptr [ebp - 0x1a], dx
004853b2  mov        dx, word ptr [eax + 8]
004853b6  mov        word ptr [ebp - 0x18], dx
004853ba  mov        dx, word ptr [eax + 4]
004853be  mov        ax, word ptr [eax]
004853c1  mov        word ptr [ebp - 0x14], ax
004853c5  push       ebx
004853c6  xor        eax, eax
004853c8  push       edi
004853c9  mov        word ptr [ebp - 0x12], ax
004853cd  lea        eax, [ebp - 0x20]
004853d0  push       eax
004853d1  push       ebx
004853d2  push       dword ptr [esi + 0xac]
004853d8  mov        dword ptr [ebp - 0xc], ecx
004853db  mov        word ptr [ebp - 0x16], dx
004853df  call       ecx

// block 004853e1
004853e1  mov        dword ptr [ebp - 0x10], eax
004853e4  cmp        eax, ebx
004853e6  je         0x48547e

// block 004853ec
004853ec  add        eax, 8
004853ef  cmp        eax, 0x400
004853f4  jg         0x485409

// block 004853f6
004853f6  call       0x48d830

// block 004853fb
004853fb  mov        eax, esp
004853fd  cmp        eax, ebx
004853ff  je         0x48541d

// block 00485401
00485401  mov        dword ptr [eax], 0xcccc
00485407  jmp        0x48541a

// block 00485409
00485409  push       eax
0048540a  call       0x46d04a

// block 0048540f
0048540f  pop        ecx
00485410  cmp        eax, ebx
00485412  je         0x48541d

// block 00485414
00485414  mov        dword ptr [eax], 0xdddd

// block 0048541a
0048541a  add        eax, 8

// block 0048541d
0048541d  mov        dword ptr [ebp - 8], eax
00485420  cmp        eax, ebx
00485422  je         0x48547e

// block 00485424
00485424  push       dword ptr [ebp - 0x10]
00485427  mov        ebx, eax
00485429  push       ebx
0048542a  push       edi
0048542b  lea        eax, [ebp - 0x20]
0048542e  push       eax
0048542f  push       0
00485431  push       dword ptr [esi + 0xac]
00485437  call       dword ptr [ebp - 0xc]

// block 0048543a
0048543a  dec        eax
0048543b  test       eax, eax
0048543d  jle        0x485460

// block 0048543f
0048543f  mov        esi, dword ptr [ebp + 0x18]
00485442  mov        ecx, dword ptr [ebp + 0x14]

// block 00485445
00485445  cmp        dword ptr [esi], 0
00485448  jbe        0x485460

// block 0048544a
0048544a  mov        edx, dword ptr [ecx]
0048544c  mov        dword ptr [ebp - 0x10], eax
0048544f  mov        al, byte ptr [ebx]
00485451  mov        byte ptr [edx], al
00485453  inc        dword ptr [ecx]
00485455  mov        eax, dword ptr [ebp - 0x10]
00485458  inc        ebx
00485459  dec        dword ptr [esi]
0048545b  dec        eax
0048545c  test       eax, eax
0048545e  jg         0x485445

// block 00485460
00485460  push       dword ptr [ebp - 8]
00485463  call       0x48326a

// block 00485468
00485468  pop        ecx

// block 00485469
00485469  xor        eax, eax
0048546b  inc        eax

// block 0048546c
0048546c  lea        esp, [ebp - 0x2c]
0048546f  pop        edi
00485470  pop        esi
00485471  pop        ebx
00485472  mov        ecx, dword ptr [ebp - 4]
00485475  xor        ecx, ebp
00485477  call       0x46c932

// block 0048547c
0048547c  leave      
0048547d  ret        

// block 0048547e
0048547e  mov        al, byte ptr [edi]
00485480  test       al, al
00485482  je         0x485469

// block 00485484
00485484  mov        ebx, dword ptr [ebp + 0x18]
00485487  mov        esi, dword ptr [ebp + 0x14]

// block 0048548a
0048548a  cmp        dword ptr [ebx], 0
0048548d  je         0x485469

// block 0048548f
0048548f  xor        edx, edx
00485491  mov        dword ptr [ebp - 8], edx
00485494  mov        ecx, edi

// block 00485496
00485496  inc        ecx
00485497  inc        edx
00485498  cmp        byte ptr [ecx], al
0048549a  je         0x485496

// block 0048549c
0048549c  mov        dword ptr [ebp - 0xc], ecx
0048549f  movsx      ecx, al
004854a2  cmp        ecx, 0x64
004854a5  jg         0x48562d

// block 004854ab
004854ab  je         0x4855ff

// block 004854b1
004854b1  cmp        ecx, 0x27
004854b4  je         0x485592

// block 004854ba
004854ba  cmp        ecx, 0x41
004854bd  je         0x48555e

// block 004854c3
004854c3  cmp        ecx, 0x48
004854c6  je         0x485546

// block 004854c8
004854c8  cmp        ecx, 0x4d
004854cb  je         0x48551c

// block 004854cd
004854cd  cmp        ecx, 0x61
004854d0  je         0x48555e

// block 004854d6
004854d6  push       dword ptr [ebp + 8]
004854d9  push       ecx
004854da  call       0x47c5a3

// block 004854df
004854df  pop        ecx
004854e0  pop        ecx
004854e1  test       eax, eax
004854e3  je         0x485502

// block 004854e5
004854e5  cmp        dword ptr [ebx], 1
004854e8  jbe        0x485502

// block 004854ea
004854ea  lea        eax, [edi + 1]
004854ed  cmp        byte ptr [eax], 0
004854f0  je         0x485790

// block 004854f6
004854f6  mov        dl, byte ptr [edi]
004854f8  mov        ecx, dword ptr [esi]
004854fa  mov        byte ptr [ecx], dl
004854fc  inc        dword ptr [esi]
004854fe  dec        dword ptr [ebx]
00485500  mov        edi, eax

// block 00485502
00485502  mov        cl, byte ptr [edi]
00485504  mov        eax, dword ptr [esi]
00485506  mov        byte ptr [eax], cl
00485508  inc        dword ptr [esi]
0048550a  inc        edi
0048550b  dec        dword ptr [ebx]

// block 0048550d
0048550d  mov        al, byte ptr [edi]
0048550f  test       al, al
00485511  jne        0x48548a

// block 00485517
00485517  jmp        0x485469

// block 0048551c
0048551c  mov        eax, edx
0048551e  dec        eax
0048551f  je         0x485538

// block 00485521
00485521  dec        eax
00485522  je         0x48553f

// block 00485524
00485524  dec        eax
00485525  je         0x485531

// block 00485527
00485527  dec        eax
00485528  jne        0x4854d6

// block 0048552a
0048552a  mov        al, 0x42
0048552c  jmp        0x48576d

// block 00485531
00485531  mov        al, 0x62
00485533  jmp        0x48576d

// block 00485538
00485538  mov        dword ptr [ebp - 8], 1

// block 0048553f
0048553f  mov        al, 0x6d
00485541  jmp        0x48576d

// block 00485546
00485546  mov        eax, edx
00485548  dec        eax
00485549  je         0x485550

// block 0048554b
0048554b  dec        eax
0048554c  je         0x485557

// block 0048554e
0048554e  jmp        0x4854d6

// block 00485550
00485550  mov        dword ptr [ebp - 8], 1

// block 00485557
00485557  mov        al, 0x48
00485559  jmp        0x48576d

// block 0048555e
0048555e  push       0x49eb08
00485563  push       edi
00485564  call       0x46e2ea

// block 00485569
00485569  pop        ecx
0048556a  pop        ecx
0048556b  test       eax, eax
0048556d  jne        0x485574

// block 0048556f
0048556f  add        edi, 5
00485572  jmp        0x485588

// block 00485574
00485574  push       0x49eb04
00485579  push       edi
0048557a  call       0x46e2ea

// block 0048557f
0048557f  pop        ecx
00485580  pop        ecx
00485581  test       eax, eax
00485583  jne        0x48558b

// block 00485585
00485585  add        edi, 3

// block 00485588
00485588  mov        dword ptr [ebp - 0xc], edi

// block 0048558b
0048558b  mov        al, 0x70
0048558d  jmp        0x48576d

// block 00485592
00485592  add        edi, edx
00485594  test       dl, 1
00485597  je         0x48550d

// block 0048559d
0048559d  mov        al, byte ptr [edi]
0048559f  test       al, al
004855a1  je         0x485469

// block 004855a7
004855a7  cmp        dword ptr [ebx], 0
004855aa  je         0x48550d

// block 004855b0
004855b0  cmp        al, 0x27
004855b2  je         0x4855f9

// block 004855b4
004855b4  push       dword ptr [ebp + 8]
004855b7  movsx      eax, al
004855ba  push       eax
004855bb  call       0x47c5a3

// block 004855c0
004855c0  pop        ecx
004855c1  pop        ecx
004855c2  test       eax, eax
004855c4  je         0x4855e3

// block 004855c6
004855c6  cmp        dword ptr [ebx], 1
004855c9  jbe        0x4855e3

// block 004855cb
004855cb  lea        eax, [edi + 1]
004855ce  cmp        byte ptr [eax], 0
004855d1  je         0x485790

// block 004855d7
004855d7  mov        dl, byte ptr [edi]
004855d9  mov        ecx, dword ptr [esi]
004855db  mov        byte ptr [ecx], dl
004855dd  inc        dword ptr [esi]
004855df  dec        dword ptr [ebx]
004855e1  mov        edi, eax

// block 004855e3
004855e3  mov        eax, dword ptr [esi]
004855e5  mov        cl, byte ptr [edi]
004855e7  mov        byte ptr [eax], cl
004855e9  inc        dword ptr [esi]
004855eb  inc        edi
004855ec  dec        dword ptr [ebx]
004855ee  mov        al, byte ptr [edi]
004855f0  test       al, al
004855f2  jne        0x4855a7

// block 004855f4
004855f4  jmp        0x48550d

// block 004855f9
004855f9  inc        edi
004855fa  jmp        0x48550d

// block 004855ff
004855ff  mov        eax, edx
00485601  dec        eax
00485602  je         0x48561f

// block 00485604
00485604  dec        eax
00485605  je         0x485626

// block 00485607
00485607  dec        eax
00485608  je         0x485618

// block 0048560a
0048560a  dec        eax
0048560b  jne        0x4854d6

// block 00485611
00485611  mov        al, 0x41
00485613  jmp        0x48576d

// block 00485618
00485618  mov        al, 0x61
0048561a  jmp        0x48576d

// block 0048561f
0048561f  mov        dword ptr [ebp - 8], 1

// block 00485626
00485626  mov        al, 0x64
00485628  jmp        0x48576d

// block 0048562d
0048562d  mov        eax, ecx
0048562f  sub        eax, 0x68
00485632  je         0x485757

// block 00485638
00485638  sub        eax, 5
0048563b  je         0x48573f

// block 00485641
00485641  sub        eax, 6
00485644  je         0x485727

// block 0048564a
0048564a  dec        eax
0048564b  je         0x485672

// block 0048564d
0048564d  sub        eax, 5
00485650  jne        0x4854d6

// block 00485656
00485656  mov        eax, edx
00485658  dec        eax
00485659  dec        eax
0048565a  je         0x48566b

// block 0048565c
0048565c  dec        eax
0048565d  dec        eax
0048565e  jne        0x4854d6

// block 00485664
00485664  mov        al, 0x59
00485666  jmp        0x48576d

// block 0048566b
0048566b  mov        al, 0x79
0048566d  jmp        0x48576d

// block 00485672
00485672  mov        eax, dword ptr [ebp + 0x10]
00485675  cmp        dword ptr [eax + 8], 0xb
00485679  mov        eax, dword ptr [ebp + 0x1c]
0048567c  jg         0x485686

// block 0048567e
0048567e  mov        edi, dword ptr [eax + 0x98]
00485684  jmp        0x48568c

// block 00485686
00485686  mov        edi, dword ptr [eax + 0x9c]

// block 0048568c
0048568c  cmp        edx, 1
0048568f  jne        0x48571f

// block 00485695
00485695  cmp        dword ptr [ebx], 0
00485698  jbe        0x48571f

// block 0048569e
0048569e  movsx      eax, byte ptr [edi]
004856a1  push       dword ptr [ebp + 8]
004856a4  push       eax
004856a5  call       0x47c5a3

// block 004856aa
004856aa  pop        ecx
004856ab  pop        ecx
004856ac  test       eax, eax
004856ae  je         0x4856cd

// block 004856b0
004856b0  cmp        dword ptr [ebx], 1
004856b3  jbe        0x4856cd

// block 004856b5
004856b5  lea        eax, [edi + 1]
004856b8  cmp        byte ptr [eax], 0
004856bb  je         0x485790

// block 004856c1
004856c1  mov        dl, byte ptr [edi]
004856c3  mov        ecx, dword ptr [esi]
004856c5  mov        byte ptr [ecx], dl
004856c7  inc        dword ptr [esi]
004856c9  dec        dword ptr [ebx]
004856cb  mov        edi, eax

// block 004856cd
004856cd  mov        eax, dword ptr [esi]
004856cf  mov        cl, byte ptr [edi]
004856d1  mov        byte ptr [eax], cl
004856d3  inc        dword ptr [esi]
004856d5  dec        dword ptr [ebx]
004856d7  jmp        0x485788

// block 004856dc
004856dc  cmp        dword ptr [ebx], 0
004856df  jbe        0x485788

// block 004856e5
004856e5  push       dword ptr [ebp + 8]
004856e8  movsx      eax, al
004856eb  push       eax
004856ec  call       0x47c5a3

// block 004856f1
004856f1  pop        ecx
004856f2  pop        ecx
004856f3  test       eax, eax
004856f5  je         0x485714

// block 004856f7
004856f7  cmp        dword ptr [ebx], 1
004856fa  jbe        0x485714

// block 004856fc
004856fc  lea        eax, [edi + 1]
004856ff  cmp        byte ptr [eax], 0
00485702  je         0x485790

// block 00485708
00485708  mov        dl, byte ptr [edi]
0048570a  mov        ecx, dword ptr [esi]
0048570c  mov        byte ptr [ecx], dl
0048570e  inc        dword ptr [esi]
00485710  dec        dword ptr [ebx]
00485712  mov        edi, eax

// block 00485714
00485714  mov        cl, byte ptr [edi]
00485716  mov        eax, dword ptr [esi]
00485718  mov        byte ptr [eax], cl
0048571a  inc        dword ptr [esi]
0048571c  inc        edi
0048571d  dec        dword ptr [ebx]

// block 0048571f
0048571f  mov        al, byte ptr [edi]
00485721  test       al, al
00485723  jne        0x4856dc

// block 00485725
00485725  jmp        0x485788

// block 00485727
00485727  mov        eax, edx
00485729  dec        eax
0048572a  je         0x485734

// block 0048572c
0048572c  dec        eax
0048572d  je         0x48573b

// block 0048572f
0048572f  jmp        0x4854d6

// block 00485734
00485734  mov        dword ptr [ebp - 8], 1

// block 0048573b
0048573b  mov        al, 0x53
0048573d  jmp        0x48576d

// block 0048573f
0048573f  mov        eax, edx
00485741  dec        eax
00485742  je         0x48574c

// block 00485744
00485744  dec        eax
00485745  je         0x485753

// block 00485747
00485747  jmp        0x4854d6

// block 0048574c
0048574c  mov        dword ptr [ebp - 8], 1

// block 00485753
00485753  mov        al, 0x4d
00485755  jmp        0x48576d

// block 00485757
00485757  mov        eax, edx
00485759  dec        eax
0048575a  je         0x485764

// block 0048575c
0048575c  dec        eax
0048575d  je         0x48576b

// block 0048575f
0048575f  jmp        0x4854d6

// block 00485764
00485764  mov        dword ptr [ebp - 8], 1

// block 0048576b
0048576b  mov        al, 0x49

// block 0048576d
0048576d  push       dword ptr [ebp - 8]
00485770  mov        edx, dword ptr [ebp + 0x10]
00485773  push       dword ptr [ebp + 0x1c]
00485776  mov        ecx, esi
00485778  push       ebx
00485779  push       dword ptr [ebp + 8]
0048577c  call       0x484f4e

// block 00485781
00485781  add        esp, 0x10
00485784  test       eax, eax
00485786  je         0x485790

// block 00485788
00485788  mov        edi, dword ptr [ebp - 0xc]
0048578b  jmp        0x48550d

// block 00485790
00485790  xor        eax, eax
00485792  jmp        0x48546c
