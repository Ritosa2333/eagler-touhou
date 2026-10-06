
// block 004904ca
004904ca  mov        edi, edi
004904cc  push       ebp
004904cd  mov        ebp, esp
004904cf  sub        esp, 0x14
004904d2  push       ebx
004904d3  push       esi
004904d4  push       edi
004904d5  mov        edi, dword ptr [ebp + 0xc]
004904d8  and        edi, 0xfff7ffff
004904de  wait       
004904df  fnstcw     word ptr [ebp - 4]
004904e2  mov        ebx, dword ptr [ebp - 4]
004904e5  xor        edx, edx
004904e7  test       bl, 1
004904ea  je         0x4904ef

// block 004904ec
004904ec  push       0x10
004904ee  pop        edx

// block 004904ef
004904ef  test       bl, 4
004904f2  je         0x4904f7

// block 004904f4
004904f4  or         edx, 8

// block 004904f7
004904f7  test       bl, 8
004904fa  je         0x4904ff

// block 004904fc
004904fc  or         edx, 4

// block 004904ff
004904ff  test       bl, 0x10
00490502  je         0x490507

// block 00490504
00490504  or         edx, 2

// block 00490507
00490507  test       bl, 0x20
0049050a  je         0x49050f

// block 0049050c
0049050c  or         edx, 1

// block 0049050f
0049050f  test       bl, 2
00490512  je         0x49051a

// block 00490514
00490514  or         edx, 0x80000

// block 0049051a
0049051a  movzx      ecx, bx
0049051d  mov        eax, ecx
0049051f  mov        esi, 0xc00
00490524  and        eax, esi
00490526  je         0x490550

// block 00490528
00490528  cmp        eax, 0x400
0049052d  je         0x49054a

// block 0049052f
0049052f  cmp        eax, 0x800
00490534  je         0x490542

// block 00490536
00490536  cmp        eax, esi
00490538  jne        0x490550

// block 0049053a
0049053a  or         edx, 0x300
00490540  jmp        0x490550

// block 00490542
00490542  or         edx, 0x200
00490548  jmp        0x490550

// block 0049054a
0049054a  or         edx, 0x100

// block 00490550
00490550  and        ecx, 0x300
00490556  je         0x490568

// block 00490558
00490558  cmp        ecx, 0x200
0049055e  jne        0x49056e

// block 00490560
00490560  or         edx, 0x10000
00490566  jmp        0x49056e

// block 00490568
00490568  or         edx, 0x20000

// block 0049056e
0049056e  test       ebx, 0x1000
00490574  je         0x49057c

// block 00490576
00490576  or         edx, 0x40000

// block 0049057c
0049057c  mov        eax, edi
0049057e  not        eax
00490580  mov        ecx, edi
00490582  and        ecx, dword ptr [ebp + 8]
00490585  and        eax, edx
00490587  or         eax, ecx
00490589  mov        dword ptr [ebp - 0x10], eax
0049058c  cmp        eax, edx
0049058e  je         0x490642

// block 00490594
00490594  mov        ebx, eax
00490596  call       0x48f849

// block 0049059b
0049059b  movzx      eax, ax
0049059e  mov        dword ptr [ebp + 0xc], eax
004905a1  fldcw      word ptr [ebp + 0xc]
004905a4  wait       
004905a5  fnstcw     word ptr [ebp + 0xc]
004905a8  mov        ebx, dword ptr [ebp + 0xc]
004905ab  xor        edx, edx
004905ad  test       bl, 1
004905b0  je         0x4905b5

// block 004905b2
004905b2  push       0x10
004905b4  pop        edx

// block 004905b5
004905b5  test       bl, 4
004905b8  je         0x4905bd

// block 004905ba
004905ba  or         edx, 8

// block 004905bd
004905bd  test       bl, 8
004905c0  je         0x4905c5

// block 004905c2
004905c2  or         edx, 4

// block 004905c5
004905c5  test       bl, 0x10
004905c8  je         0x4905cd

// block 004905ca
004905ca  or         edx, 2

// block 004905cd
004905cd  test       bl, 0x20
004905d0  je         0x4905d5

// block 004905d2
004905d2  or         edx, 1

// block 004905d5
004905d5  test       bl, 2
004905d8  je         0x4905e0

// block 004905da
004905da  or         edx, 0x80000

// block 004905e0
004905e0  movzx      ecx, bx
004905e3  mov        eax, ecx
004905e5  and        eax, esi
004905e7  je         0x490611

// block 004905e9
004905e9  cmp        eax, 0x400
004905ee  je         0x49060b

// block 004905f0
004905f0  cmp        eax, 0x800
004905f5  je         0x490603

// block 004905f7
004905f7  cmp        eax, esi
004905f9  jne        0x490611

// block 004905fb
004905fb  or         edx, 0x300
00490601  jmp        0x490611

// block 00490603
00490603  or         edx, 0x200
00490609  jmp        0x490611

// block 0049060b
0049060b  or         edx, 0x100

// block 00490611
00490611  and        ecx, 0x300
00490617  je         0x490629

// block 00490619
00490619  cmp        ecx, 0x200
0049061f  jne        0x49062f

// block 00490621
00490621  or         edx, 0x10000
00490627  jmp        0x49062f

// block 00490629
00490629  or         edx, 0x20000

// block 0049062f
0049062f  test       ebx, 0x1000
00490635  je         0x49063d

// block 00490637
00490637  or         edx, 0x40000

// block 0049063d
0049063d  mov        eax, edx
0049063f  mov        dword ptr [ebp - 0x10], edx

// block 00490642
00490642  xor        esi, esi
00490644  cmp        dword ptr [0x4d52dc], esi
0049064a  je         0x4907d7

// block 00490650
00490650  and        edi, 0x308031f
00490656  mov        dword ptr [ebp - 0x14], edi
00490659  stmxcsr    dword ptr [ebp - 0xc]
0049065d  mov        eax, dword ptr [ebp - 0xc]
00490660  test       al, al
00490662  jns        0x490667

// block 00490664
00490664  push       0x10
00490666  pop        esi

// block 00490667
00490667  test       eax, 0x200
0049066c  je         0x490671

// block 0049066e
0049066e  or         esi, 8

// block 00490671
00490671  test       eax, 0x400
00490676  je         0x49067b

// block 00490678
00490678  or         esi, 4

// block 0049067b
0049067b  test       eax, 0x800
00490680  je         0x490685

// block 00490682
00490682  or         esi, 2

// block 00490685
00490685  test       eax, 0x1000
0049068a  je         0x49068f

// block 0049068c
0049068c  or         esi, 1

// block 0049068f
0049068f  test       eax, 0x100
00490694  je         0x49069c

// block 00490696
00490696  or         esi, 0x80000

// block 0049069c
0049069c  mov        ecx, eax
0049069e  mov        edi, 0x6000
004906a3  and        ecx, edi
004906a5  je         0x4906d1

// block 004906a7
004906a7  cmp        ecx, 0x2000
004906ad  je         0x4906cb

// block 004906af
004906af  cmp        ecx, 0x4000
004906b5  je         0x4906c3

// block 004906b7
004906b7  cmp        ecx, edi
004906b9  jne        0x4906d1

// block 004906bb
004906bb  or         esi, 0x300
004906c1  jmp        0x4906d1

// block 004906c3
004906c3  or         esi, 0x200
004906c9  jmp        0x4906d1

// block 004906cb
004906cb  or         esi, 0x100

// block 004906d1
004906d1  mov        ebx, 0x8040
004906d6  and        eax, ebx
004906d8  sub        eax, 0x40
004906db  je         0x4906f9

// block 004906dd
004906dd  sub        eax, 0x7fc0
004906e2  je         0x4906f1

// block 004906e4
004906e4  sub        eax, 0x40
004906e7  jne        0x4906ff

// block 004906e9
004906e9  or         esi, 0x1000000
004906ef  jmp        0x4906ff

// block 004906f1
004906f1  or         esi, 0x3000000
004906f7  jmp        0x4906ff

// block 004906f9
004906f9  or         esi, 0x2000000

// block 004906ff
004906ff  mov        eax, dword ptr [ebp - 0x14]
00490702  mov        edx, eax
00490704  and        eax, dword ptr [ebp + 8]
00490707  not        edx
00490709  and        edx, esi
0049070b  or         edx, eax
0049070d  cmp        edx, esi
0049070f  jne        0x490718

// block 00490711
00490711  mov        eax, esi
00490713  jmp        0x4907c2

// block 00490718
00490718  call       0x48f9ba

// block 0049071d
0049071d  push       eax
0049071e  mov        dword ptr [ebp - 8], eax
00490721  call       0x491220

// block 00490726
00490726  pop        ecx
00490727  stmxcsr    dword ptr [ebp - 8]
0049072b  mov        edx, dword ptr [ebp - 8]
0049072e  xor        eax, eax
00490730  test       dl, dl
00490732  jns        0x490737

// block 00490734
00490734  push       0x10
00490736  pop        eax

// block 00490737
00490737  test       edx, 0x200
0049073d  je         0x490742

// block 0049073f
0049073f  or         eax, 8

// block 00490742
00490742  test       edx, 0x400
00490748  je         0x49074d

// block 0049074a
0049074a  or         eax, 4

// block 0049074d
0049074d  test       edx, 0x800
00490753  je         0x490758

// block 00490755
00490755  or         eax, 2

// block 00490758
00490758  test       edx, 0x1000
0049075e  je         0x490763

// block 00490760
00490760  or         eax, 1

// block 00490763
00490763  mov        esi, 0x100
00490768  test       esi, edx
0049076a  je         0x490771

// block 0049076c
0049076c  or         eax, 0x80000

// block 00490771
00490771  mov        ecx, edx
00490773  and        ecx, edi
00490775  je         0x49079b

// block 00490777
00490777  cmp        ecx, 0x2000
0049077d  je         0x490799

// block 0049077f
0049077f  cmp        ecx, 0x4000
00490785  je         0x490792

// block 00490787
00490787  cmp        ecx, edi
00490789  jne        0x49079b

// block 0049078b
0049078b  or         eax, 0x300
00490790  jmp        0x49079b

// block 00490792
00490792  or         eax, 0x200
00490797  jmp        0x49079b

// block 00490799
00490799  or         eax, esi

// block 0049079b
0049079b  and        edx, ebx
0049079d  sub        edx, 0x40
004907a0  je         0x4907bd

// block 004907a2
004907a2  sub        edx, 0x7fc0
004907a8  je         0x4907b6

// block 004907aa
004907aa  sub        edx, 0x40
004907ad  jne        0x4907c2

// block 004907af
004907af  or         eax, 0x1000000
004907b4  jmp        0x4907c2

// block 004907b6
004907b6  or         eax, 0x3000000
004907bb  jmp        0x4907c2

// block 004907bd
004907bd  or         eax, 0x2000000

// block 004907c2
004907c2  mov        ecx, eax
004907c4  xor        ecx, dword ptr [ebp - 0x10]
004907c7  or         eax, dword ptr [ebp - 0x10]
004907ca  test       ecx, 0x8031f
004907d0  je         0x4907d7

// block 004907d2
004907d2  or         eax, 0x80000000

// block 004907d7
004907d7  pop        edi
004907d8  pop        esi
004907d9  pop        ebx
004907da  leave      
004907db  ret        
