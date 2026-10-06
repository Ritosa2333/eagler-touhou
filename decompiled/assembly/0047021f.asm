
// block 0047021f
0047021f  mov        edi, edi
00470221  push       ebp
00470222  mov        ebp, esp
00470224  sub        esp, 0x278
0047022a  mov        eax, dword ptr [0x4ad138]
0047022f  xor        eax, ebp
00470231  mov        dword ptr [ebp - 4], eax
00470234  push       ebx
00470235  mov        ebx, dword ptr [ebp + 0xc]
00470238  push       esi
00470239  mov        esi, dword ptr [ebp + 8]
0047023c  xor        eax, eax
0047023e  push       edi
0047023f  mov        edi, dword ptr [ebp + 0x14]
00470242  push       dword ptr [ebp + 0x10]
00470245  lea        ecx, [ebp - 0x25c]
0047024b  mov        dword ptr [ebp - 0x24c], esi
00470251  mov        dword ptr [ebp - 0x224], edi
00470257  mov        dword ptr [ebp - 0x248], eax
0047025d  mov        dword ptr [ebp - 0x210], eax
00470263  mov        dword ptr [ebp - 0x234], eax
00470269  mov        dword ptr [ebp - 0x218], eax
0047026f  mov        dword ptr [ebp - 0x230], eax
00470275  mov        dword ptr [ebp - 0x240], eax
0047027b  mov        dword ptr [ebp - 0x238], eax
00470281  call       0x46d3b4

// block 00470286
00470286  test       esi, esi
00470288  jne        0x4702bf

// block 0047028a
0047028a  call       0x46e96f

// block 0047028f
0047028f  mov        dword ptr [eax], 0x16
00470295  xor        eax, eax
00470297  push       eax
00470298  push       eax
00470299  push       eax
0047029a  push       eax
0047029b  push       eax
0047029c  call       0x470f2d

// block 004702a1
004702a1  add        esp, 0x14
004702a4  cmp        byte ptr [ebp - 0x250], 0
004702ab  je         0x4702b7

// block 004702ad
004702ad  mov        eax, dword ptr [ebp - 0x254]
004702b3  and        dword ptr [eax + 0x70], 0xfffffffd

// block 004702b7
004702b7  or         eax, 0xffffffff
004702ba  jmp        0x470d87

// block 004702bf
004702bf  test       byte ptr [esi + 0xc], 0x40
004702c3  jne        0x470323

// block 004702c5
004702c5  push       esi
004702c6  call       0x47c1da

// block 004702cb
004702cb  pop        ecx
004702cc  mov        edx, 0x4adbb0
004702d1  cmp        eax, -1
004702d4  je         0x4702f1

// block 004702d6
004702d6  cmp        eax, -2
004702d9  je         0x4702f1

// block 004702db
004702db  mov        ecx, eax
004702dd  and        ecx, 0x1f
004702e0  mov        esi, eax
004702e2  sar        esi, 5
004702e5  shl        ecx, 6
004702e8  add        ecx, dword ptr [esi*4 + 0x4d6320]
004702ef  jmp        0x4702f3

// block 004702f1
004702f1  mov        ecx, edx

// block 004702f3
004702f3  test       byte ptr [ecx + 0x24], 0x7f
004702f7  jne        0x47028a

// block 004702f9
004702f9  cmp        eax, -1
004702fc  je         0x470317

// block 004702fe
004702fe  cmp        eax, -2
00470301  je         0x470317

// block 00470303
00470303  mov        ecx, eax
00470305  and        eax, 0x1f
00470308  sar        ecx, 5
0047030b  shl        eax, 6
0047030e  add        eax, dword ptr [ecx*4 + 0x4d6320]
00470315  jmp        0x470319

// block 00470317
00470317  mov        eax, edx

// block 00470319
00470319  test       byte ptr [eax + 0x24], 0x80
0047031d  jne        0x47028a

// block 00470323
00470323  xor        ecx, ecx
00470325  cmp        ebx, ecx
00470327  je         0x47028a

// block 0047032d
0047032d  mov        dl, byte ptr [ebx]
0047032f  mov        dword ptr [ebp - 0x228], ecx
00470335  mov        dword ptr [ebp - 0x220], ecx
0047033b  mov        dword ptr [ebp - 0x244], ecx
00470341  mov        byte ptr [ebp - 0x211], dl
00470347  test       dl, dl
00470349  je         0x470d6e

// block 0047034f
0047034f  inc        ebx
00470350  cmp        dword ptr [ebp - 0x228], 0
00470357  mov        dword ptr [ebp - 0x23c], ebx
0047035d  jl         0x470d6e

// block 00470363
00470363  mov        al, dl
00470365  sub        al, 0x20
00470367  cmp        al, 0x58
00470369  ja         0x47037c

// block 0047036b
0047036b  movsx      eax, dl
0047036e  movsx      eax, byte ptr [eax + 0x49cd88]
00470375  and        eax, 0xf
00470378  xor        esi, esi
0047037a  jmp        0x470380

// block 0047037c
0047037c  xor        esi, esi
0047037e  xor        eax, eax

// block 00470380
00470380  movsx      eax, byte ptr [ecx + eax*8 + 0x49cda8]
00470388  push       7
0047038a  sar        eax, 4
0047038d  pop        ecx
0047038e  mov        dword ptr [ebp - 0x26c], eax
00470394  cmp        eax, ecx
00470396  ja         0x470d49

// block 0047039c
0047039c  jmp        dword ptr [eax*4 + 0x470d97]

// block 004703a3
004703a3  or         dword ptr [ebp - 0x218], 0xffffffff
004703aa  mov        dword ptr [ebp - 0x270], esi
004703b0  mov        dword ptr [ebp - 0x240], esi
004703b6  mov        dword ptr [ebp - 0x234], esi
004703bc  mov        dword ptr [ebp - 0x230], esi
004703c2  mov        dword ptr [ebp - 0x210], esi
004703c8  mov        dword ptr [ebp - 0x238], esi
004703ce  jmp        0x470d49

// block 004703d3
004703d3  movsx      eax, dl
004703d6  sub        eax, 0x20
004703d9  je         0x470425

// block 004703db
004703db  sub        eax, 3
004703de  je         0x470416

// block 004703e0
004703e0  sub        eax, 8
004703e3  je         0x47040a

// block 004703e5
004703e5  dec        eax
004703e6  dec        eax
004703e7  je         0x4703fe

// block 004703e9
004703e9  sub        eax, 3
004703ec  jne        0x470d49

// block 004703f2
004703f2  or         dword ptr [ebp - 0x210], 8
004703f9  jmp        0x470d49

// block 004703fe
004703fe  or         dword ptr [ebp - 0x210], 4
00470405  jmp        0x470d49

// block 0047040a
0047040a  or         dword ptr [ebp - 0x210], 1
00470411  jmp        0x470d49

// block 00470416
00470416  or         dword ptr [ebp - 0x210], 0x80
00470420  jmp        0x470d49

// block 00470425
00470425  or         dword ptr [ebp - 0x210], 2
0047042c  jmp        0x470d49

// block 00470431
00470431  cmp        dl, 0x2a
00470434  jne        0x470462

// block 00470436
00470436  add        edi, 4
00470439  mov        dword ptr [ebp - 0x224], edi
0047043f  mov        edi, dword ptr [edi - 4]
00470442  cmp        edi, esi
00470444  mov        dword ptr [ebp - 0x234], edi
0047044a  jge        0x470d49

// block 00470450
00470450  or         dword ptr [ebp - 0x210], 4
00470457  neg        dword ptr [ebp - 0x234]
0047045d  jmp        0x470d49

// block 00470462
00470462  mov        eax, dword ptr [ebp - 0x234]
00470468  imul       eax, eax, 0xa
0047046b  movsx      ecx, dl
0047046e  lea        eax, [eax + ecx - 0x30]
00470472  mov        dword ptr [ebp - 0x234], eax
00470478  jmp        0x470d49

// block 0047047d
0047047d  mov        dword ptr [ebp - 0x218], esi
00470483  jmp        0x470d49

// block 00470488
00470488  cmp        dl, 0x2a
0047048b  jne        0x4704b3

// block 0047048d
0047048d  add        edi, 4
00470490  mov        dword ptr [ebp - 0x224], edi
00470496  mov        edi, dword ptr [edi - 4]
00470499  cmp        edi, esi
0047049b  mov        dword ptr [ebp - 0x218], edi
004704a1  jge        0x470d49

// block 004704a7
004704a7  or         dword ptr [ebp - 0x218], 0xffffffff
004704ae  jmp        0x470d49

// block 004704b3
004704b3  mov        eax, dword ptr [ebp - 0x218]
004704b9  imul       eax, eax, 0xa
004704bc  movsx      ecx, dl
004704bf  lea        eax, [eax + ecx - 0x30]
004704c3  mov        dword ptr [ebp - 0x218], eax
004704c9  jmp        0x470d49

// block 004704ce
004704ce  cmp        dl, 0x49
004704d1  je         0x470528

// block 004704d3
004704d3  cmp        dl, 0x68
004704d6  je         0x47051c

// block 004704d8
004704d8  cmp        dl, 0x6c
004704db  je         0x4704f5

// block 004704dd
004704dd  cmp        dl, 0x77
004704e0  jne        0x470d49

// block 004704e6
004704e6  or         dword ptr [ebp - 0x210], 0x800
004704f0  jmp        0x470d49

// block 004704f5
004704f5  cmp        byte ptr [ebx], 0x6c
004704f8  jne        0x470510

// block 004704fa
004704fa  inc        ebx
004704fb  or         dword ptr [ebp - 0x210], 0x1000
00470505  mov        dword ptr [ebp - 0x23c], ebx
0047050b  jmp        0x470d49

// block 00470510
00470510  or         dword ptr [ebp - 0x210], 0x10
00470517  jmp        0x470d49

// block 0047051c
0047051c  or         dword ptr [ebp - 0x210], 0x20
00470523  jmp        0x470d49

// block 00470528
00470528  mov        al, byte ptr [ebx]
0047052a  cmp        al, 0x36
0047052c  jne        0x47054b

// block 0047052e
0047052e  cmp        byte ptr [ebx + 1], 0x34
00470532  jne        0x47054b

// block 00470534
00470534  inc        ebx
00470535  inc        ebx
00470536  or         dword ptr [ebp - 0x210], 0x8000
00470540  mov        dword ptr [ebp - 0x23c], ebx
00470546  jmp        0x470d49

// block 0047054b
0047054b  cmp        al, 0x33
0047054d  jne        0x47056c

// block 0047054f
0047054f  cmp        byte ptr [ebx + 1], 0x32
00470553  jne        0x47056c

// block 00470555
00470555  inc        ebx
00470556  inc        ebx
00470557  and        dword ptr [ebp - 0x210], 0xffff7fff
00470561  mov        dword ptr [ebp - 0x23c], ebx
00470567  jmp        0x470d49

// block 0047056c
0047056c  cmp        al, 0x64
0047056e  je         0x470d49

// block 00470574
00470574  cmp        al, 0x69
00470576  je         0x470d49

// block 0047057c
0047057c  cmp        al, 0x6f
0047057e  je         0x470d49

// block 00470584
00470584  cmp        al, 0x75
00470586  je         0x470d49

// block 0047058c
0047058c  cmp        al, 0x78
0047058e  je         0x470d49

// block 00470594
00470594  cmp        al, 0x58
00470596  je         0x470d49

// block 0047059c
0047059c  mov        dword ptr [ebp - 0x26c], esi
004705a2  lea        eax, [ebp - 0x25c]
004705a8  push       eax
004705a9  movzx      eax, dl
004705ac  push       eax
004705ad  mov        dword ptr [ebp - 0x238], esi
004705b3  call       0x47c5a3

// block 004705b8
004705b8  pop        ecx
004705b9  test       eax, eax
004705bb  mov        al, byte ptr [ebp - 0x211]
004705c1  pop        ecx
004705c2  je         0x4705e6

// block 004705c4
004705c4  mov        ecx, dword ptr [ebp - 0x24c]
004705ca  lea        esi, [ebp - 0x228]
004705d0  call       0x47013f

// block 004705d5
004705d5  mov        al, byte ptr [ebx]
004705d7  inc        ebx
004705d8  mov        dword ptr [ebp - 0x23c], ebx
004705de  test       al, al
004705e0  je         0x47028a

// block 004705e6
004705e6  mov        ecx, dword ptr [ebp - 0x24c]
004705ec  lea        esi, [ebp - 0x228]
004705f2  call       0x47013f

// block 004705f7
004705f7  jmp        0x470d49

// block 004705fc
004705fc  movsx      eax, dl
004705ff  cmp        eax, 0x64
00470602  jg         0x4707f0

// block 00470608
00470608  je         0x470887

// block 0047060e
0047060e  cmp        eax, 0x53
00470611  jg         0x470709

// block 00470617
00470617  je         0x47069d

// block 0047061d
0047061d  sub        eax, 0x41
00470620  je         0x470632

// block 00470622
00470622  dec        eax
00470623  dec        eax
00470624  je         0x47067e

// block 00470626
00470626  dec        eax
00470627  dec        eax
00470628  je         0x470632

// block 0047062a
0047062a  dec        eax
0047062b  dec        eax
0047062c  jne        0x470bc4

// block 00470632
00470632  add        dl, 0x20
00470635  mov        dword ptr [ebp - 0x270], 1
0047063f  mov        byte ptr [ebp - 0x211], dl

// block 00470645
00470645  or         dword ptr [ebp - 0x210], 0x40
0047064c  cmp        dword ptr [ebp - 0x218], esi
00470652  lea        ebx, [ebp - 0x20c]
00470658  mov        eax, 0x200
0047065d  mov        dword ptr [ebp - 0x21c], ebx
00470663  mov        dword ptr [ebp - 0x260], eax
00470669  jge        0x4708b7

// block 0047066f
0047066f  mov        dword ptr [ebp - 0x218], 6
00470679  jmp        0x470923

// block 0047067e
0047067e  test       dword ptr [ebp - 0x210], 0x830
00470688  jne        0x470726

// block 0047068e
0047068e  or         dword ptr [ebp - 0x210], 0x800
00470698  jmp        0x470726

// block 0047069d
0047069d  test       dword ptr [ebp - 0x210], 0x830
004706a7  jne        0x4706b3

// block 004706a9
004706a9  or         dword ptr [ebp - 0x210], 0x800

// block 004706b3
004706b3  mov        ecx, dword ptr [ebp - 0x218]
004706b9  cmp        ecx, -1
004706bc  jne        0x4706c3

// block 004706be
004706be  mov        ecx, 0x7fffffff

// block 004706c3
004706c3  add        edi, 4
004706c6  test       dword ptr [ebp - 0x210], 0x810
004706d0  mov        dword ptr [ebp - 0x224], edi
004706d6  mov        edi, dword ptr [edi - 4]
004706d9  mov        dword ptr [ebp - 0x21c], edi
004706df  je         0x470b96

// block 004706e5
004706e5  cmp        edi, esi
004706e7  jne        0x4706f4

// block 004706e9
004706e9  mov        eax, dword ptr [0x4ad3dc]
004706ee  mov        dword ptr [ebp - 0x21c], eax

// block 004706f4
004706f4  mov        eax, dword ptr [ebp - 0x21c]
004706fa  mov        dword ptr [ebp - 0x238], 1
00470704  jmp        0x470b88

// block 00470709
00470709  sub        eax, 0x58
0047070c  je         0x4709ec

// block 00470712
00470712  dec        eax
00470713  dec        eax
00470714  je         0x47078f

// block 00470716
00470716  sub        eax, ecx
00470718  je         0x470645

// block 0047071e
0047071e  dec        eax
0047071f  dec        eax
00470720  jne        0x470bc4

// block 00470726
00470726  add        edi, 4
00470729  test       dword ptr [ebp - 0x210], 0x810
00470733  mov        dword ptr [ebp - 0x224], edi
00470739  je         0x47076b

// block 0047073b
0047073b  movzx      eax, word ptr [edi - 4]
0047073f  push       eax
00470740  push       0x200
00470745  lea        eax, [ebp - 0x20c]
0047074b  push       eax
0047074c  lea        eax, [ebp - 0x220]
00470752  push       eax
00470753  call       0x47c503

// block 00470758
00470758  add        esp, 0x10
0047075b  test       eax, eax
0047075d  je         0x47077e

// block 0047075f
0047075f  mov        dword ptr [ebp - 0x240], 1
00470769  jmp        0x47077e

// block 0047076b
0047076b  mov        al, byte ptr [edi - 4]
0047076e  mov        byte ptr [ebp - 0x20c], al
00470774  mov        dword ptr [ebp - 0x220], 1

// block 0047077e
0047077e  lea        eax, [ebp - 0x20c]
00470784  mov        dword ptr [ebp - 0x21c], eax
0047078a  jmp        0x470bc4

// block 0047078f
0047078f  mov        eax, dword ptr [edi]
00470791  add        edi, 4
00470794  mov        dword ptr [ebp - 0x224], edi
0047079a  cmp        eax, esi
0047079c  je         0x4707d9

// block 0047079e
0047079e  mov        ecx, dword ptr [eax + 4]
004707a1  cmp        ecx, esi
004707a3  je         0x4707d9

// block 004707a5
004707a5  test       dword ptr [ebp - 0x210], 0x800
004707af  movsx      eax, word ptr [eax]
004707b2  mov        dword ptr [ebp - 0x21c], ecx
004707b8  je         0x4707ce

// block 004707ba
004707ba  cdq        
004707bb  sub        eax, edx
004707bd  sar        eax, 1
004707bf  mov        dword ptr [ebp - 0x238], 1
004707c9  jmp        0x470bbe

// block 004707ce
004707ce  mov        dword ptr [ebp - 0x238], esi
004707d4  jmp        0x470bbe

// block 004707d9
004707d9  mov        eax, dword ptr [0x4ad3d8]
004707de  mov        dword ptr [ebp - 0x21c], eax
004707e4  push       eax

// block 004707e5
004707e5  call       0x475860

// block 004707ea
004707ea  pop        ecx
004707eb  jmp        0x470bbe

// block 004707f0
004707f0  cmp        eax, 0x70
004707f3  jg         0x4709f4

// block 004707f9
004707f9  je         0x4709e2

// block 004707ff
004707ff  cmp        eax, 0x65
00470802  jl         0x470bc4

// block 00470808
00470808  cmp        eax, 0x67
0047080b  jle        0x470645

// block 00470811
00470811  cmp        eax, 0x69
00470814  je         0x470887

// block 00470816
00470816  cmp        eax, 0x6e
00470819  je         0x470843

// block 0047081b
0047081b  cmp        eax, 0x6f
0047081e  jne        0x470bc4

// block 00470824
00470824  test       byte ptr [ebp - 0x210], 0x80
0047082b  mov        dword ptr [ebp - 0x220], 8
00470835  je         0x470898

// block 00470837
00470837  or         dword ptr [ebp - 0x210], 0x200
00470841  jmp        0x470898

// block 00470843
00470843  mov        esi, dword ptr [edi]
00470845  add        edi, 4
00470848  mov        dword ptr [ebp - 0x224], edi
0047084e  call       0x47c381

// block 00470853
00470853  test       eax, eax
00470855  je         0x47028a

// block 0047085b
0047085b  test       byte ptr [ebp - 0x210], 0x20
00470862  je         0x470870

// block 00470864
00470864  mov        ax, word ptr [ebp - 0x228]
0047086b  mov        word ptr [esi], ax
0047086e  jmp        0x470878

// block 00470870
00470870  mov        eax, dword ptr [ebp - 0x228]
00470876  mov        dword ptr [esi], eax

// block 00470878
00470878  mov        dword ptr [ebp - 0x240], 1
00470882  jmp        0x470d2d

// block 00470887
00470887  or         dword ptr [ebp - 0x210], 0x40

// block 0047088e
0047088e  mov        dword ptr [ebp - 0x220], 0xa

// block 00470898
00470898  mov        ecx, dword ptr [ebp - 0x210]
0047089e  test       ecx, 0x8000
004708a4  je         0x470a53

// block 004708aa
004708aa  mov        eax, dword ptr [edi]
004708ac  mov        edx, dword ptr [edi + 4]
004708af  add        edi, 8
004708b2  jmp        0x470a8c

// block 004708b7
004708b7  jne        0x4708ca

// block 004708b9
004708b9  cmp        dl, 0x67
004708bc  jne        0x470923

// block 004708be
004708be  mov        dword ptr [ebp - 0x218], 1
004708c8  jmp        0x470923

// block 004708ca
004708ca  cmp        dword ptr [ebp - 0x218], eax
004708d0  jle        0x4708d8

// block 004708d2
004708d2  mov        dword ptr [ebp - 0x218], eax

// block 004708d8
004708d8  cmp        dword ptr [ebp - 0x218], 0xa3
004708e2  jle        0x470923

// block 004708e4
004708e4  mov        esi, dword ptr [ebp - 0x218]
004708ea  add        esi, 0x15d
004708f0  push       esi
004708f1  call       0x4777ce

// block 004708f6
004708f6  mov        dl, byte ptr [ebp - 0x211]
004708fc  pop        ecx
004708fd  mov        dword ptr [ebp - 0x244], eax
00470903  test       eax, eax
00470905  je         0x470917

// block 00470907
00470907  mov        dword ptr [ebp - 0x21c], eax
0047090d  mov        dword ptr [ebp - 0x260], esi
00470913  mov        ebx, eax
00470915  jmp        0x470921

// block 00470917
00470917  mov        dword ptr [ebp - 0x218], 0xa3

// block 00470921
00470921  xor        esi, esi

// block 00470923
00470923  mov        eax, dword ptr [edi]
00470925  add        edi, 8
00470928  mov        dword ptr [ebp - 0x278], eax
0047092e  mov        eax, dword ptr [edi - 4]
00470931  mov        dword ptr [ebp - 0x274], eax
00470937  lea        eax, [ebp - 0x25c]
0047093d  push       eax
0047093e  push       dword ptr [ebp - 0x270]
00470944  movsx      eax, dl
00470947  push       dword ptr [ebp - 0x218]
0047094d  mov        dword ptr [ebp - 0x224], edi
00470953  push       eax
00470954  push       dword ptr [ebp - 0x260]
0047095a  lea        eax, [ebp - 0x278]
00470960  push       ebx
00470961  push       eax
00470962  push       dword ptr [0x4ade88]
00470968  call       0x4751de

// block 0047096d
0047096d  pop        ecx
0047096e  call       eax

// block 00470970
00470970  mov        edi, dword ptr [ebp - 0x210]
00470976  add        esp, 0x1c
00470979  and        edi, 0x80
0047097f  je         0x4709a1

// block 00470981
00470981  cmp        dword ptr [ebp - 0x218], esi
00470987  jne        0x4709a1

// block 00470989
00470989  lea        eax, [ebp - 0x25c]
0047098f  push       eax
00470990  push       ebx
00470991  push       dword ptr [0x4ade94]
00470997  call       0x4751de

// block 0047099c
0047099c  pop        ecx
0047099d  call       eax

// block 0047099f
0047099f  pop        ecx
004709a0  pop        ecx

// block 004709a1
004709a1  cmp        byte ptr [ebp - 0x211], 0x67
004709a8  jne        0x4709c6

// block 004709aa
004709aa  cmp        edi, esi
004709ac  jne        0x4709c6

// block 004709ae
004709ae  lea        eax, [ebp - 0x25c]
004709b4  push       eax
004709b5  push       ebx
004709b6  push       dword ptr [0x4ade90]
004709bc  call       0x4751de

// block 004709c1
004709c1  pop        ecx
004709c2  call       eax

// block 004709c4
004709c4  pop        ecx
004709c5  pop        ecx

// block 004709c6
004709c6  cmp        byte ptr [ebx], 0x2d
004709c9  jne        0x4709dc

// block 004709cb
004709cb  or         dword ptr [ebp - 0x210], 0x100
004709d5  inc        ebx
004709d6  mov        dword ptr [ebp - 0x21c], ebx

// block 004709dc
004709dc  push       ebx
004709dd  jmp        0x4707e5

// block 004709e2
004709e2  mov        dword ptr [ebp - 0x218], 8

// block 004709ec
004709ec  mov        dword ptr [ebp - 0x248], ecx
004709f2  jmp        0x470a18

// block 004709f4
004709f4  sub        eax, 0x73
004709f7  je         0x4706b3

// block 004709fd
004709fd  dec        eax
004709fe  dec        eax
004709ff  je         0x47088e

// block 00470a05
00470a05  sub        eax, 3
00470a08  jne        0x470bc4

// block 00470a0e
00470a0e  mov        dword ptr [ebp - 0x248], 0x27

// block 00470a18
00470a18  test       byte ptr [ebp - 0x210], 0x80
00470a1f  mov        dword ptr [ebp - 0x220], 0x10
00470a29  je         0x470898

// block 00470a2f
00470a2f  mov        al, byte ptr [ebp - 0x248]
00470a35  add        al, 0x51
00470a37  mov        byte ptr [ebp - 0x22c], 0x30
00470a3e  mov        byte ptr [ebp - 0x22b], al
00470a44  mov        dword ptr [ebp - 0x230], 2
00470a4e  jmp        0x470898

// block 00470a53
00470a53  test       ecx, 0x1000
00470a59  jne        0x4708aa

// block 00470a5f
00470a5f  add        edi, 4
00470a62  test       cl, 0x20
00470a65  je         0x470a7f

// block 00470a67
00470a67  mov        dword ptr [ebp - 0x224], edi
00470a6d  test       cl, 0x40
00470a70  je         0x470a78

// block 00470a72
00470a72  movsx      eax, word ptr [edi - 4]
00470a76  jmp        0x470a7c

// block 00470a78
00470a78  movzx      eax, word ptr [edi - 4]

// block 00470a7c
00470a7c  cdq        
00470a7d  jmp        0x470a92

// block 00470a7f
00470a7f  mov        eax, dword ptr [edi - 4]
00470a82  test       cl, 0x40
00470a85  je         0x470a8a

// block 00470a87
00470a87  cdq        
00470a88  jmp        0x470a8c

// block 00470a8a
00470a8a  xor        edx, edx

// block 00470a8c
00470a8c  mov        dword ptr [ebp - 0x224], edi

// block 00470a92
00470a92  test       cl, 0x40
00470a95  je         0x470ab2

// block 00470a97
00470a97  cmp        edx, esi
00470a99  jg         0x470ab2

// block 00470a9b
00470a9b  jl         0x470aa1

// block 00470a9d
00470a9d  cmp        eax, esi
00470a9f  jae        0x470ab2

// block 00470aa1
00470aa1  neg        eax
00470aa3  adc        edx, 0
00470aa6  neg        edx
00470aa8  or         dword ptr [ebp - 0x210], 0x100

// block 00470ab2
00470ab2  test       dword ptr [ebp - 0x210], 0x9000
00470abc  mov        ebx, edx
00470abe  mov        edi, eax
00470ac0  jne        0x470ac4

// block 00470ac2
00470ac2  xor        ebx, ebx

// block 00470ac4
00470ac4  cmp        dword ptr [ebp - 0x218], 0
00470acb  jge        0x470ad9

// block 00470acd
00470acd  mov        dword ptr [ebp - 0x218], 1
00470ad7  jmp        0x470af3

// block 00470ad9
00470ad9  and        dword ptr [ebp - 0x210], 0xfffffff7
00470ae0  mov        eax, 0x200
00470ae5  cmp        dword ptr [ebp - 0x218], eax
00470aeb  jle        0x470af3

// block 00470aed
00470aed  mov        dword ptr [ebp - 0x218], eax

// block 00470af3
00470af3  mov        eax, edi
00470af5  or         eax, ebx
00470af7  jne        0x470aff

// block 00470af9
00470af9  and        dword ptr [ebp - 0x230], eax

// block 00470aff
00470aff  lea        esi, [ebp - 0xd]

// block 00470b02
00470b02  mov        eax, dword ptr [ebp - 0x218]
00470b08  dec        dword ptr [ebp - 0x218]
00470b0e  test       eax, eax
00470b10  jg         0x470b18

// block 00470b12
00470b12  mov        eax, edi
00470b14  or         eax, ebx
00470b16  je         0x470b45

// block 00470b18
00470b18  mov        eax, dword ptr [ebp - 0x220]
00470b1e  cdq        
00470b1f  push       edx
00470b20  push       eax
00470b21  push       ebx
00470b22  push       edi
00470b23  call       0x47c890

// block 00470b28
00470b28  add        ecx, 0x30
00470b2b  cmp        ecx, 0x39
00470b2e  mov        dword ptr [ebp - 0x260], ebx
00470b34  mov        edi, eax
00470b36  mov        ebx, edx
00470b38  jle        0x470b40

// block 00470b3a
00470b3a  add        ecx, dword ptr [ebp - 0x248]

// block 00470b40
00470b40  mov        byte ptr [esi], cl
00470b42  dec        esi
00470b43  jmp        0x470b02

// block 00470b45
00470b45  lea        eax, [ebp - 0xd]
00470b48  sub        eax, esi
00470b4a  inc        esi
00470b4b  test       dword ptr [ebp - 0x210], 0x200
00470b55  mov        dword ptr [ebp - 0x220], eax
00470b5b  mov        dword ptr [ebp - 0x21c], esi
00470b61  je         0x470bc4

// block 00470b63
00470b63  test       eax, eax
00470b65  je         0x470b6e

// block 00470b67
00470b67  mov        ecx, esi
00470b69  cmp        byte ptr [ecx], 0x30
00470b6c  je         0x470bc4

// block 00470b6e
00470b6e  dec        dword ptr [ebp - 0x21c]
00470b74  mov        ecx, dword ptr [ebp - 0x21c]
00470b7a  mov        byte ptr [ecx], 0x30
00470b7d  inc        eax
00470b7e  jmp        0x470bbe

// block 00470b80
00470b80  dec        ecx
00470b81  cmp        word ptr [eax], si
00470b84  je         0x470b8c

// block 00470b86
00470b86  inc        eax
00470b87  inc        eax

// block 00470b88
00470b88  cmp        ecx, esi
00470b8a  jne        0x470b80

// block 00470b8c
00470b8c  sub        eax, dword ptr [ebp - 0x21c]
00470b92  sar        eax, 1
00470b94  jmp        0x470bbe

// block 00470b96
00470b96  cmp        edi, esi
00470b98  jne        0x470ba5

// block 00470b9a
00470b9a  mov        eax, dword ptr [0x4ad3d8]
00470b9f  mov        dword ptr [ebp - 0x21c], eax

// block 00470ba5
00470ba5  mov        eax, dword ptr [ebp - 0x21c]
00470bab  jmp        0x470bb4

// block 00470bad
00470bad  dec        ecx
00470bae  cmp        byte ptr [eax], 0
00470bb1  je         0x470bb8

// block 00470bb3
00470bb3  inc        eax

// block 00470bb4
00470bb4  cmp        ecx, esi
00470bb6  jne        0x470bad

// block 00470bb8
00470bb8  sub        eax, dword ptr [ebp - 0x21c]

// block 00470bbe
00470bbe  mov        dword ptr [ebp - 0x220], eax

// block 00470bc4
00470bc4  cmp        dword ptr [ebp - 0x240], 0
00470bcb  jne        0x470d2d

// block 00470bd1
00470bd1  mov        eax, dword ptr [ebp - 0x210]
00470bd7  test       al, 0x40
00470bd9  je         0x470c0d

// block 00470bdb
00470bdb  test       eax, 0x100
00470be0  je         0x470beb

// block 00470be2
00470be2  mov        byte ptr [ebp - 0x22c], 0x2d
00470be9  jmp        0x470c03

// block 00470beb
00470beb  test       al, 1
00470bed  je         0x470bf8

// block 00470bef
00470bef  mov        byte ptr [ebp - 0x22c], 0x2b
00470bf6  jmp        0x470c03

// block 00470bf8
00470bf8  test       al, 2
00470bfa  je         0x470c0d

// block 00470bfc
00470bfc  mov        byte ptr [ebp - 0x22c], 0x20

// block 00470c03
00470c03  mov        dword ptr [ebp - 0x230], 1

// block 00470c0d
00470c0d  mov        ebx, dword ptr [ebp - 0x234]
00470c13  sub        ebx, dword ptr [ebp - 0x220]
00470c19  sub        ebx, dword ptr [ebp - 0x230]
00470c1f  test       byte ptr [ebp - 0x210], 0xc
00470c26  jne        0x470c3f

// block 00470c28
00470c28  push       dword ptr [ebp - 0x24c]
00470c2e  lea        eax, [ebp - 0x228]
00470c34  push       ebx
00470c35  push       0x20
00470c37  call       0x470172

// block 00470c3c
00470c3c  add        esp, 0xc

// block 00470c3f
00470c3f  push       dword ptr [ebp - 0x230]
00470c45  mov        edi, dword ptr [ebp - 0x24c]
00470c4b  lea        eax, [ebp - 0x228]
00470c51  lea        ecx, [ebp - 0x22c]
00470c57  call       0x470198

// block 00470c5c
00470c5c  test       byte ptr [ebp - 0x210], 8
00470c63  pop        ecx
00470c64  je         0x470c81

// block 00470c66
00470c66  test       byte ptr [ebp - 0x210], 4
00470c6d  jne        0x470c81

// block 00470c6f
00470c6f  push       edi
00470c70  push       ebx
00470c71  push       0x30
00470c73  lea        eax, [ebp - 0x228]
00470c79  call       0x470172

// block 00470c7e
00470c7e  add        esp, 0xc

// block 00470c81
00470c81  cmp        dword ptr [ebp - 0x238], 0
00470c88  mov        eax, dword ptr [ebp - 0x220]
00470c8e  je         0x470cf6

// block 00470c90
00470c90  test       eax, eax
00470c92  jle        0x470cf6

// block 00470c94
00470c94  mov        esi, dword ptr [ebp - 0x21c]
00470c9a  mov        dword ptr [ebp - 0x260], eax

// block 00470ca0
00470ca0  movzx      eax, word ptr [esi]
00470ca3  dec        dword ptr [ebp - 0x260]
00470ca9  push       eax
00470caa  push       6
00470cac  lea        eax, [ebp - 0xc]
00470caf  push       eax
00470cb0  lea        eax, [ebp - 0x268]
00470cb6  inc        esi
00470cb7  push       eax
00470cb8  inc        esi
00470cb9  call       0x47c503

// block 00470cbe
00470cbe  add        esp, 0x10
00470cc1  test       eax, eax
00470cc3  jne        0x470ced

// block 00470cc5
00470cc5  cmp        dword ptr [ebp - 0x268], eax
00470ccb  je         0x470ced

// block 00470ccd
00470ccd  push       dword ptr [ebp - 0x268]
00470cd3  lea        eax, [ebp - 0x228]
00470cd9  lea        ecx, [ebp - 0xc]
00470cdc  call       0x470198

// block 00470ce1
00470ce1  cmp        dword ptr [ebp - 0x260], 0
00470ce8  pop        ecx
00470ce9  jne        0x470ca0

// block 00470ceb
00470ceb  jmp        0x470d09

// block 00470ced
00470ced  or         dword ptr [ebp - 0x228], 0xffffffff
00470cf4  jmp        0x470d09

// block 00470cf6
00470cf6  mov        ecx, dword ptr [ebp - 0x21c]
00470cfc  push       eax
00470cfd  lea        eax, [ebp - 0x228]
00470d03  call       0x470198

// block 00470d08
00470d08  pop        ecx

// block 00470d09
00470d09  cmp        dword ptr [ebp - 0x228], 0
00470d10  jl         0x470d2d

// block 00470d12
00470d12  test       byte ptr [ebp - 0x210], 4
00470d19  je         0x470d2d

// block 00470d1b
00470d1b  push       edi
00470d1c  push       ebx
00470d1d  push       0x20
00470d1f  lea        eax, [ebp - 0x228]
00470d25  call       0x470172

// block 00470d2a
00470d2a  add        esp, 0xc

// block 00470d2d
00470d2d  cmp        dword ptr [ebp - 0x244], 0
00470d34  je         0x470d49

// block 00470d36
00470d36  push       dword ptr [ebp - 0x244]
00470d3c  call       0x46c941

// block 00470d41
00470d41  and        dword ptr [ebp - 0x244], 0
00470d48  pop        ecx

// block 00470d49
00470d49  mov        ebx, dword ptr [ebp - 0x23c]
00470d4f  mov        al, byte ptr [ebx]
00470d51  mov        byte ptr [ebp - 0x211], al
00470d57  test       al, al
00470d59  je         0x470d6e

// block 00470d5b
00470d5b  mov        ecx, dword ptr [ebp - 0x26c]
00470d61  mov        edi, dword ptr [ebp - 0x224]
00470d67  mov        dl, al
00470d69  jmp        0x47034f

// block 00470d6e
00470d6e  cmp        byte ptr [ebp - 0x250], 0
00470d75  je         0x470d81

// block 00470d77
00470d77  mov        eax, dword ptr [ebp - 0x254]
00470d7d  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00470d81
00470d81  mov        eax, dword ptr [ebp - 0x228]

// block 00470d87
00470d87  mov        ecx, dword ptr [ebp - 4]
00470d8a  pop        edi
00470d8b  pop        esi
00470d8c  xor        ecx, ebp
00470d8e  pop        ebx
00470d8f  call       0x46c932

// block 00470d94
00470d94  leave      
00470d95  ret        
