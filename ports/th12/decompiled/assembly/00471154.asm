
// block 00471154
00471154  mov        edi, edi
00471156  push       ebp
00471157  mov        ebp, esp
00471159  sub        esp, 0x8d0
0047115f  mov        eax, dword ptr [0x4ad138]
00471164  xor        eax, ebp
00471166  mov        dword ptr [ebp - 4], eax
00471169  mov        eax, dword ptr [ebp + 0x14]
0047116c  push       ebx
0047116d  mov        ebx, dword ptr [ebp + 0xc]
00471170  push       esi
00471171  mov        esi, dword ptr [ebp + 8]
00471174  push       edi
00471175  push       dword ptr [ebp + 0x10]
00471178  xor        edi, edi
0047117a  lea        ecx, [ebp - 0x23c]
00471180  mov        dword ptr [ebp - 0x270], esi
00471186  mov        dword ptr [ebp - 0x214], eax
0047118c  mov        dword ptr [ebp - 0x274], edi
00471192  mov        dword ptr [ebp - 0x210], edi
00471198  mov        dword ptr [ebp - 0x258], edi
0047119e  mov        dword ptr [ebp - 0x278], edi
004711a4  mov        dword ptr [ebp - 0x260], edi
004711aa  call       0x46d3b4

// block 004711af
004711af  or         dword ptr [ebp - 0x224], 0xffffffff
004711b6  mov        dword ptr [ebp - 0x25c], edi
004711bc  cmp        esi, edi
004711be  jne        0x4711f3

// block 004711c0
004711c0  call       0x46e96f

// block 004711c5
004711c5  push       edi
004711c6  push       edi
004711c7  push       edi
004711c8  push       edi
004711c9  push       edi
004711ca  mov        dword ptr [eax], 0x16
004711d0  call       0x470f2d

// block 004711d5
004711d5  add        esp, 0x14
004711d8  cmp        byte ptr [ebp - 0x230], 0
004711df  je         0x4711eb

// block 004711e1
004711e1  mov        eax, dword ptr [ebp - 0x234]
004711e7  and        dword ptr [eax + 0x70], 0xfffffffd

// block 004711eb
004711eb  or         eax, 0xffffffff
004711ee  jmp        0x4723e4

// block 004711f3
004711f3  test       byte ptr [esi + 0xc], 0x40
004711f7  jne        0x471257

// block 004711f9
004711f9  push       esi
004711fa  call       0x47c1da

// block 004711ff
004711ff  pop        ecx
00471200  mov        edx, 0x4adbb0
00471205  cmp        eax, -1
00471208  je         0x471225

// block 0047120a
0047120a  cmp        eax, -2
0047120d  je         0x471225

// block 0047120f
0047120f  mov        ecx, eax
00471211  and        ecx, 0x1f
00471214  mov        esi, eax
00471216  sar        esi, 5
00471219  shl        ecx, 6
0047121c  add        ecx, dword ptr [esi*4 + 0x4d6320]
00471223  jmp        0x471227

// block 00471225
00471225  mov        ecx, edx

// block 00471227
00471227  test       byte ptr [ecx + 0x24], 0x7f
0047122b  jne        0x4711c0

// block 0047122d
0047122d  cmp        eax, -1
00471230  je         0x47124b

// block 00471232
00471232  cmp        eax, -2
00471235  je         0x47124b

// block 00471237
00471237  mov        ecx, eax
00471239  and        eax, 0x1f
0047123c  sar        ecx, 5
0047123f  shl        eax, 6
00471242  add        eax, dword ptr [ecx*4 + 0x4d6320]
00471249  jmp        0x47124d

// block 0047124b
0047124b  mov        eax, edx

// block 0047124d
0047124d  test       byte ptr [eax + 0x24], 0x80
00471251  jne        0x4711c0

// block 00471257
00471257  cmp        ebx, edi
00471259  je         0x4711c0

// block 0047125f
0047125f  mov        dword ptr [ebp - 0x240], edi
00471265  mov        dword ptr [ebp - 0x290], ebx
0047126b  mov        dword ptr [ebp - 0x228], edi

// block 00471271
00471271  cmp        dword ptr [ebp - 0x228], 1
00471278  jne        0x471288

// block 0047127a
0047127a  cmp        dword ptr [ebp - 0x224], edi
00471280  je         0x4723cb

// block 00471286
00471286  xor        edi, edi

// block 00471288
00471288  mov        eax, dword ptr [ebp - 0x290]
0047128e  or         dword ptr [ebp - 0x254], 0xffffffff
00471295  or         dword ptr [ebp - 0x218], 0xffffffff
0047129c  or         dword ptr [ebp - 0x224], 0xffffffff
004712a3  mov        dword ptr [ebp - 0x268], edi
004712a9  mov        dl, byte ptr [eax]
004712ab  mov        dword ptr [ebp - 0x264], edi
004712b1  mov        dword ptr [ebp - 0x27c], edi
004712b7  mov        dword ptr [ebp - 0x250], edi
004712bd  mov        dword ptr [ebp - 0x220], edi
004712c3  mov        byte ptr [ebp - 0x22c], dl
004712c9  test       dl, dl
004712cb  je         0x4723b5

// block 004712d1
004712d1  mov        esi, eax
004712d3  jmp        0x4712db

// block 004712d5
004712d5  mov        dl, byte ptr [ebp - 0x22c]

// block 004712db
004712db  inc        esi
004712dc  xor        edi, edi
004712de  cmp        dword ptr [ebp - 0x240], edi
004712e4  mov        dword ptr [ebp - 0x244], esi
004712ea  jl         0x47233e

// block 004712f0
004712f0  mov        al, dl
004712f2  sub        al, 0x20
004712f4  cmp        al, 0x58
004712f6  ja         0x471307

// block 004712f8
004712f8  movsx      eax, dl
004712fb  movzx      eax, byte ptr [eax + 0x49d620]
00471302  and        eax, 0xf
00471305  jmp        0x471309

// block 00471307
00471307  xor        eax, eax

// block 00471309
00471309  mov        ecx, dword ptr [ebp - 0x264]
0047130f  imul       eax, eax, 9
00471312  movzx      eax, byte ptr [eax + ecx + 0x49d640]
0047131a  xor        ebx, ebx
0047131c  shr        eax, 4
0047131f  inc        ebx
00471320  mov        dword ptr [ebp - 0x264], eax
00471326  cmp        eax, ebx
00471328  jne        0x471407

// block 0047132e
0047132e  cmp        byte ptr [esi], 0x25
00471331  je         0x4713fa

// block 00471337
00471337  cmp        dword ptr [ebp - 0x224], -1
0047133e  jne        0x471392

// block 00471340
00471340  push       0xa
00471342  lea        eax, [ebp - 0x25c]
00471348  push       eax
00471349  push       esi
0047134a  call       0x4733c6

// block 0047134f
0047134f  add        esp, 0xc
00471352  test       eax, eax
00471354  jle        0x471386

// block 00471356
00471356  mov        eax, dword ptr [ebp - 0x25c]
0047135c  cmp        byte ptr [eax], 0x24
0047135f  jne        0x471386

// block 00471361
00471361  cmp        dword ptr [ebp - 0x228], edi
00471367  jne        0x47137e

// block 00471369
00471369  push       0x640
0047136e  lea        eax, [ebp - 0x8d0]
00471374  push       edi
00471375  push       eax
00471376  call       0x477420

// block 0047137b
0047137b  add        esp, 0xc

// block 0047137e
0047137e  mov        dword ptr [ebp - 0x224], ebx
00471384  jmp        0x47139a

// block 00471386
00471386  mov        dl, byte ptr [ebp - 0x22c]
0047138c  mov        dword ptr [ebp - 0x224], edi

// block 00471392
00471392  cmp        dword ptr [ebp - 0x224], ebx
00471398  jne        0x4713fa

// block 0047139a
0047139a  push       0xa
0047139c  lea        eax, [ebp - 0x25c]
004713a2  push       eax
004713a3  push       esi
004713a4  call       0x4733c6

// block 004713a9
004713a9  mov        ecx, dword ptr [ebp - 0x25c]
004713af  add        esp, 0xc
004713b2  dec        eax
004713b3  lea        edx, [ecx + 1]
004713b6  mov        dword ptr [ebp - 0x218], eax
004713bc  mov        dword ptr [ebp - 0x244], edx
004713c2  cmp        dword ptr [ebp - 0x228], edi
004713c8  jne        0x4713f2

// block 004713ca
004713ca  cmp        eax, edi
004713cc  jl         0x4711c0

// block 004713d2
004713d2  cmp        byte ptr [ecx], 0x24
004713d5  jne        0x4711c0

// block 004713db
004713db  cmp        eax, 0x64
004713de  jge        0x4711c0

// block 004713e4
004713e4  cmp        eax, dword ptr [ebp - 0x254]
004713ea  jle        0x4713f2

// block 004713ec
004713ec  mov        dword ptr [ebp - 0x254], eax

// block 004713f2
004713f2  mov        esi, edx
004713f4  mov        dl, byte ptr [ebp - 0x22c]

// block 004713fa
004713fa  mov        eax, dword ptr [ebp - 0x264]
00471400  jmp        dword ptr [eax*4 + 0x4723f4]

// block 00471407
00471407  cmp        eax, 8
0047140a  je         0x4711c0

// block 00471410
00471410  cmp        eax, 7
00471413  ja         0x472326

// block 00471419
00471419  jmp        0x4713fa

// block 0047141b
0047141b  cmp        dword ptr [ebp - 0x228], edi
00471421  jne        0x47142f

// block 00471423
00471423  cmp        dword ptr [ebp - 0x224], ebx
00471429  je         0x472326

// block 0047142f
0047142f  cmp        dword ptr [ebp - 0x228], ebx
00471435  jne        0x471732

// block 0047143b
0047143b  cmp        dword ptr [ebp - 0x224], -1
00471442  jne        0x471732

// block 00471448
00471448  jmp        0x472326

// block 0047144d
0047144d  or         dword ptr [ebp - 0x220], 0xffffffff
00471454  mov        dword ptr [ebp - 0x280], edi
0047145a  mov        dword ptr [ebp - 0x278], edi
00471460  mov        dword ptr [ebp - 0x250], edi
00471466  mov        dword ptr [ebp - 0x258], edi
0047146c  mov        dword ptr [ebp - 0x210], edi
00471472  mov        dword ptr [ebp - 0x260], edi
00471478  jmp        0x472326

// block 0047147d
0047147d  movsx      eax, dl
00471480  sub        eax, 0x20
00471483  je         0x4714ce

// block 00471485
00471485  sub        eax, 3
00471488  je         0x4714bf

// block 0047148a
0047148a  sub        eax, 8
0047148d  je         0x4714b4

// block 0047148f
0047148f  dec        eax
00471490  dec        eax
00471491  je         0x4714a8

// block 00471493
00471493  sub        eax, 3
00471496  jne        0x472326

// block 0047149c
0047149c  or         dword ptr [ebp - 0x210], 8
004714a3  jmp        0x472326

// block 004714a8
004714a8  or         dword ptr [ebp - 0x210], 4
004714af  jmp        0x472326

// block 004714b4
004714b4  or         dword ptr [ebp - 0x210], ebx
004714ba  jmp        0x472326

// block 004714bf
004714bf  or         dword ptr [ebp - 0x210], 0x80
004714c9  jmp        0x472326

// block 004714ce
004714ce  or         dword ptr [ebp - 0x210], 2
004714d5  jmp        0x472326

// block 004714da
004714da  cmp        dl, 0x2a
004714dd  jne        0x4715a2

// block 004714e3
004714e3  cmp        dword ptr [ebp - 0x224], edi
004714e9  jne        0x471500

// block 004714eb
004714eb  add        dword ptr [ebp - 0x214], 4
004714f2  mov        eax, dword ptr [ebp - 0x214]
004714f8  mov        eax, dword ptr [eax - 4]
004714fb  jmp        0x471582

// block 00471500
00471500  push       0xa
00471502  lea        eax, [ebp - 0x25c]
00471508  push       eax
00471509  push       esi
0047150a  call       0x4733c6

// block 0047150f
0047150f  mov        ecx, dword ptr [ebp - 0x25c]
00471515  add        esp, 0xc
00471518  dec        eax
00471519  lea        edx, [ecx + 1]
0047151c  mov        dword ptr [ebp - 0x244], edx
00471522  cmp        dword ptr [ebp - 0x228], edi
00471528  jne        0x471576

// block 0047152a
0047152a  cmp        eax, edi
0047152c  jl         0x4711c0

// block 00471532
00471532  cmp        byte ptr [ecx], 0x24
00471535  jne        0x4711c0

// block 0047153b
0047153b  cmp        dword ptr [ebp - 0x218], 0x64
00471542  jge        0x4711c0

// block 00471548
00471548  cmp        eax, dword ptr [ebp - 0x254]
0047154e  jle        0x471556

// block 00471550
00471550  mov        dword ptr [ebp - 0x254], eax

// block 00471556
00471556  shl        eax, 4
00471559  lea        ecx, [ebp + eax - 0x8d0]
00471560  cmp        dword ptr [ecx], edi
00471562  je         0x47163f

// block 00471568
00471568  push       dword ptr [ebp - 0x210]
0047156e  push       0x2a
00471570  push       ebx
00471571  jmp        0x471ba7

// block 00471576
00471576  shl        eax, 4
00471579  mov        eax, dword ptr [ebp + eax - 0x8cc]
00471580  mov        eax, dword ptr [eax]

// block 00471582
00471582  cmp        eax, edi
00471584  mov        dword ptr [ebp - 0x250], eax
0047158a  jge        0x472326

// block 00471590
00471590  or         dword ptr [ebp - 0x210], 4
00471597  neg        dword ptr [ebp - 0x250]
0047159d  jmp        0x472326

// block 004715a2
004715a2  mov        eax, dword ptr [ebp - 0x250]
004715a8  imul       eax, eax, 0xa
004715ab  movsx      ecx, dl
004715ae  lea        eax, [eax + ecx - 0x30]
004715b2  mov        dword ptr [ebp - 0x250], eax
004715b8  jmp        0x472326

// block 004715bd
004715bd  mov        dword ptr [ebp - 0x220], edi
004715c3  jmp        0x472326

// block 004715c8
004715c8  cmp        dl, 0x2a
004715cb  jne        0x47165b

// block 004715d1
004715d1  cmp        dword ptr [ebp - 0x224], edi
004715d7  jne        0x4715eb

// block 004715d9
004715d9  add        dword ptr [ebp - 0x214], 4
004715e0  mov        eax, dword ptr [ebp - 0x214]
004715e6  mov        eax, dword ptr [eax - 4]
004715e9  jmp        0x471625

// block 004715eb
004715eb  push       0xa
004715ed  lea        eax, [ebp - 0x25c]
004715f3  push       eax
004715f4  push       esi
004715f5  call       0x4733c6

// block 004715fa
004715fa  mov        ecx, dword ptr [ebp - 0x25c]
00471600  add        esp, 0xc
00471603  dec        eax
00471604  lea        edx, [ecx + 1]
00471607  mov        dword ptr [ebp - 0x244], edx
0047160d  cmp        dword ptr [ebp - 0x228], edi
00471613  je         0x47152a

// block 00471619
00471619  shl        eax, 4
0047161c  mov        eax, dword ptr [ebp + eax - 0x8cc]
00471623  mov        eax, dword ptr [eax]

// block 00471625
00471625  cmp        eax, edi
00471627  mov        dword ptr [ebp - 0x220], eax
0047162d  jge        0x472326

// block 00471633
00471633  or         dword ptr [ebp - 0x220], 0xffffffff
0047163a  jmp        0x472326

// block 0047163f
0047163f  mov        dword ptr [ecx], ebx
00471641  mov        byte ptr [ebp + eax - 0x8c8], 0x2a

// block 00471649
00471649  mov        ecx, dword ptr [ebp - 0x210]
0047164f  mov        dword ptr [ebp + eax - 0x8c4], ecx
00471656  jmp        0x472326

// block 0047165b
0047165b  mov        eax, dword ptr [ebp - 0x220]
00471661  imul       eax, eax, 0xa
00471664  movsx      ecx, dl
00471667  lea        eax, [eax + ecx - 0x30]
0047166b  mov        dword ptr [ebp - 0x220], eax
00471671  jmp        0x472326

// block 00471676
00471676  cmp        dl, 0x49
00471679  je         0x4716d0

// block 0047167b
0047167b  cmp        dl, 0x68
0047167e  je         0x4716c4

// block 00471680
00471680  cmp        dl, 0x6c
00471683  je         0x47169d

// block 00471685
00471685  cmp        dl, 0x77
00471688  jne        0x472326

// block 0047168e
0047168e  or         dword ptr [ebp - 0x210], 0x800
00471698  jmp        0x472326

// block 0047169d
0047169d  cmp        byte ptr [esi], 0x6c
004716a0  jne        0x4716b8

// block 004716a2
004716a2  inc        esi
004716a3  or         dword ptr [ebp - 0x210], 0x1000
004716ad  mov        dword ptr [ebp - 0x244], esi
004716b3  jmp        0x472326

// block 004716b8
004716b8  or         dword ptr [ebp - 0x210], 0x10
004716bf  jmp        0x472326

// block 004716c4
004716c4  or         dword ptr [ebp - 0x210], 0x20
004716cb  jmp        0x472326

// block 004716d0
004716d0  mov        al, byte ptr [esi]
004716d2  cmp        al, 0x36
004716d4  jne        0x4716f3

// block 004716d6
004716d6  cmp        byte ptr [esi + 1], 0x34
004716da  jne        0x4716f3

// block 004716dc
004716dc  inc        esi
004716dd  inc        esi
004716de  or         dword ptr [ebp - 0x210], 0x8000
004716e8  mov        dword ptr [ebp - 0x244], esi
004716ee  jmp        0x472326

// block 004716f3
004716f3  cmp        al, 0x33
004716f5  jne        0x471714

// block 004716f7
004716f7  cmp        byte ptr [esi + 1], 0x32
004716fb  jne        0x471714

// block 004716fd
004716fd  inc        esi
004716fe  inc        esi
004716ff  and        dword ptr [ebp - 0x210], 0xffff7fff
00471709  mov        dword ptr [ebp - 0x244], esi
0047170f  jmp        0x472326

// block 00471714
00471714  cmp        al, 0x64
00471716  je         0x471791

// block 00471718
00471718  cmp        al, 0x69
0047171a  je         0x471791

// block 0047171c
0047171c  cmp        al, 0x6f
0047171e  je         0x471791

// block 00471720
00471720  cmp        al, 0x75
00471722  je         0x471791

// block 00471724
00471724  cmp        al, 0x78
00471726  je         0x471791

// block 00471728
00471728  cmp        al, 0x58
0047172a  je         0x471791

// block 0047172c
0047172c  mov        dword ptr [ebp - 0x264], edi

// block 00471732
00471732  lea        eax, [ebp - 0x23c]
00471738  push       eax
00471739  movzx      eax, dl
0047173c  push       eax
0047173d  mov        dword ptr [ebp - 0x260], edi
00471743  call       0x47c5a3

// block 00471748
00471748  pop        ecx
00471749  test       eax, eax
0047174b  mov        al, byte ptr [ebp - 0x22c]
00471751  pop        ecx
00471752  je         0x47177b

// block 00471754
00471754  mov        ecx, dword ptr [ebp - 0x270]
0047175a  lea        esi, [ebp - 0x240]
00471760  call       0x471099

// block 00471765
00471765  mov        eax, dword ptr [ebp - 0x244]
0047176b  mov        al, byte ptr [eax]
0047176d  inc        dword ptr [ebp - 0x244]
00471773  test       al, al
00471775  je         0x4711c0

// block 0047177b
0047177b  mov        ecx, dword ptr [ebp - 0x270]
00471781  lea        esi, [ebp - 0x240]
00471787  call       0x471099

// block 0047178c
0047178c  jmp        0x472326

// block 00471791
00471791  or         dword ptr [ebp - 0x210], 0x10000
0047179b  jmp        0x472326

// block 004717a0
004717a0  movsx      eax, dl
004717a3  cmp        eax, 0x64
004717a6  jg         0x471a5c

// block 004717ac
004717ac  je         0x471b52

// block 004717b2
004717b2  cmp        eax, 0x53
004717b5  jg         0x47189d

// block 004717bb
004717bb  je         0x471856

// block 004717c1
004717c1  sub        eax, 0x41
004717c4  je         0x4717d9

// block 004717c6
004717c6  push       2
004717c8  pop        esi
004717c9  sub        eax, esi
004717cb  je         0x47183e

// block 004717cd
004717cd  sub        eax, esi
004717cf  je         0x4717d9

// block 004717d1
004717d1  sub        eax, esi
004717d3  jne        0x472190

// block 004717d9
004717d9  add        dl, 0x20
004717dc  mov        dword ptr [ebp - 0x280], ebx
004717e2  mov        byte ptr [ebp - 0x22c], dl

// block 004717e8
004717e8  or         dword ptr [ebp - 0x210], 0x40
004717ef  cmp        dword ptr [ebp - 0x224], ebx
004717f5  jne        0x471bbd

// block 004717fb
004717fb  cmp        dword ptr [ebp - 0x228], edi
00471801  jne        0x471bbd

// block 00471807
00471807  cmp        dword ptr [ebp - 0x218], 0x63
0047180e  ja         0x4711c0

// block 00471814
00471814  mov        eax, dword ptr [ebp - 0x218]
0047181a  shl        eax, 4
0047181d  lea        ecx, [ebp + eax - 0x8d0]
00471824  cmp        dword ptr [ecx], edi
00471826  jne        0x471b99

// block 0047182c
0047182c  mov        dword ptr [ecx], 8
00471832  mov        byte ptr [ebp + eax - 0x8c8], dl
00471839  jmp        0x471649

// block 0047183e
0047183e  test       dword ptr [ebp - 0x210], 0x830
00471848  jne        0x4718c2

// block 0047184a
0047184a  or         dword ptr [ebp - 0x210], 0x800
00471854  jmp        0x4718c2

// block 00471856
00471856  test       dword ptr [ebp - 0x210], 0x830
00471860  jne        0x47186c

// block 00471862
00471862  or         dword ptr [ebp - 0x210], 0x800

// block 0047186c
0047186c  mov        ecx, dword ptr [ebp - 0x220]
00471872  cmp        ecx, -1
00471875  jne        0x47187c

// block 00471877
00471877  mov        ecx, 0x7fffffff

// block 0047187c
0047187c  cmp        dword ptr [ebp - 0x224], edi
00471882  jne        0x4720d6

// block 00471888
00471888  add        dword ptr [ebp - 0x214], 4
0047188f  mov        eax, dword ptr [ebp - 0x214]
00471895  mov        eax, dword ptr [eax - 4]
00471898  jmp        0x4720fd

// block 0047189d
0047189d  sub        eax, 0x58
004718a0  je         0x471d54

// block 004718a6
004718a6  dec        eax
004718a7  dec        eax
004718a8  je         0x4719c5

// block 004718ae
004718ae  sub        eax, 7
004718b1  je         0x4717e8

// block 004718b7
004718b7  dec        eax
004718b8  dec        eax
004718b9  jne        0x472190

// block 004718bf
004718bf  push       2
004718c1  pop        esi

// block 004718c2
004718c2  test       dword ptr [ebp - 0x210], 0x810
004718cc  je         0x471961

// block 004718d2
004718d2  cmp        dword ptr [ebp - 0x224], edi
004718d8  jne        0x4718ed

// block 004718da
004718da  add        dword ptr [ebp - 0x214], 4
004718e1  mov        eax, dword ptr [ebp - 0x214]
004718e7  movzx      eax, word ptr [eax - 4]
004718eb  jmp        0x471939

// block 004718ed
004718ed  cmp        dword ptr [ebp - 0x218], 0x63
004718f4  ja         0x4711c0

// block 004718fa
004718fa  mov        eax, dword ptr [ebp - 0x218]
00471900  shl        eax, 4
00471903  cmp        dword ptr [ebp - 0x228], edi
00471909  jne        0x47192f

// block 0047190b
0047190b  lea        ecx, [ebp + eax - 0x8d0]
00471912  cmp        dword ptr [ecx], edi
00471914  jne        0x47191d

// block 00471916
00471916  mov        dword ptr [ecx], esi
00471918  jmp        0x472132

// block 0047191d
0047191d  push       dword ptr [ebp - 0x210]
00471923  push       dword ptr [ebp - 0x22c]
00471929  push       esi
0047192a  jmp        0x471fb8

// block 0047192f
0047192f  mov        eax, dword ptr [ebp + eax - 0x8cc]
00471936  movzx      eax, word ptr [eax]

// block 00471939
00471939  push       eax
0047193a  push       0x200
0047193f  lea        eax, [ebp - 0x20c]
00471945  push       eax
00471946  lea        eax, [ebp - 0x268]
0047194c  push       eax
0047194d  call       0x47c503

// block 00471952
00471952  add        esp, 0x10
00471955  test       eax, eax
00471957  je         0x4719b4

// block 00471959
00471959  mov        dword ptr [ebp - 0x278], ebx
0047195f  jmp        0x4719b4

// block 00471961
00471961  cmp        dword ptr [ebp - 0x224], edi
00471967  jne        0x47197c

// block 00471969
00471969  add        dword ptr [ebp - 0x214], 4
00471970  mov        eax, dword ptr [ebp - 0x214]
00471976  movzx      eax, word ptr [eax - 4]
0047197a  jmp        0x4719a8

// block 0047197c
0047197c  cmp        dword ptr [ebp - 0x218], 0x63
00471983  ja         0x4711c0

// block 00471989
00471989  mov        eax, dword ptr [ebp - 0x218]
0047198f  shl        eax, 4
00471992  cmp        dword ptr [ebp - 0x228], edi
00471998  je         0x471f99

// block 0047199e
0047199e  mov        eax, dword ptr [ebp + eax - 0x8cc]
004719a5  movzx      eax, word ptr [eax]

// block 004719a8
004719a8  mov        byte ptr [ebp - 0x20c], al
004719ae  mov        dword ptr [ebp - 0x268], ebx

// block 004719b4
004719b4  lea        eax, [ebp - 0x20c]
004719ba  mov        dword ptr [ebp - 0x21c], eax
004719c0  jmp        0x472190

// block 004719c5
004719c5  cmp        dword ptr [ebp - 0x224], edi
004719cb  jne        0x4719df

// block 004719cd
004719cd  add        dword ptr [ebp - 0x214], 4
004719d4  mov        eax, dword ptr [ebp - 0x214]
004719da  mov        eax, dword ptr [eax - 4]
004719dd  jmp        0x471a0a

// block 004719df
004719df  cmp        dword ptr [ebp - 0x218], 0x63
004719e6  ja         0x4711c0

// block 004719ec
004719ec  mov        eax, dword ptr [ebp - 0x218]
004719f2  shl        eax, 4
004719f5  cmp        dword ptr [ebp - 0x228], edi
004719fb  je         0x471af2

// block 00471a01
00471a01  mov        eax, dword ptr [ebp + eax - 0x8cc]
00471a08  mov        eax, dword ptr [eax]

// block 00471a0a
00471a0a  cmp        eax, edi
00471a0c  je         0x471a45

// block 00471a0e
00471a0e  mov        ecx, dword ptr [eax + 4]
00471a11  cmp        ecx, edi
00471a13  je         0x471a45

// block 00471a15
00471a15  test       dword ptr [ebp - 0x210], 0x800
00471a1f  movsx      eax, word ptr [eax]
00471a22  mov        dword ptr [ebp - 0x21c], ecx
00471a28  je         0x471a3a

// block 00471a2a
00471a2a  cdq        
00471a2b  sub        eax, edx
00471a2d  sar        eax, 1
00471a2f  mov        dword ptr [ebp - 0x260], ebx
00471a35  jmp        0x47218a

// block 00471a3a
00471a3a  mov        dword ptr [ebp - 0x260], edi
00471a40  jmp        0x47218a

// block 00471a45
00471a45  mov        eax, dword ptr [0x4ad3d8]
00471a4a  mov        dword ptr [ebp - 0x21c], eax
00471a50  push       eax

// block 00471a51
00471a51  call       0x475860

// block 00471a56
00471a56  pop        ecx
00471a57  jmp        0x47218a

// block 00471a5c
00471a5c  cmp        eax, 0x70
00471a5f  jg         0x471d60

// block 00471a65
00471a65  je         0x471d4a

// block 00471a6b
00471a6b  cmp        eax, 0x65
00471a6e  jl         0x472190

// block 00471a74
00471a74  cmp        eax, 0x67
00471a77  jle        0x4717e8

// block 00471a7d
00471a7d  cmp        eax, 0x69
00471a80  je         0x471b52

// block 00471a86
00471a86  cmp        eax, 0x6e
00471a89  je         0x471aba

// block 00471a8b
00471a8b  cmp        eax, 0x6f
00471a8e  jne        0x472190

// block 00471a94
00471a94  test       byte ptr [ebp - 0x210], 0x80
00471a9b  mov        dword ptr [ebp - 0x24c], 8
00471aa5  je         0x471b63

// block 00471aab
00471aab  or         dword ptr [ebp - 0x210], 0x200
00471ab5  jmp        0x471b63

// block 00471aba
00471aba  cmp        dword ptr [ebp - 0x224], edi
00471ac0  jne        0x471ad4

// block 00471ac2
00471ac2  add        dword ptr [ebp - 0x214], 4
00471ac9  mov        eax, dword ptr [ebp - 0x214]
00471acf  mov        esi, dword ptr [eax - 4]
00471ad2  jmp        0x471b1d

// block 00471ad4
00471ad4  cmp        dword ptr [ebp - 0x218], 0x63
00471adb  ja         0x4711c0

// block 00471ae1
00471ae1  mov        eax, dword ptr [ebp - 0x218]
00471ae7  shl        eax, 4
00471aea  cmp        dword ptr [ebp - 0x228], edi
00471af0  jne        0x471b14

// block 00471af2
00471af2  lea        ecx, [ebp + eax - 0x8d0]
00471af9  cmp        dword ptr [ecx], edi
00471afb  je         0x47212c

// block 00471b01
00471b01  push       dword ptr [ebp - 0x210]
00471b07  push       dword ptr [ebp - 0x22c]
00471b0d  push       3
00471b0f  jmp        0x471fb8

// block 00471b14
00471b14  mov        eax, dword ptr [ebp + eax - 0x8cc]
00471b1b  mov        esi, dword ptr [eax]

// block 00471b1d
00471b1d  call       0x47c381

// block 00471b22
00471b22  test       eax, eax
00471b24  je         0x4711c0

// block 00471b2a
00471b2a  test       byte ptr [ebp - 0x210], 0x20
00471b31  je         0x471b3f

// block 00471b33
00471b33  mov        ax, word ptr [ebp - 0x240]
00471b3a  mov        word ptr [esi], ax
00471b3d  jmp        0x471b47

// block 00471b3f
00471b3f  mov        eax, dword ptr [ebp - 0x240]
00471b45  mov        dword ptr [esi], eax

// block 00471b47
00471b47  mov        dword ptr [ebp - 0x278], ebx
00471b4d  jmp        0x472190

// block 00471b52
00471b52  or         dword ptr [ebp - 0x210], 0x40

// block 00471b59
00471b59  mov        dword ptr [ebp - 0x24c], 0xa

// block 00471b63
00471b63  mov        eax, dword ptr [ebp - 0x210]
00471b69  test       eax, 0x8000
00471b6e  je         0x471e0a

// block 00471b74
00471b74  cmp        dword ptr [ebp - 0x224], edi
00471b7a  jne        0x471dbf

// block 00471b80
00471b80  mov        ecx, dword ptr [ebp - 0x214]
00471b86  mov        eax, dword ptr [ecx]
00471b88  mov        edx, dword ptr [ecx + 4]
00471b8b  add        ecx, 8
00471b8e  mov        dword ptr [ebp - 0x214], ecx
00471b94  jmp        0x471fd9

// block 00471b99
00471b99  push       dword ptr [ebp - 0x210]
00471b9f  push       dword ptr [ebp - 0x22c]
00471ba5  push       8

// block 00471ba7
00471ba7  push       ecx
00471ba8  call       0x470f6e

// block 00471bad
00471bad  add        esp, 0x10
00471bb0  test       eax, eax
00471bb2  je         0x4711c0

// block 00471bb8
00471bb8  jmp        0x472326

// block 00471bbd
00471bbd  cmp        dword ptr [ebp - 0x220], edi
00471bc3  lea        eax, [ebp - 0x20c]
00471bc9  mov        dword ptr [ebp - 0x21c], eax
00471bcf  mov        eax, 0x200
00471bd4  mov        dword ptr [ebp - 0x24c], eax
00471bda  jge        0x471be8

// block 00471bdc
00471bdc  mov        dword ptr [ebp - 0x220], 6
00471be6  jmp        0x471bf5

// block 00471be8
00471be8  jne        0x471c0e

// block 00471bea
00471bea  cmp        dl, 0x67
00471bed  jne        0x471bf5

// block 00471bef
00471bef  mov        dword ptr [ebp - 0x220], ebx

// block 00471bf5
00471bf5  cmp        dword ptr [ebp - 0x224], edi
00471bfb  jne        0x471c5a

// block 00471bfd
00471bfd  mov        eax, dword ptr [ebp - 0x214]
00471c03  add        eax, 8
00471c06  mov        dword ptr [ebp - 0x214], eax
00471c0c  jmp        0x471c7a

// block 00471c0e
00471c0e  cmp        dword ptr [ebp - 0x220], eax
00471c14  jle        0x471c1c

// block 00471c16
00471c16  mov        dword ptr [ebp - 0x220], eax

// block 00471c1c
00471c1c  mov        ebx, 0xa3
00471c21  cmp        dword ptr [ebp - 0x220], ebx
00471c27  jle        0x471bf5

// block 00471c29
00471c29  mov        esi, dword ptr [ebp - 0x220]
00471c2f  add        esi, 0x15d
00471c35  push       esi
00471c36  call       0x4777ce

// block 00471c3b
00471c3b  mov        dl, byte ptr [ebp - 0x22c]
00471c41  pop        ecx
00471c42  mov        dword ptr [ebp - 0x27c], eax
00471c48  cmp        eax, edi
00471c4a  je         0x471bef

// block 00471c4c
00471c4c  mov        dword ptr [ebp - 0x21c], eax
00471c52  mov        dword ptr [ebp - 0x24c], esi
00471c58  jmp        0x471bf5

// block 00471c5a
00471c5a  cmp        dword ptr [ebp - 0x218], 0x63
00471c61  ja         0x4711c0

// block 00471c67
00471c67  mov        eax, dword ptr [ebp - 0x218]
00471c6d  shl        eax, 4
00471c70  mov        eax, dword ptr [ebp + eax - 0x8cc]
00471c77  add        eax, 8

// block 00471c7a
00471c7a  mov        ecx, dword ptr [eax - 8]
00471c7d  mov        dword ptr [ebp - 0x28c], ecx
00471c83  mov        eax, dword ptr [eax - 4]
00471c86  mov        dword ptr [ebp - 0x288], eax
00471c8c  lea        eax, [ebp - 0x23c]
00471c92  push       eax
00471c93  push       dword ptr [ebp - 0x280]
00471c99  movsx      eax, dl
00471c9c  push       dword ptr [ebp - 0x220]
00471ca2  push       eax
00471ca3  push       dword ptr [ebp - 0x24c]
00471ca9  lea        eax, [ebp - 0x28c]
00471caf  push       dword ptr [ebp - 0x21c]
00471cb5  push       eax
00471cb6  push       dword ptr [0x4ade88]
00471cbc  call       0x4751de

// block 00471cc1
00471cc1  pop        ecx
00471cc2  call       eax

// block 00471cc4
00471cc4  mov        esi, dword ptr [ebp - 0x210]
00471cca  add        esp, 0x1c
00471ccd  and        esi, 0x80
00471cd3  je         0x471cfa

// block 00471cd5
00471cd5  cmp        dword ptr [ebp - 0x220], edi
00471cdb  jne        0x471cfa

// block 00471cdd
00471cdd  lea        eax, [ebp - 0x23c]
00471ce3  push       eax
00471ce4  push       dword ptr [ebp - 0x21c]
00471cea  push       dword ptr [0x4ade94]
00471cf0  call       0x4751de

// block 00471cf5
00471cf5  pop        ecx
00471cf6  call       eax

// block 00471cf8
00471cf8  pop        ecx
00471cf9  pop        ecx

// block 00471cfa
00471cfa  cmp        byte ptr [ebp - 0x22c], 0x67
00471d01  jne        0x471d24

// block 00471d03
00471d03  cmp        esi, edi
00471d05  jne        0x471d24

// block 00471d07
00471d07  lea        eax, [ebp - 0x23c]
00471d0d  push       eax
00471d0e  push       dword ptr [ebp - 0x21c]
00471d14  push       dword ptr [0x4ade90]
00471d1a  call       0x4751de

// block 00471d1f
00471d1f  pop        ecx
00471d20  call       eax

// block 00471d22
00471d22  pop        ecx
00471d23  pop        ecx

// block 00471d24
00471d24  mov        eax, dword ptr [ebp - 0x21c]
00471d2a  cmp        byte ptr [eax], 0x2d
00471d2d  jne        0x471d3f

// block 00471d2f
00471d2f  or         dword ptr [ebp - 0x210], 0x100
00471d39  inc        dword ptr [ebp - 0x21c]

// block 00471d3f
00471d3f  push       dword ptr [ebp - 0x21c]
00471d45  jmp        0x471a51

// block 00471d4a
00471d4a  mov        dword ptr [ebp - 0x220], 8

// block 00471d54
00471d54  mov        dword ptr [ebp - 0x274], 7
00471d5e  jmp        0x471d84

// block 00471d60
00471d60  sub        eax, 0x73
00471d63  je         0x47186c

// block 00471d69
00471d69  dec        eax
00471d6a  dec        eax
00471d6b  je         0x471b59

// block 00471d71
00471d71  sub        eax, 3
00471d74  jne        0x472190

// block 00471d7a
00471d7a  mov        dword ptr [ebp - 0x274], 0x27

// block 00471d84
00471d84  test       byte ptr [ebp - 0x210], 0x80
00471d8b  mov        dword ptr [ebp - 0x24c], 0x10
00471d95  je         0x471b63

// block 00471d9b
00471d9b  mov        al, byte ptr [ebp - 0x274]
00471da1  add        al, 0x51
00471da3  mov        byte ptr [ebp - 0x248], 0x30
00471daa  mov        byte ptr [ebp - 0x247], al
00471db0  mov        dword ptr [ebp - 0x258], 2
00471dba  jmp        0x471b63

// block 00471dbf
00471dbf  cmp        dword ptr [ebp - 0x218], 0x63
00471dc6  ja         0x4711c0

// block 00471dcc
00471dcc  mov        eax, dword ptr [ebp - 0x218]
00471dd2  shl        eax, 4
00471dd5  cmp        dword ptr [ebp - 0x228], edi
00471ddb  jne        0x471e64

// block 00471de1
00471de1  lea        ecx, [ebp + eax - 0x8d0]
00471de8  cmp        dword ptr [ecx], edi
00471dea  jne        0x471df7

// block 00471dec
00471dec  mov        dword ptr [ecx], 4
00471df2  jmp        0x472132

// block 00471df7
00471df7  push       dword ptr [ebp - 0x210]
00471dfd  push       dword ptr [ebp - 0x22c]
00471e03  push       4
00471e05  jmp        0x471fb8

// block 00471e0a
00471e0a  test       eax, 0x1000
00471e0f  je         0x471e75

// block 00471e11
00471e11  cmp        dword ptr [ebp - 0x224], edi
00471e17  je         0x471b80

// block 00471e1d
00471e1d  cmp        dword ptr [ebp - 0x218], 0x63
00471e24  ja         0x4711c0

// block 00471e2a
00471e2a  mov        eax, dword ptr [ebp - 0x218]
00471e30  shl        eax, 4
00471e33  cmp        dword ptr [ebp - 0x228], edi
00471e39  jne        0x471e64

// block 00471e3b
00471e3b  lea        ecx, [ebp + eax - 0x8d0]
00471e42  cmp        dword ptr [ecx], edi
00471e44  jne        0x471e51

// block 00471e46
00471e46  mov        dword ptr [ecx], 5
00471e4c  jmp        0x472132

// block 00471e51
00471e51  push       dword ptr [ebp - 0x210]
00471e57  push       dword ptr [ebp - 0x22c]
00471e5d  push       5
00471e5f  jmp        0x471fb8

// block 00471e64
00471e64  mov        ecx, dword ptr [ebp + eax - 0x8cc]
00471e6b  mov        eax, dword ptr [ecx]
00471e6d  mov        edx, dword ptr [ecx + 4]
00471e70  jmp        0x471fd9

// block 00471e75
00471e75  test       al, 0x20
00471e77  je         0x471f19

// block 00471e7d
00471e7d  test       al, 0x40
00471e7f  je         0x471ed0

// block 00471e81
00471e81  cmp        dword ptr [ebp - 0x224], edi
00471e87  jne        0x471e9f

// block 00471e89
00471e89  add        dword ptr [ebp - 0x214], 4
00471e90  mov        eax, dword ptr [ebp - 0x214]
00471e96  movsx      eax, word ptr [eax - 4]
00471e9a  jmp        0x471f5e

// block 00471e9f
00471e9f  cmp        dword ptr [ebp - 0x218], 0x63
00471ea6  ja         0x4711c0

// block 00471eac
00471eac  mov        eax, dword ptr [ebp - 0x218]
00471eb2  shl        eax, 4
00471eb5  cmp        dword ptr [ebp - 0x228], edi
00471ebb  je         0x471f99

// block 00471ec1
00471ec1  mov        eax, dword ptr [ebp + eax - 0x8cc]
00471ec8  movsx      eax, word ptr [eax]
00471ecb  jmp        0x471f5e

// block 00471ed0
00471ed0  cmp        dword ptr [ebp - 0x224], edi
00471ed6  jne        0x471eeb

// block 00471ed8
00471ed8  add        dword ptr [ebp - 0x214], 4
00471edf  mov        eax, dword ptr [ebp - 0x214]
00471ee5  movzx      eax, word ptr [eax - 4]
00471ee9  jmp        0x471f5e

// block 00471eeb
00471eeb  cmp        dword ptr [ebp - 0x218], 0x63
00471ef2  ja         0x4711c0

// block 00471ef8
00471ef8  mov        eax, dword ptr [ebp - 0x218]
00471efe  shl        eax, 4
00471f01  cmp        dword ptr [ebp - 0x228], edi
00471f07  je         0x471f99

// block 00471f0d
00471f0d  mov        eax, dword ptr [ebp + eax - 0x8cc]
00471f14  movzx      eax, word ptr [eax]
00471f17  jmp        0x471f5e

// block 00471f19
00471f19  test       al, 0x40
00471f1b  je         0x471f61

// block 00471f1d
00471f1d  cmp        dword ptr [ebp - 0x224], edi
00471f23  jne        0x471f37

// block 00471f25
00471f25  add        dword ptr [ebp - 0x214], 4
00471f2c  mov        eax, dword ptr [ebp - 0x214]
00471f32  mov        eax, dword ptr [eax - 4]
00471f35  jmp        0x471f5e

// block 00471f37
00471f37  cmp        dword ptr [ebp - 0x218], 0x63
00471f3e  ja         0x4711c0

// block 00471f44
00471f44  mov        eax, dword ptr [ebp - 0x218]
00471f4a  shl        eax, 4
00471f4d  cmp        dword ptr [ebp - 0x228], edi
00471f53  je         0x471f99

// block 00471f55
00471f55  mov        eax, dword ptr [ebp + eax - 0x8cc]
00471f5c  mov        eax, dword ptr [eax]

// block 00471f5e
00471f5e  cdq        
00471f5f  jmp        0x471fd9

// block 00471f61
00471f61  cmp        dword ptr [ebp - 0x224], edi
00471f67  jne        0x471f7b

// block 00471f69
00471f69  add        dword ptr [ebp - 0x214], 4
00471f70  mov        eax, dword ptr [ebp - 0x214]
00471f76  mov        eax, dword ptr [eax - 4]
00471f79  jmp        0x471fd7

// block 00471f7b
00471f7b  cmp        dword ptr [ebp - 0x218], 0x63
00471f82  ja         0x4711c0

// block 00471f88
00471f88  mov        eax, dword ptr [ebp - 0x218]
00471f8e  shl        eax, 4
00471f91  cmp        dword ptr [ebp - 0x228], edi
00471f97  jne        0x471fce

// block 00471f99
00471f99  lea        ecx, [ebp + eax - 0x8d0]
00471fa0  cmp        dword ptr [ecx], edi
00471fa2  jne        0x471fab

// block 00471fa4
00471fa4  mov        dword ptr [ecx], ebx
00471fa6  jmp        0x472132

// block 00471fab
00471fab  push       dword ptr [ebp - 0x210]
00471fb1  push       dword ptr [ebp - 0x22c]
00471fb7  push       ebx

// block 00471fb8
00471fb8  push       ecx
00471fb9  call       0x470f6e

// block 00471fbe
00471fbe  add        esp, 0x10
00471fc1  test       eax, eax
00471fc3  je         0x4711c0

// block 00471fc9
00471fc9  jmp        0x472190

// block 00471fce
00471fce  mov        eax, dword ptr [ebp + eax - 0x8cc]
00471fd5  mov        eax, dword ptr [eax]

// block 00471fd7
00471fd7  xor        edx, edx

// block 00471fd9
00471fd9  test       byte ptr [ebp - 0x210], 0x40
00471fe0  je         0x471ffd

// block 00471fe2
00471fe2  cmp        edx, edi
00471fe4  jg         0x471ffd

// block 00471fe6
00471fe6  jl         0x471fec

// block 00471fe8
00471fe8  cmp        eax, edi
00471fea  jae        0x471ffd

// block 00471fec
00471fec  neg        eax
00471fee  adc        edx, 0
00471ff1  neg        edx
00471ff3  or         dword ptr [ebp - 0x210], 0x100

// block 00471ffd
00471ffd  test       dword ptr [ebp - 0x210], 0x9000
00472007  mov        ebx, edx
00472009  mov        edi, eax
0047200b  jne        0x47200f

// block 0047200d
0047200d  xor        ebx, ebx

// block 0047200f
0047200f  cmp        dword ptr [ebp - 0x220], 0
00472016  jge        0x472024

// block 00472018
00472018  mov        dword ptr [ebp - 0x220], 1
00472022  jmp        0x47203e

// block 00472024
00472024  and        dword ptr [ebp - 0x210], 0xfffffff7
0047202b  mov        eax, 0x200
00472030  cmp        dword ptr [ebp - 0x220], eax
00472036  jle        0x47203e

// block 00472038
00472038  mov        dword ptr [ebp - 0x220], eax

// block 0047203e
0047203e  mov        eax, edi
00472040  or         eax, ebx
00472042  jne        0x47204a

// block 00472044
00472044  and        dword ptr [ebp - 0x258], eax

// block 0047204a
0047204a  lea        esi, [ebp - 0xd]

// block 0047204d
0047204d  mov        eax, dword ptr [ebp - 0x220]
00472053  dec        dword ptr [ebp - 0x220]
00472059  test       eax, eax
0047205b  jg         0x472063

// block 0047205d
0047205d  mov        eax, edi
0047205f  or         eax, ebx
00472061  je         0x472090

// block 00472063
00472063  mov        eax, dword ptr [ebp - 0x24c]
00472069  cdq        
0047206a  push       edx
0047206b  push       eax
0047206c  push       ebx
0047206d  push       edi
0047206e  call       0x47c890

// block 00472073
00472073  add        ecx, 0x30
00472076  cmp        ecx, 0x39
00472079  mov        dword ptr [ebp - 0x268], ebx
0047207f  mov        edi, eax
00472081  mov        ebx, edx
00472083  jle        0x47208b

// block 00472085
00472085  add        ecx, dword ptr [ebp - 0x274]

// block 0047208b
0047208b  mov        byte ptr [esi], cl
0047208d  dec        esi
0047208e  jmp        0x47204d

// block 00472090
00472090  lea        eax, [ebp - 0xd]
00472093  sub        eax, esi
00472095  inc        esi
00472096  test       dword ptr [ebp - 0x210], 0x200
004720a0  mov        dword ptr [ebp - 0x268], eax
004720a6  mov        dword ptr [ebp - 0x21c], esi
004720ac  je         0x472190

// block 004720b2
004720b2  test       eax, eax
004720b4  je         0x4720c1

// block 004720b6
004720b6  mov        ecx, esi
004720b8  cmp        byte ptr [ecx], 0x30
004720bb  je         0x472190

// block 004720c1
004720c1  dec        dword ptr [ebp - 0x21c]
004720c7  mov        ecx, dword ptr [ebp - 0x21c]
004720cd  mov        byte ptr [ecx], 0x30
004720d0  inc        eax
004720d1  jmp        0x47218a

// block 004720d6
004720d6  mov        eax, dword ptr [ebp - 0x218]
004720dc  cmp        eax, 0x63
004720df  ja         0x4711c0

// block 004720e5
004720e5  shl        eax, 4
004720e8  cmp        dword ptr [ebp - 0x228], edi
004720ee  je         0x471af2

// block 004720f4
004720f4  mov        eax, dword ptr [ebp + eax - 0x8cc]
004720fb  mov        eax, dword ptr [eax]

// block 004720fd
004720fd  test       dword ptr [ebp - 0x210], 0x810
00472107  mov        dword ptr [ebp - 0x21c], eax
0047210d  je         0x47215e

// block 0047210f
0047210f  cmp        eax, edi
00472111  jne        0x47211e

// block 00472113
00472113  mov        eax, dword ptr [0x4ad3dc]
00472118  mov        dword ptr [ebp - 0x21c], eax

// block 0047211e
0047211e  mov        eax, dword ptr [ebp - 0x21c]
00472124  mov        dword ptr [ebp - 0x260], ebx
0047212a  jmp        0x472150

// block 0047212c
0047212c  mov        dword ptr [ecx], 3

// block 00472132
00472132  mov        ecx, dword ptr [ebp - 0x210]
00472138  mov        byte ptr [ebp + eax - 0x8c8], dl
0047213f  mov        dword ptr [ebp + eax - 0x8c4], ecx
00472146  jmp        0x472190

// block 00472148
00472148  dec        ecx
00472149  cmp        word ptr [eax], di
0047214c  je         0x472154

// block 0047214e
0047214e  inc        eax
0047214f  inc        eax

// block 00472150
00472150  cmp        ecx, edi
00472152  jne        0x472148

// block 00472154
00472154  sub        eax, dword ptr [ebp - 0x21c]
0047215a  sar        eax, 1
0047215c  jmp        0x47218a

// block 0047215e
0047215e  cmp        dword ptr [ebp - 0x21c], edi
00472164  jne        0x472171

// block 00472166
00472166  mov        eax, dword ptr [0x4ad3d8]
0047216b  mov        dword ptr [ebp - 0x21c], eax

// block 00472171
00472171  mov        eax, dword ptr [ebp - 0x21c]
00472177  jmp        0x472180

// block 00472179
00472179  dec        ecx
0047217a  cmp        byte ptr [eax], 0
0047217d  je         0x472184

// block 0047217f
0047217f  inc        eax

// block 00472180
00472180  cmp        ecx, edi
00472182  jne        0x472179

// block 00472184
00472184  sub        eax, dword ptr [ebp - 0x21c]

// block 0047218a
0047218a  mov        dword ptr [ebp - 0x268], eax

// block 00472190
00472190  cmp        dword ptr [ebp - 0x224], 1
00472197  jne        0x4721a6

// block 00472199
00472199  cmp        dword ptr [ebp - 0x228], 0
004721a0  je         0x472326

// block 004721a6
004721a6  cmp        dword ptr [ebp - 0x278], 0
004721ad  jne        0x47230a

// block 004721b3
004721b3  mov        eax, dword ptr [ebp - 0x210]
004721b9  test       al, 0x40
004721bb  je         0x4721ef

// block 004721bd
004721bd  test       eax, 0x100
004721c2  je         0x4721cd

// block 004721c4
004721c4  mov        byte ptr [ebp - 0x248], 0x2d
004721cb  jmp        0x4721e5

// block 004721cd
004721cd  test       al, 1
004721cf  je         0x4721da

// block 004721d1
004721d1  mov        byte ptr [ebp - 0x248], 0x2b
004721d8  jmp        0x4721e5

// block 004721da
004721da  test       al, 2
004721dc  je         0x4721ef

// block 004721de
004721de  mov        byte ptr [ebp - 0x248], 0x20

// block 004721e5
004721e5  mov        dword ptr [ebp - 0x258], 1

// block 004721ef
004721ef  mov        ebx, dword ptr [ebp - 0x250]
004721f5  sub        ebx, dword ptr [ebp - 0x268]
004721fb  sub        ebx, dword ptr [ebp - 0x258]
00472201  test       al, 0xc
00472203  jne        0x47221c

// block 00472205
00472205  push       dword ptr [ebp - 0x270]
0047220b  lea        eax, [ebp - 0x240]
00472211  push       ebx
00472212  push       0x20
00472214  call       0x4710cc

// block 00472219
00472219  add        esp, 0xc

// block 0047221c
0047221c  push       dword ptr [ebp - 0x258]
00472222  mov        edi, dword ptr [ebp - 0x270]
00472228  lea        eax, [ebp - 0x240]
0047222e  lea        ecx, [ebp - 0x248]
00472234  call       0x4710f2

// block 00472239
00472239  test       byte ptr [ebp - 0x210], 8
00472240  pop        ecx
00472241  je         0x47225e

// block 00472243
00472243  test       byte ptr [ebp - 0x210], 4
0047224a  jne        0x47225e

// block 0047224c
0047224c  push       edi
0047224d  push       ebx
0047224e  push       0x30
00472250  lea        eax, [ebp - 0x240]
00472256  call       0x4710cc

// block 0047225b
0047225b  add        esp, 0xc

// block 0047225e
0047225e  cmp        dword ptr [ebp - 0x260], 0
00472265  mov        eax, dword ptr [ebp - 0x268]
0047226b  je         0x4722d3

// block 0047226d
0047226d  test       eax, eax
0047226f  jle        0x4722d3

// block 00472271
00472271  mov        esi, dword ptr [ebp - 0x21c]
00472277  mov        dword ptr [ebp - 0x24c], eax

// block 0047227d
0047227d  movzx      eax, word ptr [esi]
00472280  dec        dword ptr [ebp - 0x24c]
00472286  push       eax
00472287  push       6
00472289  lea        eax, [ebp - 0xc]
0047228c  push       eax
0047228d  lea        eax, [ebp - 0x284]
00472293  inc        esi
00472294  push       eax
00472295  inc        esi
00472296  call       0x47c503

// block 0047229b
0047229b  add        esp, 0x10
0047229e  test       eax, eax
004722a0  jne        0x4722ca

// block 004722a2
004722a2  cmp        dword ptr [ebp - 0x284], eax
004722a8  je         0x4722ca

// block 004722aa
004722aa  push       dword ptr [ebp - 0x284]
004722b0  lea        eax, [ebp - 0x240]
004722b6  lea        ecx, [ebp - 0xc]
004722b9  call       0x4710f2

// block 004722be
004722be  cmp        dword ptr [ebp - 0x24c], 0
004722c5  pop        ecx
004722c6  jne        0x47227d

// block 004722c8
004722c8  jmp        0x4722e6

// block 004722ca
004722ca  or         dword ptr [ebp - 0x240], 0xffffffff
004722d1  jmp        0x4722e6

// block 004722d3
004722d3  mov        ecx, dword ptr [ebp - 0x21c]
004722d9  push       eax
004722da  lea        eax, [ebp - 0x240]
004722e0  call       0x4710f2

// block 004722e5
004722e5  pop        ecx

// block 004722e6
004722e6  cmp        dword ptr [ebp - 0x240], 0
004722ed  jl         0x47230a

// block 004722ef
004722ef  test       byte ptr [ebp - 0x210], 4
004722f6  je         0x47230a

// block 004722f8
004722f8  push       edi
004722f9  push       ebx
004722fa  push       0x20
004722fc  lea        eax, [ebp - 0x240]
00472302  call       0x4710cc

// block 00472307
00472307  add        esp, 0xc

// block 0047230a
0047230a  cmp        dword ptr [ebp - 0x27c], 0
00472311  je         0x472326

// block 00472313
00472313  push       dword ptr [ebp - 0x27c]
00472319  call       0x46c941

// block 0047231e
0047231e  and        dword ptr [ebp - 0x27c], 0
00472325  pop        ecx

// block 00472326
00472326  mov        esi, dword ptr [ebp - 0x244]
0047232c  mov        al, byte ptr [esi]
0047232e  mov        byte ptr [ebp - 0x22c], al
00472334  test       al, al
00472336  jne        0x4712d5

// block 0047233c
0047233c  xor        edi, edi

// block 0047233e
0047233e  cmp        dword ptr [ebp - 0x264], edi
00472344  je         0x472353

// block 00472346
00472346  cmp        dword ptr [ebp - 0x264], 7
0047234d  jne        0x4711c0

// block 00472353
00472353  cmp        dword ptr [ebp - 0x224], 1
0047235a  jne        0x4723b5

// block 0047235c
0047235c  cmp        dword ptr [ebp - 0x228], edi
00472362  jne        0x4723b5

// block 00472364
00472364  xor        esi, esi
00472366  cmp        dword ptr [ebp - 0x254], edi
0047236c  jl         0x4723b5

// block 0047236e
0047236e  mov        edx, dword ptr [ebp - 0x214]
00472374  lea        eax, [ebp - 0x8cc]

// block 0047237a
0047237a  mov        ecx, dword ptr [eax - 4]
0047237d  dec        ecx
0047237e  je         0x47239e

// block 00472380
00472380  dec        ecx
00472381  je         0x47239e

// block 00472383
00472383  dec        ecx
00472384  je         0x47239e

// block 00472386
00472386  dec        ecx
00472387  je         0x472397

// block 00472389
00472389  dec        ecx
0047238a  je         0x472397

// block 0047238c
0047238c  dec        ecx
0047238d  je         0x47239e

// block 0047238f
0047238f  dec        ecx
00472390  dec        ecx
00472391  jne        0x4711c0

// block 00472397
00472397  mov        dword ptr [eax], edx
00472399  add        edx, 8
0047239c  jmp        0x4723a3

// block 0047239e
0047239e  mov        dword ptr [eax], edx
004723a0  add        edx, 4

// block 004723a3
004723a3  inc        esi
004723a4  add        eax, 0x10
004723a7  cmp        esi, dword ptr [ebp - 0x254]
004723ad  mov        dword ptr [ebp - 0x214], edx
004723b3  jle        0x47237a

// block 004723b5
004723b5  inc        dword ptr [ebp - 0x228]
004723bb  cmp        dword ptr [ebp - 0x228], 2
004723c2  jge        0x4723cb

// block 004723c4
004723c4  xor        edi, edi
004723c6  jmp        0x471271

// block 004723cb
004723cb  cmp        byte ptr [ebp - 0x230], 0
004723d2  je         0x4723de

// block 004723d4
004723d4  mov        eax, dword ptr [ebp - 0x234]
004723da  and        dword ptr [eax + 0x70], 0xfffffffd

// block 004723de
004723de  mov        eax, dword ptr [ebp - 0x240]

// block 004723e4
004723e4  mov        ecx, dword ptr [ebp - 4]
004723e7  pop        edi
004723e8  pop        esi
004723e9  xor        ecx, ebp
004723eb  pop        ebx
004723ec  call       0x46c932

// block 004723f1
004723f1  leave      
004723f2  ret        
