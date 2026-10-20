
// block 00467170
00467170  sub        esp, 0x120
00467176  push       ebx
00467177  push       ebp
00467178  push       esi
00467179  push       edi
0046717a  mov        edi, eax
0046717c  mov        eax, dword ptr [edi + 4]
0046717f  xor        ebx, ebx
00467181  cmp        eax, ebx
00467183  je         0x468c27

// block 00467189
00467189  fild       dword ptr [eax]
0046718b  fld        dword ptr [edi]
0046718d  fcompp     
0046718f  fnstsw     ax
00467191  test       ah, 1
00467194  jne        0x468c0a

// block 0046719a
0046719a  lea        ebp, [ebx + 1]
0046719d  lea        ecx, [ecx]

// block 004671a0
004671a0  mov        esi, dword ptr [edi + 4]
004671a3  mov        al, byte ptr [esi + 0xa]
004671a6  test       byte ptr [edi + 0x101c], al
004671ac  je         0x468bea

// block 004671b2
004671b2  movsx      eax, word ptr [esi + 4]
004671b6  cmp        eax, 0x59
004671b9  ja         0x468bd9

// block 004671bf
004671bf  movzx      ecx, byte ptr [eax + 0x468d30]
004671c6  jmp        dword ptr [ecx*4 + 0x468c38]

// block 004671cd
004671cd  lea        esi, [edi + 8]
004671d0  mov        eax, esi
004671d2  call       0x469700

// block 004671d7
004671d7  cmp        dword ptr [edi + 0x1008], ebx
004671dd  je         0x468c24

// block 004671e3
004671e3  mov        eax, dword ptr [esi + 0x1000]
004671e9  add        eax, -4
004671ec  cmp        eax, ebx
004671ee  jl         0x4671fc

// block 004671f0
004671f0  mov        dword ptr [esi + 0x1000], eax
004671f6  mov        edx, dword ptr [esi + eax]
004671f9  mov        dword ptr [edi + 4], edx

// block 004671fc
004671fc  mov        eax, dword ptr [esi + 0x1000]
00467202  add        eax, -4
00467205  cmp        eax, ebx
00467207  jl         0x467214

// block 00467209
00467209  mov        dword ptr [esi + 0x1000], eax
0046720f  mov        eax, dword ptr [esi + eax]
00467212  mov        dword ptr [edi], eax

// block 00467214
00467214  mov        eax, dword ptr [esi + 0x1000]
0046721a  add        eax, -4
0046721d  cmp        eax, ebx
0046721f  jl         0x467231

// block 00467221
00467221  mov        dword ptr [esi + 0x1000], eax
00467227  mov        ecx, dword ptr [esi + eax]
0046722a  mov        dword ptr [esp + 0x120], ecx

// block 00467231
00467231  mov        edx, dword ptr [esp + 0x120]
00467238  mov        dword ptr [edi + 0x1008], edx
0046723e  cmp        dword ptr [edi + 4], ebx
00467241  je         0x468c27

// block 00467247
00467247  jmp        0x468bea

// block 0046724c
0046724c  mov        eax, dword ptr [edi + 0x1014]
00467252  push       ebx
00467253  push       -1
00467255  call       0x469110

// block 0046725a
0046725a  jmp        0x468bea

// block 0046725f
0046725f  mov        eax, dword ptr [edi + 0x1014]
00467265  call       0x4691e0

// block 0046726a
0046726a  jmp        0x468bea

// block 0046726f
0046726f  mov        eax, dword ptr [esi + 0x10]
00467272  add        eax, 4
00467275  shr        eax, 2
00467278  test       byte ptr [esi + 8], 2
0046727c  mov        eax, dword ptr [esi + eax*4 + 0x10]
00467280  je         0x46728a

// block 00467282
00467282  push       eax
00467283  mov        ecx, edi
00467285  call       0x468f70

// block 0046728a
0046728a  push       ebp
0046728b  push       eax
0046728c  mov        eax, dword ptr [edi + 0x1014]
00467292  call       0x469110

// block 00467297
00467297  jmp        0x468bea

// block 0046729c
0046729c  push       ebx
0046729d  mov        edx, edi
0046729f  call       0x468e20

// block 004672a4
004672a4  mov        ecx, eax
004672a6  mov        eax, dword ptr [edi + 0x1014]
004672ac  call       0x4691b0

// block 004672b1
004672b1  cmp        eax, ebx
004672b3  je         0x468bea

// block 004672b9
004672b9  mov        ecx, dword ptr [eax]
004672bb  mov        dword ptr [ecx + 4], ebx
004672be  jmp        0x468bea

// block 004672c3
004672c3  push       ebx
004672c4  mov        edx, edi
004672c6  call       0x468e20

// block 004672cb
004672cb  mov        ecx, eax
004672cd  mov        eax, dword ptr [edi + 0x1014]
004672d3  call       0x4691b0

// block 004672d8
004672d8  cmp        eax, ebx
004672da  je         0x468bea

// block 004672e0
004672e0  mov        eax, dword ptr [eax]
004672e2  or         dword ptr [eax + 0x1020], ebp
004672e8  jmp        0x468bea

// block 004672ed
004672ed  push       ebx
004672ee  mov        edx, edi
004672f0  call       0x468e20

// block 004672f5
004672f5  mov        ecx, eax
004672f7  mov        eax, dword ptr [edi + 0x1014]
004672fd  call       0x4691b0

// block 00467302
00467302  cmp        eax, ebx
00467304  je         0x468bea

// block 0046730a
0046730a  mov        eax, dword ptr [eax]
0046730c  and        dword ptr [eax + 0x1020], 0xfffffffe
00467313  jmp        0x468bea

// block 00467318
00467318  push       ebx
00467319  mov        edx, edi
0046731b  call       0x468e20

// block 00467320
00467320  mov        ecx, eax
00467322  mov        eax, dword ptr [edi + 0x1014]
00467328  call       0x4691b0

// block 0046732d
0046732d  cmp        eax, ebx
0046732f  je         0x468bea

// block 00467335
00467335  mov        esi, dword ptr [eax]
00467337  push       ebp
00467338  mov        edx, edi
0046733a  call       0x468e20

// block 0046733f
0046733f  mov        dword ptr [esi + 0x1018], eax
00467345  jmp        0x468bea

// block 0046734a
0046734a  push       ebx
0046734b  mov        eax, edi
0046734d  call       0x466f00

// block 00467352
00467352  test       eax, eax
00467354  jne        0x468c27

// block 0046735a
0046735a  jmp        0x468bf6

// block 0046735f
0046735f  mov        eax, dword ptr [edi + 0x1008]
00467365  add        eax, -4
00467368  cmp        eax, ebx
0046736a  jl         0x4673a0

// block 0046736c
0046736c  mov        dword ptr [edi + 0x1008], eax
00467372  mov        edx, dword ptr [edi + eax + 8]
00467376  add        eax, -4
00467379  mov        dword ptr [edi + 0x1008], eax
0046737f  cmp        byte ptr [eax + edi + 8], 0x66
00467384  mov        dword ptr [esp + 0x11c], edx
0046738b  jne        0x4673a0

// block 0046738d
0046738d  fld        dword ptr [esp + 0x11c]
00467394  call       0x4931e0

// block 00467399
00467399  mov        dword ptr [esp + 0x11c], eax

// block 004673a0
004673a0  cmp        dword ptr [esp + 0x11c], ebx
004673a7  jne        0x4673fc

// block 004673a9
004673a9  jmp        0x468bea

// block 004673ae
004673ae  mov        eax, dword ptr [edi + 0x1008]
004673b4  add        eax, -4
004673b7  cmp        eax, ebx
004673b9  jl         0x4673ef

// block 004673bb
004673bb  mov        dword ptr [edi + 0x1008], eax
004673c1  mov        ecx, dword ptr [edi + eax + 8]
004673c5  add        eax, -4
004673c8  mov        dword ptr [edi + 0x1008], eax
004673ce  cmp        byte ptr [edi + eax + 8], 0x66
004673d3  mov        dword ptr [esp + 0x104], ecx
004673da  jne        0x4673ef

// block 004673dc
004673dc  fld        dword ptr [esp + 0x104]
004673e3  call       0x4931e0

// block 004673e8
004673e8  mov        dword ptr [esp + 0x104], eax

// block 004673ef
004673ef  cmp        dword ptr [esp + 0x104], ebx
004673f6  jne        0x468bea

// block 004673fc
004673fc  mov        eax, dword ptr [edi + 4]
004673ff  fild       dword ptr [eax + 0x14]
00467402  fstp       dword ptr [edi]
00467404  mov        edx, dword ptr [eax + 0x10]
00467407  add        edx, eax
00467409  mov        dword ptr [edi + 4], edx
0046740c  jmp        0x468bf6

// block 00467411
00467411  fld        dword ptr [edi]
00467413  push       ebx
00467414  mov        edx, edi
00467416  fstp       qword ptr [esp + 0x24]
0046741a  call       0x468e20

// block 0046741f
0046741f  mov        dword ptr [esp + 0x10], eax
00467423  fild       dword ptr [esp + 0x10]
00467427  fsubr      qword ptr [esp + 0x20]
0046742b  fstp       dword ptr [edi]
0046742d  jmp        0x468bea

// block 00467432
00467432  push       ebx
00467433  mov        edx, edi
00467435  call       0x468e20

// block 0046743a
0046743a  lea        edx, [edi + 8]
0046743d  mov        ecx, eax
0046743f  mov        eax, edx
00467441  call       0x4696c0

// block 00467446
00467446  jmp        0x468bea

// block 0046744b
0046744b  lea        eax, [edi + 8]
0046744e  call       0x469700

// block 00467453
00467453  jmp        0x468bea

// block 00467458
00467458  push       ebx
00467459  mov        edx, edi
0046745b  call       0x468e20

// block 00467460
00467460  mov        dword ptr [esp + 0x10], eax
00467464  lea        eax, [esp + 0x10]
00467468  push       eax
00467469  lea        eax, [edi + 8]
0046746c  mov        dl, 0x69
0046746e  call       0x469610

// block 00467473
00467473  jmp        0x468bea

// block 00467478
00467478  mov        eax, edi
0046747a  call       0x469080

// block 0046747f
0046747f  mov        esi, eax
00467481  mov        eax, dword ptr [edi + 0x1008]
00467487  add        eax, -4
0046748a  cmp        eax, ebx
0046748c  jl         0x468bea

// block 00467492
00467492  mov        dword ptr [edi + 0x1008], eax
00467498  mov        ecx, dword ptr [edi + eax + 8]
0046749c  mov        dword ptr [esi], ecx
0046749e  add        dword ptr [edi + 0x1008], -4
004674a5  mov        eax, dword ptr [edi + 0x1008]
004674ab  cmp        byte ptr [edi + eax + 8], 0x66
004674b0  jne        0x468bea

// block 004674b6
004674b6  fld        dword ptr [esi]
004674b8  call       0x4931e0

// block 004674bd
004674bd  mov        dword ptr [esi], eax
004674bf  jmp        0x468bea

// block 004674c4
004674c4  xor        ecx, ecx
004674c6  mov        eax, edi
004674c8  call       0x468ec0

// block 004674cd
004674cd  fstp       dword ptr [esp + 0x10]
004674d1  lea        edx, [esp + 0x10]
004674d5  push       edx
004674d6  lea        eax, [edi + 8]
004674d9  mov        dl, 0x66
004674db  call       0x469610

// block 004674e0
004674e0  jmp        0x468bea

// block 004674e5
004674e5  xor        ecx, ecx
004674e7  mov        edx, edi
004674e9  call       0x4690b0

// block 004674ee
004674ee  mov        ecx, dword ptr [edi + 0x1008]
004674f4  add        ecx, -4
004674f7  cmp        ecx, ebx
004674f9  jl         0x468bea

// block 004674ff
004674ff  mov        dword ptr [edi + 0x1008], ecx
00467505  mov        ecx, dword ptr [edi + ecx + 8]
00467509  mov        dword ptr [eax], ecx
0046750b  add        dword ptr [edi + 0x1008], -4
00467512  mov        ecx, dword ptr [edi + 0x1008]
00467518  mov        cl, byte ptr [edi + ecx + 8]
0046751c  cmp        cl, 0x66
0046751f  je         0x468bea

// block 00467525
00467525  cmp        cl, 0x69
00467528  jne        0x468bea

// block 0046752e
0046752e  fild       dword ptr [eax]
00467530  fstp       dword ptr [eax]
00467532  jmp        0x468bea

// block 00467537
00467537  mov        eax, dword ptr [edi + 0x1008]
0046753d  lea        esi, [edi + 8]
00467540  add        eax, -4
00467543  cmp        eax, ebx
00467545  jl         0x467579

// block 00467547
00467547  mov        dword ptr [esi + 0x1000], eax
0046754d  mov        edx, dword ptr [esi + eax]
00467550  add        eax, -4
00467553  mov        dword ptr [esi + 0x1000], eax
00467559  cmp        byte ptr [esi + eax], 0x66
0046755d  mov        dword ptr [esp + 0x10c], edx
00467564  jne        0x467579

// block 00467566
00467566  fld        dword ptr [esp + 0x10c]
0046756d  call       0x4931e0

// block 00467572
00467572  mov        dword ptr [esp + 0x10c], eax

// block 00467579
00467579  mov        eax, dword ptr [esi + 0x1000]
0046757f  add        eax, -4
00467582  cmp        eax, ebx
00467584  jl         0x4675af

// block 00467586
00467586  mov        dword ptr [esi + 0x1000], eax
0046758c  mov        ecx, dword ptr [esi + eax]
0046758f  add        eax, -4
00467592  mov        dword ptr [esi + 0x1000], eax
00467598  cmp        byte ptr [esi + eax], 0x66
0046759c  mov        dword ptr [esp + 0x40], ecx
004675a0  jne        0x4675af

// block 004675a2
004675a2  fld        dword ptr [esp + 0x40]
004675a6  call       0x4931e0

// block 004675ab
004675ab  mov        dword ptr [esp + 0x40], eax

// block 004675af
004675af  mov        edx, dword ptr [esp + 0x10c]
004675b6  add        dword ptr [esp + 0x40], edx
004675ba  lea        eax, [esp + 0x40]
004675be  push       eax
004675bf  mov        dl, 0x69
004675c1  mov        eax, esi
004675c3  call       0x469610

// block 004675c8
004675c8  jmp        0x468bea

// block 004675cd
004675cd  mov        eax, dword ptr [edi + 0x1008]
004675d3  lea        esi, [edi + 8]
004675d6  add        eax, -4
004675d9  cmp        eax, ebx
004675db  jl         0x46760f

// block 004675dd
004675dd  mov        dword ptr [esi + 0x1000], eax
004675e3  mov        ecx, dword ptr [esi + eax]
004675e6  add        eax, -4
004675e9  mov        dword ptr [esi + 0x1000], eax
004675ef  cmp        byte ptr [esi + eax], 0x66
004675f3  mov        dword ptr [esp + 0x114], ecx
004675fa  jne        0x46760f

// block 004675fc
004675fc  fld        dword ptr [esp + 0x114]
00467603  call       0x4931e0

// block 00467608
00467608  mov        dword ptr [esp + 0x114], eax

// block 0046760f
0046760f  mov        eax, dword ptr [esi + 0x1000]
00467615  add        eax, -4
00467618  cmp        eax, ebx
0046761a  jl         0x467645

// block 0046761c
0046761c  mov        dword ptr [esi + 0x1000], eax
00467622  mov        edx, dword ptr [esi + eax]
00467625  add        eax, -4
00467628  mov        dword ptr [esi + 0x1000], eax
0046762e  cmp        byte ptr [esi + eax], 0x66
00467632  mov        dword ptr [esp + 0x3c], edx
00467636  jne        0x467645

// block 00467638
00467638  fld        dword ptr [esp + 0x3c]
0046763c  call       0x4931e0

// block 00467641
00467641  mov        dword ptr [esp + 0x3c], eax

// block 00467645
00467645  mov        eax, dword ptr [esp + 0x114]
0046764c  sub        dword ptr [esp + 0x3c], eax
00467650  lea        ecx, [esp + 0x3c]
00467654  push       ecx
00467655  mov        dl, 0x69
00467657  mov        eax, esi
00467659  call       0x469610

// block 0046765e
0046765e  jmp        0x468bea

// block 00467663
00467663  mov        eax, dword ptr [edi + 0x1008]
00467669  lea        esi, [edi + 8]
0046766c  add        eax, -4
0046766f  cmp        eax, ebx
00467671  jl         0x46769c

// block 00467673
00467673  mov        dword ptr [esi + 0x1000], eax
00467679  mov        edx, dword ptr [esi + eax]
0046767c  add        eax, -4
0046767f  mov        dword ptr [esi + 0x1000], eax
00467685  cmp        byte ptr [esi + eax], 0x66
00467689  mov        dword ptr [esp + 0x60], edx
0046768d  jne        0x46769c

// block 0046768f
0046768f  fld        dword ptr [esp + 0x60]
00467693  call       0x4931e0

// block 00467698
00467698  mov        dword ptr [esp + 0x60], eax

// block 0046769c
0046769c  mov        ecx, dword ptr [esi + 0x1000]
004676a2  add        ecx, -4
004676a5  cmp        ecx, ebx
004676a7  jl         0x4676d0

// block 004676a9
004676a9  mov        dword ptr [esi + 0x1000], ecx
004676af  mov        eax, dword ptr [esi + ecx]
004676b2  add        ecx, -4
004676b5  mov        dword ptr [esi + 0x1000], ecx
004676bb  cmp        byte ptr [esi + ecx], 0x66
004676bf  mov        dword ptr [esp + 0x4c], eax
004676c3  jne        0x4676d4

// block 004676c5
004676c5  fld        dword ptr [esp + 0x4c]
004676c9  call       0x4931e0

// block 004676ce
004676ce  jmp        0x4676d4

// block 004676d0
004676d0  mov        eax, dword ptr [esp + 0x4c]

// block 004676d4
004676d4  imul       eax, dword ptr [esp + 0x60]
004676d9  mov        dword ptr [esp + 0x4c], eax
004676dd  lea        eax, [esp + 0x4c]
004676e1  push       eax
004676e2  mov        dl, 0x69
004676e4  mov        eax, esi
004676e6  call       0x469610

// block 004676eb
004676eb  jmp        0x468bea

// block 004676f0
004676f0  mov        eax, dword ptr [edi + 0x1008]
004676f6  lea        esi, [edi + 8]
004676f9  add        eax, -4
004676fc  cmp        eax, ebx
004676fe  jl         0x467732

// block 00467700
00467700  mov        dword ptr [esi + 0x1000], eax
00467706  mov        ecx, dword ptr [esi + eax]
00467709  add        eax, -4
0046770c  mov        dword ptr [esi + 0x1000], eax
00467712  cmp        byte ptr [esi + eax], 0x66
00467716  mov        dword ptr [esp + 0x118], ecx
0046771d  jne        0x467732

// block 0046771f
0046771f  fld        dword ptr [esp + 0x118]
00467726  call       0x4931e0

// block 0046772b
0046772b  mov        dword ptr [esp + 0x118], eax

// block 00467732
00467732  mov        ecx, dword ptr [esi + 0x1000]
00467738  add        ecx, -4
0046773b  cmp        ecx, ebx
0046773d  jl         0x467766

// block 0046773f
0046773f  mov        dword ptr [esi + 0x1000], ecx
00467745  mov        eax, dword ptr [esi + ecx]
00467748  add        ecx, -4
0046774b  mov        dword ptr [esi + 0x1000], ecx
00467751  cmp        byte ptr [esi + ecx], 0x66
00467755  mov        dword ptr [esp + 0x50], eax
00467759  jne        0x46776a

// block 0046775b
0046775b  fld        dword ptr [esp + 0x50]
0046775f  call       0x4931e0

// block 00467764
00467764  jmp        0x46776a

// block 00467766
00467766  mov        eax, dword ptr [esp + 0x50]

// block 0046776a
0046776a  cdq        
0046776b  idiv       dword ptr [esp + 0x118]
00467772  lea        edx, [esp + 0x50]
00467776  push       edx
00467777  mov        dl, 0x69
00467779  mov        dword ptr [esp + 0x54], eax
0046777d  mov        eax, esi
0046777f  call       0x469610

// block 00467784
00467784  jmp        0x468bea

// block 00467789
00467789  mov        eax, dword ptr [edi + 0x1008]
0046778f  lea        esi, [edi + 8]
00467792  add        eax, -4
00467795  cmp        eax, ebx
00467797  jl         0x4677c2

// block 00467799
00467799  mov        dword ptr [esi + 0x1000], eax
0046779f  mov        ecx, dword ptr [esi + eax]
004677a2  add        eax, -4
004677a5  mov        dword ptr [esi + 0x1000], eax
004677ab  cmp        byte ptr [esi + eax], 0x66
004677af  mov        dword ptr [esp + 0x68], ecx
004677b3  jne        0x4677c2

// block 004677b5
004677b5  fld        dword ptr [esp + 0x68]
004677b9  call       0x4931e0

// block 004677be
004677be  mov        dword ptr [esp + 0x68], eax

// block 004677c2
004677c2  mov        ecx, dword ptr [esi + 0x1000]
004677c8  add        ecx, -4
004677cb  cmp        ecx, ebx
004677cd  jl         0x4677f6

// block 004677cf
004677cf  mov        dword ptr [esi + 0x1000], ecx
004677d5  mov        eax, dword ptr [esi + ecx]
004677d8  add        ecx, -4
004677db  mov        dword ptr [esi + 0x1000], ecx
004677e1  cmp        byte ptr [esi + ecx], 0x66
004677e5  mov        dword ptr [esp + 0x48], eax
004677e9  jne        0x4677fa

// block 004677eb
004677eb  fld        dword ptr [esp + 0x48]
004677ef  call       0x4931e0

// block 004677f4
004677f4  jmp        0x4677fa

// block 004677f6
004677f6  mov        eax, dword ptr [esp + 0x48]

// block 004677fa
004677fa  cdq        
004677fb  idiv       dword ptr [esp + 0x68]
004677ff  mov        eax, esi
00467801  mov        dword ptr [esp + 0x48], edx
00467805  lea        edx, [esp + 0x48]
00467809  push       edx
0046780a  mov        dl, 0x69
0046780c  call       0x469610

// block 00467811
00467811  jmp        0x468bea

// block 00467816
00467816  mov        ecx, dword ptr [edi + 0x1008]
0046781c  lea        eax, [edi + 8]
0046781f  add        ecx, -4
00467822  cmp        ecx, ebx
00467824  jl         0x46785a

// block 00467826
00467826  mov        dword ptr [eax + 0x1000], ecx
0046782c  mov        edx, dword ptr [eax + ecx]
0046782f  add        ecx, -4
00467832  mov        dword ptr [eax + 0x1000], ecx
00467838  mov        cl, byte ptr [eax + ecx]
0046783b  mov        dword ptr [esp + 0xc8], edx
00467842  cmp        cl, 0x66
00467845  je         0x46785a

// block 00467847
00467847  cmp        cl, 0x69
0046784a  jne        0x46785a

// block 0046784c
0046784c  fild       dword ptr [esp + 0xc8]
00467853  fstp       dword ptr [esp + 0xc8]

// block 0046785a
0046785a  mov        ecx, dword ptr [eax + 0x1000]
00467860  add        ecx, -4
00467863  cmp        ecx, ebx
00467865  jl         0x467892

// block 00467867
00467867  mov        dword ptr [eax + 0x1000], ecx
0046786d  mov        edx, dword ptr [eax + ecx]
00467870  add        ecx, -4
00467873  mov        dword ptr [eax + 0x1000], ecx
00467879  mov        cl, byte ptr [eax + ecx]
0046787c  mov        dword ptr [esp + 0x44], edx
00467880  cmp        cl, 0x66
00467883  je         0x467892

// block 00467885
00467885  cmp        cl, 0x69
00467888  jne        0x467892

// block 0046788a
0046788a  fild       dword ptr [esp + 0x44]
0046788e  fstp       dword ptr [esp + 0x44]

// block 00467892
00467892  fld        dword ptr [esp + 0x44]
00467896  lea        ecx, [esp + 0x44]
0046789a  fadd       dword ptr [esp + 0xc8]
004678a1  push       ecx
004678a2  mov        dl, 0x66
004678a4  fstp       dword ptr [esp + 0x48]
004678a8  call       0x469610

// block 004678ad
004678ad  jmp        0x468bea

// block 004678b2
004678b2  mov        ecx, dword ptr [edi + 0x1008]
004678b8  lea        eax, [edi + 8]
004678bb  add        ecx, -4
004678be  cmp        ecx, ebx
004678c0  jl         0x4678ed

// block 004678c2
004678c2  mov        dword ptr [eax + 0x1000], ecx
004678c8  mov        edx, dword ptr [eax + ecx]
004678cb  add        ecx, -4
004678ce  mov        dword ptr [eax + 0x1000], ecx
004678d4  mov        cl, byte ptr [eax + ecx]
004678d7  mov        dword ptr [esp + 0x70], edx
004678db  cmp        cl, 0x66
004678de  je         0x4678ed

// block 004678e0
004678e0  cmp        cl, 0x69
004678e3  jne        0x4678ed

// block 004678e5
004678e5  fild       dword ptr [esp + 0x70]
004678e9  fstp       dword ptr [esp + 0x70]

// block 004678ed
004678ed  mov        ecx, dword ptr [eax + 0x1000]
004678f3  add        ecx, -4
004678f6  cmp        ecx, ebx
004678f8  jl         0x467925

// block 004678fa
004678fa  mov        dword ptr [eax + 0x1000], ecx
00467900  mov        edx, dword ptr [eax + ecx]
00467903  add        ecx, -4
00467906  mov        dword ptr [eax + 0x1000], ecx
0046790c  mov        cl, byte ptr [eax + ecx]
0046790f  mov        dword ptr [esp + 0x38], edx
00467913  cmp        cl, 0x66
00467916  je         0x467925

// block 00467918
00467918  cmp        cl, 0x69
0046791b  jne        0x467925

// block 0046791d
0046791d  fild       dword ptr [esp + 0x38]
00467921  fstp       dword ptr [esp + 0x38]

// block 00467925
00467925  fld        dword ptr [esp + 0x38]
00467929  lea        ecx, [esp + 0x38]
0046792d  fsub       dword ptr [esp + 0x70]
00467931  push       ecx
00467932  mov        dl, 0x66
00467934  fstp       dword ptr [esp + 0x3c]
00467938  call       0x469610

// block 0046793d
0046793d  jmp        0x468bea

// block 00467942
00467942  mov        ecx, dword ptr [edi + 0x1008]
00467948  lea        eax, [edi + 8]
0046794b  add        ecx, -4
0046794e  cmp        ecx, ebx
00467950  jl         0x467986

// block 00467952
00467952  mov        dword ptr [eax + 0x1000], ecx
00467958  mov        edx, dword ptr [eax + ecx]
0046795b  add        ecx, -4
0046795e  mov        dword ptr [eax + 0x1000], ecx
00467964  mov        cl, byte ptr [eax + ecx]
00467967  mov        dword ptr [esp + 0xf8], edx
0046796e  cmp        cl, 0x66
00467971  je         0x467986

// block 00467973
00467973  cmp        cl, 0x69
00467976  jne        0x467986

// block 00467978
00467978  fild       dword ptr [esp + 0xf8]
0046797f  fstp       dword ptr [esp + 0xf8]

// block 00467986
00467986  mov        ecx, dword ptr [eax + 0x1000]
0046798c  add        ecx, -4
0046798f  cmp        ecx, ebx
00467991  jl         0x4679be

// block 00467993
00467993  mov        dword ptr [eax + 0x1000], ecx
00467999  mov        edx, dword ptr [eax + ecx]
0046799c  add        ecx, -4
0046799f  mov        dword ptr [eax + 0x1000], ecx
004679a5  mov        cl, byte ptr [eax + ecx]
004679a8  mov        dword ptr [esp + 0x34], edx
004679ac  cmp        cl, 0x66
004679af  je         0x4679be

// block 004679b1
004679b1  cmp        cl, 0x69
004679b4  jne        0x4679be

// block 004679b6
004679b6  fild       dword ptr [esp + 0x34]
004679ba  fstp       dword ptr [esp + 0x34]

// block 004679be
004679be  fld        dword ptr [esp + 0x34]
004679c2  lea        ecx, [esp + 0x34]
004679c6  fmul       dword ptr [esp + 0xf8]
004679cd  push       ecx
004679ce  mov        dl, 0x66
004679d0  fstp       dword ptr [esp + 0x38]
004679d4  call       0x469610

// block 004679d9
004679d9  jmp        0x468bea

// block 004679de
004679de  mov        ecx, dword ptr [edi + 0x1008]
004679e4  lea        eax, [edi + 8]
004679e7  add        ecx, -4
004679ea  cmp        ecx, ebx
004679ec  jl         0x467a19

// block 004679ee
004679ee  mov        dword ptr [eax + 0x1000], ecx
004679f4  mov        edx, dword ptr [eax + ecx]
004679f7  add        ecx, -4
004679fa  mov        dword ptr [eax + 0x1000], ecx
00467a00  mov        cl, byte ptr [eax + ecx]
00467a03  mov        dword ptr [esp + 0x78], edx
00467a07  cmp        cl, 0x66
00467a0a  je         0x467a19

// block 00467a0c
00467a0c  cmp        cl, 0x69
00467a0f  jne        0x467a19

// block 00467a11
00467a11  fild       dword ptr [esp + 0x78]
00467a15  fstp       dword ptr [esp + 0x78]

// block 00467a19
00467a19  mov        ecx, dword ptr [eax + 0x1000]
00467a1f  add        ecx, -4
00467a22  cmp        ecx, ebx
00467a24  jl         0x467a51

// block 00467a26
00467a26  mov        dword ptr [eax + 0x1000], ecx
00467a2c  mov        edx, dword ptr [eax + ecx]
00467a2f  add        ecx, -4
00467a32  mov        dword ptr [eax + 0x1000], ecx
00467a38  mov        cl, byte ptr [eax + ecx]
00467a3b  mov        dword ptr [esp + 0x30], edx
00467a3f  cmp        cl, 0x66
00467a42  je         0x467a51

// block 00467a44
00467a44  cmp        cl, 0x69
00467a47  jne        0x467a51

// block 00467a49
00467a49  fild       dword ptr [esp + 0x30]
00467a4d  fstp       dword ptr [esp + 0x30]

// block 00467a51
00467a51  fld        dword ptr [esp + 0x30]
00467a55  lea        ecx, [esp + 0x30]
00467a59  fdiv       dword ptr [esp + 0x78]
00467a5d  push       ecx
00467a5e  mov        dl, 0x66
00467a60  fstp       dword ptr [esp + 0x34]
00467a64  call       0x469610

// block 00467a69
00467a69  jmp        0x468bea

// block 00467a6e
00467a6e  mov        eax, dword ptr [edi + 0x1008]
00467a74  lea        esi, [edi + 8]
00467a77  add        eax, -4
00467a7a  cmp        eax, ebx
00467a7c  jl         0x467ab0

// block 00467a7e
00467a7e  mov        dword ptr [esi + 0x1000], eax
00467a84  mov        edx, dword ptr [esi + eax]
00467a87  add        eax, -4
00467a8a  mov        dword ptr [esi + 0x1000], eax
00467a90  cmp        byte ptr [esi + eax], 0x66
00467a94  mov        dword ptr [esp + 0xd0], edx
00467a9b  jne        0x467ab0

// block 00467a9d
00467a9d  fld        dword ptr [esp + 0xd0]
00467aa4  call       0x4931e0

// block 00467aa9
00467aa9  mov        dword ptr [esp + 0xd0], eax

// block 00467ab0
00467ab0  mov        eax, dword ptr [esi + 0x1000]
00467ab6  add        eax, -4
00467ab9  cmp        eax, ebx
00467abb  jl         0x467aef

// block 00467abd
00467abd  mov        dword ptr [esi + 0x1000], eax
00467ac3  mov        ecx, dword ptr [esi + eax]
00467ac6  add        eax, -4
00467ac9  mov        dword ptr [esi + 0x1000], eax
00467acf  cmp        byte ptr [esi + eax], 0x66
00467ad3  mov        dword ptr [esp + 0x80], ecx
00467ada  jne        0x467aef

// block 00467adc
00467adc  fld        dword ptr [esp + 0x80]
00467ae3  call       0x4931e0

// block 00467ae8
00467ae8  mov        dword ptr [esp + 0x80], eax

// block 00467aef
00467aef  mov        eax, dword ptr [esp + 0xd0]
00467af6  xor        edx, edx
00467af8  cmp        dword ptr [esp + 0x80], eax
00467aff  sete       dl

// block 00467b02
00467b02  lea        ecx, [esp + 0x10]
00467b06  mov        dword ptr [esp + 0x10], edx
00467b0a  push       ecx
00467b0b  mov        dl, 0x69
00467b0d  mov        eax, esi
00467b0f  call       0x469610

// block 00467b14
00467b14  jmp        0x468bea

// block 00467b19
00467b19  mov        eax, dword ptr [edi + 0x1008]
00467b1f  lea        esi, [edi + 8]
00467b22  add        eax, -4
00467b25  cmp        eax, ebx
00467b27  jl         0x467b5b

// block 00467b29
00467b29  mov        dword ptr [esi + 0x1000], eax
00467b2f  mov        edx, dword ptr [esi + eax]
00467b32  add        eax, -4
00467b35  mov        dword ptr [esi + 0x1000], eax
00467b3b  cmp        byte ptr [esi + eax], 0x66
00467b3f  mov        dword ptr [esp + 0x110], edx
00467b46  jne        0x467b5b

// block 00467b48
00467b48  fld        dword ptr [esp + 0x110]
00467b4f  call       0x4931e0

// block 00467b54
00467b54  mov        dword ptr [esp + 0x110], eax

// block 00467b5b
00467b5b  mov        eax, dword ptr [esi + 0x1000]
00467b61  add        eax, -4
00467b64  cmp        eax, ebx
00467b66  jl         0x467b9a

// block 00467b68
00467b68  mov        dword ptr [esi + 0x1000], eax
00467b6e  mov        ecx, dword ptr [esi + eax]
00467b71  add        eax, -4
00467b74  mov        dword ptr [esi + 0x1000], eax
00467b7a  cmp        byte ptr [esi + eax], 0x66
00467b7e  mov        dword ptr [esp + 0x88], ecx
00467b85  jne        0x467b9a

// block 00467b87
00467b87  fld        dword ptr [esp + 0x88]
00467b8e  call       0x4931e0

// block 00467b93
00467b93  mov        dword ptr [esp + 0x88], eax

// block 00467b9a
00467b9a  mov        eax, dword ptr [esp + 0x110]
00467ba1  xor        edx, edx
00467ba3  cmp        dword ptr [esp + 0x88], eax
00467baa  setne      dl
00467bad  jmp        0x467b02

// block 00467bb2
00467bb2  mov        eax, dword ptr [edi + 0x1008]
00467bb8  lea        esi, [edi + 8]
00467bbb  add        eax, -4
00467bbe  cmp        eax, ebx
00467bc0  jl         0x467bf4

// block 00467bc2
00467bc2  mov        dword ptr [esi + 0x1000], eax
00467bc8  mov        edx, dword ptr [esi + eax]
00467bcb  add        eax, -4
00467bce  mov        dword ptr [esi + 0x1000], eax
00467bd4  cmp        byte ptr [esi + eax], 0x66
00467bd8  mov        dword ptr [esp + 0xd8], edx
00467bdf  jne        0x467bf4

// block 00467be1
00467be1  fld        dword ptr [esp + 0xd8]
00467be8  call       0x4931e0

// block 00467bed
00467bed  mov        dword ptr [esp + 0xd8], eax

// block 00467bf4
00467bf4  mov        eax, dword ptr [esi + 0x1000]
00467bfa  add        eax, -4
00467bfd  cmp        eax, ebx
00467bff  jl         0x467c33

// block 00467c01
00467c01  mov        dword ptr [esi + 0x1000], eax
00467c07  mov        ecx, dword ptr [esi + eax]
00467c0a  add        eax, -4
00467c0d  mov        dword ptr [esi + 0x1000], eax
00467c13  cmp        byte ptr [esi + eax], 0x66
00467c17  mov        dword ptr [esp + 0x90], ecx
00467c1e  jne        0x467c33

// block 00467c20
00467c20  fld        dword ptr [esp + 0x90]
00467c27  call       0x4931e0

// block 00467c2c
00467c2c  mov        dword ptr [esp + 0x90], eax

// block 00467c33
00467c33  mov        eax, dword ptr [esp + 0xd8]
00467c3a  xor        edx, edx
00467c3c  cmp        dword ptr [esp + 0x90], eax
00467c43  setl       dl
00467c46  jmp        0x467b02

// block 00467c4b
00467c4b  mov        eax, dword ptr [edi + 0x1008]
00467c51  lea        esi, [edi + 8]
00467c54  add        eax, -4
00467c57  cmp        eax, ebx
00467c59  jl         0x467c8d

// block 00467c5b
00467c5b  mov        dword ptr [esi + 0x1000], eax
00467c61  mov        edx, dword ptr [esi + eax]
00467c64  add        eax, -4
00467c67  mov        dword ptr [esi + 0x1000], eax
00467c6d  cmp        byte ptr [esi + eax], 0x66
00467c71  mov        dword ptr [esp + 0x100], edx
00467c78  jne        0x467c8d

// block 00467c7a
00467c7a  fld        dword ptr [esp + 0x100]
00467c81  call       0x4931e0

// block 00467c86
00467c86  mov        dword ptr [esp + 0x100], eax

// block 00467c8d
00467c8d  mov        eax, dword ptr [esi + 0x1000]
00467c93  add        eax, -4
00467c96  cmp        eax, ebx
00467c98  jl         0x467ccc

// block 00467c9a
00467c9a  mov        dword ptr [esi + 0x1000], eax
00467ca0  mov        ecx, dword ptr [esi + eax]
00467ca3  add        eax, -4
00467ca6  mov        dword ptr [esi + 0x1000], eax
00467cac  cmp        byte ptr [esi + eax], 0x66
00467cb0  mov        dword ptr [esp + 0x98], ecx
00467cb7  jne        0x467ccc

// block 00467cb9
00467cb9  fld        dword ptr [esp + 0x98]
00467cc0  call       0x4931e0

// block 00467cc5
00467cc5  mov        dword ptr [esp + 0x98], eax

// block 00467ccc
00467ccc  mov        eax, dword ptr [esp + 0x100]
00467cd3  xor        edx, edx
00467cd5  cmp        dword ptr [esp + 0x98], eax
00467cdc  setle      dl
00467cdf  jmp        0x467b02

// block 00467ce4
00467ce4  mov        eax, dword ptr [edi + 0x1008]
00467cea  lea        esi, [edi + 8]
00467ced  add        eax, -4
00467cf0  cmp        eax, ebx
00467cf2  jl         0x467d26

// block 00467cf4
00467cf4  mov        dword ptr [esi + 0x1000], eax
00467cfa  mov        edx, dword ptr [esi + eax]
00467cfd  add        eax, -4
00467d00  mov        dword ptr [esi + 0x1000], eax
00467d06  cmp        byte ptr [esi + eax], 0x66
00467d0a  mov        dword ptr [esp + 0xe0], edx
00467d11  jne        0x467d26

// block 00467d13
00467d13  fld        dword ptr [esp + 0xe0]
00467d1a  call       0x4931e0

// block 00467d1f
00467d1f  mov        dword ptr [esp + 0xe0], eax

// block 00467d26
00467d26  mov        eax, dword ptr [esi + 0x1000]
00467d2c  add        eax, -4
00467d2f  cmp        eax, ebx
00467d31  jl         0x467d65

// block 00467d33
00467d33  mov        dword ptr [esi + 0x1000], eax
00467d39  mov        ecx, dword ptr [esi + eax]
00467d3c  add        eax, -4
00467d3f  mov        dword ptr [esi + 0x1000], eax
00467d45  cmp        byte ptr [esi + eax], 0x66
00467d49  mov        dword ptr [esp + 0xa0], ecx
00467d50  jne        0x467d65

// block 00467d52
00467d52  fld        dword ptr [esp + 0xa0]
00467d59  call       0x4931e0

// block 00467d5e
00467d5e  mov        dword ptr [esp + 0xa0], eax

// block 00467d65
00467d65  mov        eax, dword ptr [esp + 0xe0]
00467d6c  xor        edx, edx
00467d6e  cmp        dword ptr [esp + 0xa0], eax
00467d75  setg       dl
00467d78  jmp        0x467b02

// block 00467d7d
00467d7d  mov        eax, dword ptr [edi + 0x1008]
00467d83  lea        esi, [edi + 8]
00467d86  add        eax, -4
00467d89  cmp        eax, ebx
00467d8b  jl         0x467db6

// block 00467d8d
00467d8d  mov        dword ptr [esi + 0x1000], eax
00467d93  mov        edx, dword ptr [esi + eax]
00467d96  add        eax, -4
00467d99  mov        dword ptr [esi + 0x1000], eax
00467d9f  cmp        byte ptr [esi + eax], 0x66
00467da3  mov        dword ptr [esp + 0x58], edx
00467da7  jne        0x467db6

// block 00467da9
00467da9  fld        dword ptr [esp + 0x58]
00467dad  call       0x4931e0

// block 00467db2
00467db2  mov        dword ptr [esp + 0x58], eax

// block 00467db6
00467db6  mov        eax, dword ptr [esi + 0x1000]
00467dbc  add        eax, -4
00467dbf  cmp        eax, ebx
00467dc1  jl         0x467df5

// block 00467dc3
00467dc3  mov        dword ptr [esi + 0x1000], eax
00467dc9  mov        ecx, dword ptr [esi + eax]
00467dcc  add        eax, -4
00467dcf  mov        dword ptr [esi + 0x1000], eax
00467dd5  cmp        byte ptr [esi + eax], 0x66
00467dd9  mov        dword ptr [esp + 0xa8], ecx
00467de0  jne        0x467df5

// block 00467de2
00467de2  fld        dword ptr [esp + 0xa8]
00467de9  call       0x4931e0

// block 00467dee
00467dee  mov        dword ptr [esp + 0xa8], eax

// block 00467df5
00467df5  mov        eax, dword ptr [esp + 0x58]
00467df9  xor        edx, edx
00467dfb  cmp        dword ptr [esp + 0xa8], eax
00467e02  setge      dl
00467e05  jmp        0x467b02

// block 00467e0a
00467e0a  mov        eax, dword ptr [edi + 0x1008]
00467e10  lea        esi, [edi + 8]
00467e13  add        eax, -4
00467e16  cmp        eax, ebx
00467e18  jl         0x467e4c

// block 00467e1a
00467e1a  mov        dword ptr [esi + 0x1000], eax
00467e20  mov        edx, dword ptr [esi + eax]
00467e23  add        eax, -4
00467e26  mov        dword ptr [esi + 0x1000], eax
00467e2c  cmp        byte ptr [esi + eax], 0x66
00467e30  mov        dword ptr [esp + 0xe8], edx
00467e37  jne        0x467e4c

// block 00467e39
00467e39  fld        dword ptr [esp + 0xe8]
00467e40  call       0x4931e0

// block 00467e45
00467e45  mov        dword ptr [esp + 0xe8], eax

// block 00467e4c
00467e4c  xor        eax, eax
00467e4e  cmp        dword ptr [esp + 0xe8], ebx
00467e55  lea        ecx, [esp + 0x10]
00467e59  sete       al
00467e5c  push       ecx
00467e5d  mov        dl, 0x69
00467e5f  mov        dword ptr [esp + 0x14], eax
00467e63  mov        eax, esi
00467e65  call       0x469610

// block 00467e6a
00467e6a  jmp        0x468bea

// block 00467e6f
00467e6f  mov        eax, dword ptr [edi + 0x1008]
00467e75  lea        ecx, [edi + 8]
00467e78  add        eax, -4
00467e7b  cmp        eax, ebx
00467e7d  jl         0x467eb1

// block 00467e7f
00467e7f  mov        dword ptr [ecx + 0x1000], eax
00467e85  mov        edx, dword ptr [ecx + eax]
00467e88  add        eax, -4
00467e8b  mov        dword ptr [ecx + 0x1000], eax
00467e91  mov        al, byte ptr [ecx + eax]
00467e94  mov        dword ptr [esp + 0x108], edx
00467e9b  cmp        al, 0x66
00467e9d  je         0x467eb1

// block 00467e9f
00467e9f  cmp        al, 0x69
00467ea1  jne        0x467eb1

// block 00467ea3
00467ea3  fild       dword ptr [esp + 0x108]
00467eaa  fstp       dword ptr [esp + 0x108]

// block 00467eb1
00467eb1  mov        eax, dword ptr [ecx + 0x1000]
00467eb7  add        eax, -4
00467eba  cmp        eax, ebx
00467ebc  jl         0x467ef0

// block 00467ebe
00467ebe  mov        dword ptr [ecx + 0x1000], eax
00467ec4  mov        edx, dword ptr [ecx + eax]
00467ec7  add        eax, -4
00467eca  mov        dword ptr [ecx + 0x1000], eax
00467ed0  mov        al, byte ptr [ecx + eax]
00467ed3  mov        dword ptr [esp + 0xb0], edx
00467eda  cmp        al, 0x66
00467edc  je         0x467ef0

// block 00467ede
00467ede  cmp        al, 0x69
00467ee0  jne        0x467ef0

// block 00467ee2
00467ee2  fild       dword ptr [esp + 0xb0]
00467ee9  fstp       dword ptr [esp + 0xb0]

// block 00467ef0
00467ef0  fld        dword ptr [esp + 0xb0]
00467ef7  mov        dword ptr [esp + 0x18], ebp
00467efb  fld        dword ptr [esp + 0x108]
00467f02  fucompp    
00467f04  fnstsw     ax
00467f06  test       ah, 0x44
00467f09  jnp        0x467f0f

// block 00467f0b
00467f0b  mov        dword ptr [esp + 0x18], ebx

// block 00467f0f
00467f0f  lea        eax, [esp + 0x18]
00467f13  push       eax
00467f14  mov        dl, 0x69
00467f16  mov        eax, ecx
00467f18  call       0x469610

// block 00467f1d
00467f1d  jmp        0x468bea

// block 00467f22
00467f22  mov        eax, dword ptr [edi + 0x1008]
00467f28  lea        ecx, [edi + 8]
00467f2b  add        eax, -4
00467f2e  cmp        eax, ebx
00467f30  jl         0x467f64

// block 00467f32
00467f32  mov        dword ptr [ecx + 0x1000], eax
00467f38  mov        edx, dword ptr [ecx + eax]
00467f3b  add        eax, -4
00467f3e  mov        dword ptr [ecx + 0x1000], eax
00467f44  mov        al, byte ptr [ecx + eax]
00467f47  mov        dword ptr [esp + 0xf0], edx
00467f4e  cmp        al, 0x66
00467f50  je         0x467f64

// block 00467f52
00467f52  cmp        al, 0x69
00467f54  jne        0x467f64

// block 00467f56
00467f56  fild       dword ptr [esp + 0xf0]
00467f5d  fstp       dword ptr [esp + 0xf0]

// block 00467f64
00467f64  mov        eax, dword ptr [ecx + 0x1000]
00467f6a  add        eax, -4
00467f6d  cmp        eax, ebx
00467f6f  jl         0x467fa3

// block 00467f71
00467f71  mov        dword ptr [ecx + 0x1000], eax
00467f77  mov        edx, dword ptr [ecx + eax]
00467f7a  add        eax, -4
00467f7d  mov        dword ptr [ecx + 0x1000], eax
00467f83  mov        al, byte ptr [ecx + eax]
00467f86  mov        dword ptr [esp + 0xb8], edx
00467f8d  cmp        al, 0x66
00467f8f  je         0x467fa3

// block 00467f91
00467f91  cmp        al, 0x69
00467f93  jne        0x467fa3

// block 00467f95
00467f95  fild       dword ptr [esp + 0xb8]
00467f9c  fstp       dword ptr [esp + 0xb8]

// block 00467fa3
00467fa3  fld        dword ptr [esp + 0xb8]
00467faa  mov        dword ptr [esp + 0x18], ebp
00467fae  fld        dword ptr [esp + 0xf0]
00467fb5  fucompp    
00467fb7  fnstsw     ax
00467fb9  test       ah, 0x44
00467fbc  jp         0x467f0f

// block 00467fc2
00467fc2  lea        eax, [esp + 0x18]
00467fc6  push       eax
00467fc7  mov        dl, 0x69
00467fc9  mov        eax, ecx
00467fcb  mov        dword ptr [esp + 0x1c], ebx
00467fcf  call       0x469610

// block 00467fd4
00467fd4  jmp        0x468bea

// block 00467fd9
00467fd9  mov        eax, dword ptr [edi + 0x1008]
00467fdf  lea        ecx, [edi + 8]
00467fe2  add        eax, -4
00467fe5  cmp        eax, ebx
00467fe7  jl         0x468012

// block 00467fe9
00467fe9  mov        dword ptr [ecx + 0x1000], eax
00467fef  mov        edx, dword ptr [ecx + eax]
00467ff2  add        eax, -4
00467ff5  mov        dword ptr [ecx + 0x1000], eax
00467ffb  mov        al, byte ptr [ecx + eax]
00467ffe  mov        dword ptr [esp + 0x5c], edx
00468002  cmp        al, 0x66
00468004  je         0x468012

// block 00468006
00468006  cmp        al, 0x69
00468008  jne        0x468012

// block 0046800a
0046800a  fild       dword ptr [esp + 0x5c]
0046800e  fstp       dword ptr [esp + 0x5c]

// block 00468012
00468012  mov        eax, dword ptr [ecx + 0x1000]
00468018  add        eax, -4
0046801b  cmp        eax, ebx
0046801d  jl         0x468051

// block 0046801f
0046801f  mov        dword ptr [ecx + 0x1000], eax
00468025  mov        edx, dword ptr [eax + ecx]
00468028  add        eax, -4
0046802b  mov        dword ptr [ecx + 0x1000], eax
00468031  mov        al, byte ptr [eax + ecx]
00468034  mov        dword ptr [esp + 0xc0], edx
0046803b  cmp        al, 0x66
0046803d  je         0x468051

// block 0046803f
0046803f  cmp        al, 0x69
00468041  jne        0x468051

// block 00468043
00468043  fild       dword ptr [esp + 0xc0]
0046804a  fstp       dword ptr [esp + 0xc0]

// block 00468051
00468051  fld        dword ptr [esp + 0xc0]
00468058  mov        dword ptr [esp + 0x18], ebp
0046805c  fld        dword ptr [esp + 0x5c]
00468060  fcompp     
00468062  fnstsw     ax
00468064  test       ah, 0x41
00468067  je         0x467f0f

// block 0046806d
0046806d  lea        eax, [esp + 0x18]
00468071  push       eax
00468072  mov        dl, 0x69
00468074  mov        eax, ecx
00468076  mov        dword ptr [esp + 0x1c], ebx
0046807a  call       0x469610

// block 0046807f
0046807f  jmp        0x468bea

// block 00468084
00468084  mov        eax, dword ptr [edi + 0x1008]
0046808a  lea        ecx, [edi + 8]
0046808d  add        eax, -4
00468090  cmp        eax, ebx
00468092  jl         0x4680bd

// block 00468094
00468094  mov        dword ptr [ecx + 0x1000], eax
0046809a  mov        edx, dword ptr [eax + ecx]
0046809d  add        eax, -4
004680a0  mov        dword ptr [ecx + 0x1000], eax
004680a6  mov        al, byte ptr [eax + ecx]
004680a9  mov        dword ptr [esp + 0x6c], edx
004680ad  cmp        al, 0x66
004680af  je         0x4680bd

// block 004680b1
004680b1  cmp        al, 0x69
004680b3  jne        0x4680bd

// block 004680b5
004680b5  fild       dword ptr [esp + 0x6c]
004680b9  fstp       dword ptr [esp + 0x6c]

// block 004680bd
004680bd  mov        eax, dword ptr [ecx + 0x1000]
004680c3  add        eax, -4
004680c6  cmp        eax, ebx
004680c8  jl         0x4680f3

// block 004680ca
004680ca  mov        dword ptr [ecx + 0x1000], eax
004680d0  mov        edx, dword ptr [eax + ecx]
004680d3  add        eax, -4
004680d6  mov        dword ptr [ecx + 0x1000], eax
004680dc  mov        al, byte ptr [eax + ecx]
004680df  mov        dword ptr [esp + 0x64], edx
004680e3  cmp        al, 0x66
004680e5  je         0x4680f3

// block 004680e7
004680e7  cmp        al, 0x69
004680e9  jne        0x4680f3

// block 004680eb
004680eb  fild       dword ptr [esp + 0x64]
004680ef  fstp       dword ptr [esp + 0x64]

// block 004680f3
004680f3  fld        dword ptr [esp + 0x64]
004680f7  mov        dword ptr [esp + 0x18], ebp
004680fb  fld        dword ptr [esp + 0x6c]
004680ff  fcompp     
00468101  fnstsw     ax
00468103  test       ah, 1
00468106  je         0x467f0f

// block 0046810c
0046810c  lea        eax, [esp + 0x18]
00468110  push       eax
00468111  mov        dl, 0x69
00468113  mov        eax, ecx
00468115  mov        dword ptr [esp + 0x1c], ebx
00468119  call       0x469610

// block 0046811e
0046811e  jmp        0x468bea

// block 00468123
00468123  mov        eax, dword ptr [edi + 0x1008]
00468129  lea        ecx, [edi + 8]
0046812c  add        eax, -4
0046812f  cmp        eax, ebx
00468131  jl         0x46815c

// block 00468133
00468133  mov        dword ptr [ecx + 0x1000], eax
00468139  mov        edx, dword ptr [eax + ecx]
0046813c  add        eax, -4
0046813f  mov        dword ptr [ecx + 0x1000], eax
00468145  mov        al, byte ptr [eax + ecx]
00468148  mov        dword ptr [esp + 0x7c], edx
0046814c  cmp        al, 0x66
0046814e  je         0x46815c

// block 00468150
00468150  cmp        al, 0x69
00468152  jne        0x46815c

// block 00468154
00468154  fild       dword ptr [esp + 0x7c]
00468158  fstp       dword ptr [esp + 0x7c]

// block 0046815c
0046815c  mov        eax, dword ptr [ecx + 0x1000]
00468162  add        eax, -4
00468165  cmp        eax, ebx
00468167  jl         0x468192

// block 00468169
00468169  mov        dword ptr [ecx + 0x1000], eax
0046816f  mov        edx, dword ptr [eax + ecx]
00468172  add        eax, -4
00468175  mov        dword ptr [ecx + 0x1000], eax
0046817b  mov        al, byte ptr [eax + ecx]
0046817e  mov        dword ptr [esp + 0x74], edx
00468182  cmp        al, 0x66
00468184  je         0x468192

// block 00468186
00468186  cmp        al, 0x69
00468188  jne        0x468192

// block 0046818a
0046818a  fild       dword ptr [esp + 0x74]
0046818e  fstp       dword ptr [esp + 0x74]

// block 00468192
00468192  fld        dword ptr [esp + 0x74]
00468196  mov        dword ptr [esp + 0x18], ebp
0046819a  fld        dword ptr [esp + 0x7c]
0046819e  fcompp     
004681a0  fnstsw     ax
004681a2  test       ah, 5
004681a5  jnp        0x467f0f

// block 004681ab
004681ab  lea        eax, [esp + 0x18]
004681af  push       eax
004681b0  mov        dl, 0x69
004681b2  mov        eax, ecx
004681b4  mov        dword ptr [esp + 0x1c], ebx
004681b8  call       0x469610

// block 004681bd
004681bd  jmp        0x468bea

// block 004681c2
004681c2  mov        eax, dword ptr [edi + 0x1008]
004681c8  lea        ecx, [edi + 8]
004681cb  add        eax, -4
004681ce  cmp        eax, ebx
004681d0  jl         0x468204

// block 004681d2
004681d2  mov        dword ptr [ecx + 0x1000], eax
004681d8  mov        edx, dword ptr [eax + ecx]
004681db  add        eax, -4
004681de  mov        dword ptr [ecx + 0x1000], eax
004681e4  mov        al, byte ptr [eax + ecx]
004681e7  mov        dword ptr [esp + 0x8c], edx
004681ee  cmp        al, 0x66
004681f0  je         0x468204

// block 004681f2
004681f2  cmp        al, 0x69
004681f4  jne        0x468204

// block 004681f6
004681f6  fild       dword ptr [esp + 0x8c]
004681fd  fstp       dword ptr [esp + 0x8c]

// block 00468204
00468204  mov        eax, dword ptr [ecx + 0x1000]
0046820a  add        eax, -4
0046820d  cmp        eax, ebx
0046820f  jl         0x468243

// block 00468211
00468211  mov        dword ptr [ecx + 0x1000], eax
00468217  mov        edx, dword ptr [eax + ecx]
0046821a  add        eax, -4
0046821d  mov        dword ptr [ecx + 0x1000], eax
00468223  mov        al, byte ptr [eax + ecx]
00468226  mov        dword ptr [esp + 0x84], edx
0046822d  cmp        al, 0x66
0046822f  je         0x468243

// block 00468231
00468231  cmp        al, 0x69
00468233  jne        0x468243

// block 00468235
00468235  fild       dword ptr [esp + 0x84]
0046823c  fstp       dword ptr [esp + 0x84]

// block 00468243
00468243  fld        dword ptr [esp + 0x84]
0046824a  mov        dword ptr [esp + 0x18], ebp
0046824e  fld        dword ptr [esp + 0x8c]
00468255  fcompp     
00468257  fnstsw     ax
00468259  test       ah, 0x41
0046825c  jnp        0x467f0f

// block 00468262
00468262  lea        eax, [esp + 0x18]
00468266  push       eax
00468267  mov        dl, 0x69
00468269  mov        eax, ecx
0046826b  mov        dword ptr [esp + 0x1c], ebx
0046826f  call       0x469610

// block 00468274
00468274  jmp        0x468bea

// block 00468279
00468279  mov        eax, dword ptr [edi + 0x1008]
0046827f  lea        ecx, [edi + 8]
00468282  add        eax, -4
00468285  cmp        eax, ebx
00468287  jl         0x4682bb

// block 00468289
00468289  mov        dword ptr [ecx + 0x1000], eax
0046828f  mov        edx, dword ptr [eax + ecx]
00468292  add        eax, -4
00468295  mov        dword ptr [ecx + 0x1000], eax
0046829b  mov        al, byte ptr [eax + ecx]
0046829e  mov        dword ptr [esp + 0x94], edx
004682a5  cmp        al, 0x66
004682a7  je         0x4682bb

// block 004682a9
004682a9  cmp        al, 0x69
004682ab  jne        0x4682bb

// block 004682ad
004682ad  fild       dword ptr [esp + 0x94]
004682b4  fstp       dword ptr [esp + 0x94]

// block 004682bb
004682bb  fldz       
004682bd  mov        dword ptr [esp + 0x18], ebp
004682c1  fcomp      dword ptr [esp + 0x94]
004682c8  fnstsw     ax
004682ca  test       ah, 0x44
004682cd  jnp        0x467f0f

// block 004682d3
004682d3  lea        eax, [esp + 0x18]
004682d7  push       eax
004682d8  mov        dl, 0x69
004682da  mov        eax, ecx
004682dc  mov        dword ptr [esp + 0x1c], ebx
004682e0  call       0x469610

// block 004682e5
004682e5  jmp        0x468bea

// block 004682ea
004682ea  mov        eax, dword ptr [edi + 0x1008]
004682f0  lea        esi, [edi + 8]
004682f3  add        eax, -4
004682f6  cmp        eax, ebx
004682f8  jl         0x46832c

// block 004682fa
004682fa  mov        dword ptr [esi + 0x1000], eax
00468300  mov        ecx, dword ptr [eax + esi]
00468303  add        eax, -4
00468306  mov        dword ptr [esi + 0x1000], eax
0046830c  cmp        byte ptr [eax + esi], 0x66
00468310  mov        dword ptr [esp + 0xa4], ecx
00468317  jne        0x46832c

// block 00468319
00468319  fld        dword ptr [esp + 0xa4]
00468320  call       0x4931e0

// block 00468325
00468325  mov        dword ptr [esp + 0xa4], eax

// block 0046832c
0046832c  mov        eax, dword ptr [esi + 0x1000]
00468332  add        eax, -4
00468335  cmp        eax, ebx
00468337  jl         0x46836b

// block 00468339
00468339  mov        dword ptr [esi + 0x1000], eax
0046833f  mov        edx, dword ptr [eax + esi]
00468342  add        eax, -4
00468345  mov        dword ptr [esi + 0x1000], eax
0046834b  cmp        byte ptr [eax + esi], 0x66
0046834f  mov        dword ptr [esp + 0x9c], edx
00468356  jne        0x46836b

// block 00468358
00468358  fld        dword ptr [esp + 0x9c]
0046835f  call       0x4931e0

// block 00468364
00468364  mov        dword ptr [esp + 0x9c], eax

// block 0046836b
0046836b  cmp        dword ptr [esp + 0x9c], ebx
00468372  jne        0x468381

// block 00468374
00468374  mov        dword ptr [esp + 0x18], ebx
00468378  cmp        dword ptr [esp + 0xa4], ebx
0046837f  je         0x468385

// block 00468381
00468381  mov        dword ptr [esp + 0x18], ebp

// block 00468385
00468385  lea        eax, [esp + 0x18]
00468389  push       eax
0046838a  mov        dl, 0x69
0046838c  mov        eax, esi
0046838e  call       0x469610

// block 00468393
00468393  jmp        0x468bea

// block 00468398
00468398  mov        eax, dword ptr [edi + 0x1008]
0046839e  lea        esi, [edi + 8]
004683a1  add        eax, -4
004683a4  cmp        eax, ebx
004683a6  jl         0x4683da

// block 004683a8
004683a8  mov        dword ptr [esi + 0x1000], eax
004683ae  mov        ecx, dword ptr [eax + esi]
004683b1  add        eax, -4
004683b4  mov        dword ptr [esi + 0x1000], eax
004683ba  cmp        byte ptr [eax + esi], 0x66
004683be  mov        dword ptr [esp + 0xb4], ecx
004683c5  jne        0x4683da

// block 004683c7
004683c7  fld        dword ptr [esp + 0xb4]
004683ce  call       0x4931e0

// block 004683d3
004683d3  mov        dword ptr [esp + 0xb4], eax

// block 004683da
004683da  mov        eax, dword ptr [esi + 0x1000]
004683e0  add        eax, -4
004683e3  cmp        eax, ebx
004683e5  jl         0x468419

// block 004683e7
004683e7  mov        dword ptr [esi + 0x1000], eax
004683ed  mov        edx, dword ptr [eax + esi]
004683f0  add        eax, -4
004683f3  mov        dword ptr [esi + 0x1000], eax
004683f9  cmp        byte ptr [eax + esi], 0x66
004683fd  mov        dword ptr [esp + 0xac], edx
00468404  jne        0x468419

// block 00468406
00468406  fld        dword ptr [esp + 0xac]
0046840d  call       0x4931e0

// block 00468412
00468412  mov        dword ptr [esp + 0xac], eax

// block 00468419
00468419  cmp        dword ptr [esp + 0xac], ebx
00468420  je         0x46842f

// block 00468422
00468422  mov        dword ptr [esp + 0x18], ebp
00468426  cmp        dword ptr [esp + 0xb4], ebx
0046842d  jne        0x468433

// block 0046842f
0046842f  mov        dword ptr [esp + 0x18], ebx

// block 00468433
00468433  lea        eax, [esp + 0x18]
00468437  push       eax
00468438  mov        dl, 0x69
0046843a  mov        eax, esi
0046843c  call       0x469610

// block 00468441
00468441  jmp        0x468bea

// block 00468446
00468446  mov        eax, dword ptr [edi + 0x1008]
0046844c  lea        esi, [edi + 8]
0046844f  add        eax, -4
00468452  cmp        eax, ebx
00468454  jl         0x468488

// block 00468456
00468456  mov        dword ptr [esi + 0x1000], eax
0046845c  mov        ecx, dword ptr [eax + esi]
0046845f  add        eax, -4
00468462  mov        dword ptr [esi + 0x1000], eax
00468468  cmp        byte ptr [eax + esi], 0x66
0046846c  mov        dword ptr [esp + 0xc4], ecx
00468473  jne        0x468488

// block 00468475
00468475  fld        dword ptr [esp + 0xc4]
0046847c  call       0x4931e0

// block 00468481
00468481  mov        dword ptr [esp + 0xc4], eax

// block 00468488
00468488  mov        eax, dword ptr [esi + 0x1000]
0046848e  add        eax, -4
00468491  cmp        eax, ebx
00468493  jl         0x4684c7

// block 00468495
00468495  mov        dword ptr [esi + 0x1000], eax
0046849b  mov        edx, dword ptr [eax + esi]
0046849e  add        eax, -4
004684a1  mov        dword ptr [esi + 0x1000], eax
004684a7  cmp        byte ptr [eax + esi], 0x66
004684ab  mov        dword ptr [esp + 0xbc], edx
004684b2  jne        0x4684c7

// block 004684b4
004684b4  fld        dword ptr [esp + 0xbc]
004684bb  call       0x4931e0

// block 004684c0
004684c0  mov        dword ptr [esp + 0xbc], eax

// block 004684c7
004684c7  mov        eax, dword ptr [esp + 0xbc]
004684ce  xor        eax, dword ptr [esp + 0xc4]
004684d5  lea        ecx, [esp + 0x10]
004684d9  mov        dword ptr [esp + 0x10], eax
004684dd  push       ecx
004684de  mov        dl, 0x69
004684e0  mov        eax, esi
004684e2  call       0x469610

// block 004684e7
004684e7  jmp        0x468bea

// block 004684ec
004684ec  mov        eax, dword ptr [edi + 0x1008]
004684f2  lea        esi, [edi + 8]
004684f5  add        eax, -4
004684f8  cmp        eax, ebx
004684fa  jl         0x46852e

// block 004684fc
004684fc  mov        dword ptr [esi + 0x1000], eax
00468502  mov        edx, dword ptr [eax + esi]
00468505  add        eax, -4
00468508  mov        dword ptr [esi + 0x1000], eax
0046850e  cmp        byte ptr [eax + esi], 0x66
00468512  mov        dword ptr [esp + 0xd4], edx
00468519  jne        0x46852e

// block 0046851b
0046851b  fld        dword ptr [esp + 0xd4]
00468522  call       0x4931e0

// block 00468527
00468527  mov        dword ptr [esp + 0xd4], eax

// block 0046852e
0046852e  mov        eax, dword ptr [esi + 0x1000]
00468534  add        eax, -4
00468537  cmp        eax, ebx
00468539  jl         0x46856d

// block 0046853b
0046853b  mov        dword ptr [esi + 0x1000], eax
00468541  mov        ecx, dword ptr [eax + esi]
00468544  add        eax, -4
00468547  mov        dword ptr [esi + 0x1000], eax
0046854d  cmp        byte ptr [eax + esi], 0x66
00468551  mov        dword ptr [esp + 0xcc], ecx
00468558  jne        0x46856d

// block 0046855a
0046855a  fld        dword ptr [esp + 0xcc]
00468561  call       0x4931e0

// block 00468566
00468566  mov        dword ptr [esp + 0xcc], eax

// block 0046856d
0046856d  mov        edx, dword ptr [esp + 0xcc]
00468574  or         edx, dword ptr [esp + 0xd4]
0046857b  lea        eax, [esp + 0x10]
0046857f  mov        dword ptr [esp + 0x10], edx
00468583  push       eax
00468584  mov        dl, 0x69
00468586  mov        eax, esi
00468588  call       0x469610

// block 0046858d
0046858d  jmp        0x468bea

// block 00468592
00468592  mov        eax, dword ptr [edi + 0x1008]
00468598  lea        esi, [edi + 8]
0046859b  add        eax, -4
0046859e  cmp        eax, ebx
004685a0  jl         0x4685d4

// block 004685a2
004685a2  mov        dword ptr [esi + 0x1000], eax
004685a8  mov        ecx, dword ptr [eax + esi]
004685ab  add        eax, -4
004685ae  mov        dword ptr [esi + 0x1000], eax
004685b4  cmp        byte ptr [eax + esi], 0x66
004685b8  mov        dword ptr [esp + 0xe4], ecx
004685bf  jne        0x4685d4

// block 004685c1
004685c1  fld        dword ptr [esp + 0xe4]
004685c8  call       0x4931e0

// block 004685cd
004685cd  mov        dword ptr [esp + 0xe4], eax

// block 004685d4
004685d4  mov        eax, dword ptr [esi + 0x1000]
004685da  add        eax, -4
004685dd  cmp        eax, ebx
004685df  jl         0x468613

// block 004685e1
004685e1  mov        dword ptr [esi + 0x1000], eax
004685e7  mov        edx, dword ptr [eax + esi]
004685ea  add        eax, -4
004685ed  mov        dword ptr [esi + 0x1000], eax
004685f3  cmp        byte ptr [eax + esi], 0x66
004685f7  mov        dword ptr [esp + 0xdc], edx
004685fe  jne        0x468613

// block 00468600
00468600  fld        dword ptr [esp + 0xdc]
00468607  call       0x4931e0

// block 0046860c
0046860c  mov        dword ptr [esp + 0xdc], eax

// block 00468613
00468613  mov        eax, dword ptr [esp + 0xdc]
0046861a  and        eax, dword ptr [esp + 0xe4]
00468621  lea        ecx, [esp + 0x10]
00468625  mov        dword ptr [esp + 0x10], eax
00468629  push       ecx
0046862a  mov        dl, 0x69
0046862c  mov        eax, esi
0046862e  call       0x469610

// block 00468633
00468633  jmp        0x468bea

// block 00468638
00468638  mov        ecx, dword ptr [edi + 0x1008]
0046863e  lea        esi, [edi + 8]
00468641  add        ecx, -4
00468644  cmp        ecx, ebx
00468646  jl         0x468686

// block 00468648
00468648  mov        dword ptr [esi + 0x1000], ecx
0046864e  mov        eax, dword ptr [esi + ecx]
00468651  add        ecx, -4
00468654  mov        dword ptr [esi + 0x1000], ecx
0046865a  cmp        byte ptr [ecx + esi], 0x66
0046865e  mov        dword ptr [esp + 0x54], eax
00468662  jne        0x46868a

// block 00468664
00468664  fld        dword ptr [esp + 0x54]
00468668  call       0x4931e0

// block 0046866d
0046866d  neg        eax
0046866f  lea        edx, [esp + 0x54]
00468673  mov        dword ptr [esp + 0x54], eax
00468677  push       edx
00468678  mov        dl, 0x69
0046867a  mov        eax, esi
0046867c  call       0x469610

// block 00468681
00468681  jmp        0x468bea

// block 00468686
00468686  mov        eax, dword ptr [esp + 0x54]

// block 0046868a
0046868a  neg        eax
0046868c  lea        edx, [esp + 0x54]
00468690  mov        dword ptr [esp + 0x54], eax
00468694  push       edx
00468695  mov        dl, 0x69
00468697  mov        eax, esi
00468699  call       0x469610

// block 0046869e
0046869e  jmp        0x468bea

// block 004686a3
004686a3  mov        ecx, dword ptr [edi + 0x1008]
004686a9  lea        eax, [edi + 8]
004686ac  add        ecx, -4
004686af  cmp        ecx, ebx
004686b1  jl         0x4686de

// block 004686b3
004686b3  mov        dword ptr [eax + 0x1000], ecx
004686b9  mov        edx, dword ptr [eax + ecx]
004686bc  add        ecx, -4
004686bf  mov        dword ptr [eax + 0x1000], ecx
004686c5  mov        cl, byte ptr [ecx + eax]
004686c8  mov        dword ptr [esp + 0x28], edx
004686cc  cmp        cl, 0x66
004686cf  je         0x4686e2

// block 004686d1
004686d1  cmp        cl, 0x69
004686d4  jne        0x4686e2

// block 004686d6
004686d6  fild       dword ptr [esp + 0x28]
004686da  fstp       dword ptr [esp + 0x28]

// block 004686de
004686de  mov        edx, dword ptr [esp + 0x28]

// block 004686e2
004686e2  neg        edx
004686e4  lea        ecx, [esp + 0x28]
004686e8  mov        dword ptr [esp + 0x28], edx
004686ec  push       ecx
004686ed  mov        dl, 0x66
004686ef  call       0x469610

// block 004686f4
004686f4  jmp        0x468bea

// block 004686f9
004686f9  push       ebx
004686fa  mov        edx, edi
004686fc  call       0x468e20

// block 00468701
00468701  mov        esi, eax
00468703  mov        eax, edi
00468705  mov        dword ptr [esp + 0x10], esi
00468709  call       0x469080

// block 0046870e
0046870e  lea        edx, [esp + 0x10]
00468712  dec        esi
00468713  mov        dword ptr [eax], esi
00468715  push       edx
00468716  lea        eax, [edi + 8]
00468719  mov        dl, 0x69
0046871b  call       0x469610

// block 00468720
00468720  jmp        0x468bea

// block 00468725
00468725  mov        eax, dword ptr [edi + 0x1008]
0046872b  lea        esi, [edi + 8]
0046872e  add        eax, -4
00468731  cmp        eax, ebx
00468733  jl         0x468767

// block 00468735
00468735  mov        dword ptr [esi + 0x1000], eax
0046873b  mov        ecx, dword ptr [esi + eax]
0046873e  add        eax, -4
00468741  mov        dword ptr [esi + 0x1000], eax
00468747  mov        al, byte ptr [eax + esi]
0046874a  mov        dword ptr [esp + 0xec], ecx
00468751  cmp        al, 0x66
00468753  je         0x468767

// block 00468755
00468755  cmp        al, 0x69
00468757  jne        0x468767

// block 00468759
00468759  fild       dword ptr [esp + 0xec]
00468760  fstp       dword ptr [esp + 0xec]

// block 00468767
00468767  fld        dword ptr [esp + 0xec]
0046876e  push       ecx
0046876f  fstp       dword ptr [esp]
00468772  call       0x406680

// block 00468777
00468777  lea        edx, [esp + 0x10]
0046877b  fstp       dword ptr [esp + 0x10]
0046877f  push       edx
00468780  mov        dl, 0x66
00468782  mov        eax, esi
00468784  call       0x469610

// block 00468789
00468789  jmp        0x468bea

// block 0046878e
0046878e  mov        eax, dword ptr [edi + 0x1008]
00468794  lea        esi, [edi + 8]
00468797  add        eax, -4
0046879a  cmp        eax, ebx
0046879c  jl         0x4687d0

// block 0046879e
0046879e  mov        dword ptr [esi + 0x1000], eax
004687a4  mov        ecx, dword ptr [esi + eax]
004687a7  add        eax, -4
004687aa  mov        dword ptr [esi + 0x1000], eax
004687b0  mov        al, byte ptr [eax + esi]
004687b3  mov        dword ptr [esp + 0xf4], ecx
004687ba  cmp        al, 0x66
004687bc  je         0x4687d0

// block 004687be
004687be  cmp        al, 0x69
004687c0  jne        0x4687d0

// block 004687c2
004687c2  fild       dword ptr [esp + 0xf4]
004687c9  fstp       dword ptr [esp + 0xf4]

// block 004687d0
004687d0  fld        dword ptr [esp + 0xf4]
004687d7  push       ecx
004687d8  fstp       dword ptr [esp]
004687db  call       0x408860

// block 004687e0
004687e0  lea        edx, [esp + 0x10]
004687e4  fstp       dword ptr [esp + 0x10]
004687e8  push       edx
004687e9  mov        dl, 0x66
004687eb  mov        eax, esi
004687ed  call       0x469610

// block 004687f2
004687f2  jmp        0x468bea

// block 004687f7
004687f7  mov        eax, dword ptr [edi + 0x1008]
004687fd  lea        esi, [edi + 8]
00468800  add        eax, -4
00468803  cmp        eax, ebx
00468805  jl         0x468839

// block 00468807
00468807  mov        dword ptr [esi + 0x1000], eax
0046880d  mov        ecx, dword ptr [esi + eax]
00468810  add        eax, -4
00468813  mov        dword ptr [esi + 0x1000], eax
00468819  mov        al, byte ptr [eax + esi]
0046881c  mov        dword ptr [esp + 0xfc], ecx
00468823  cmp        al, 0x66
00468825  je         0x468839

// block 00468827
00468827  cmp        al, 0x69
00468829  jne        0x468839

// block 0046882b
0046882b  fild       dword ptr [esp + 0xfc]
00468832  fstp       dword ptr [esp + 0xfc]

// block 00468839
00468839  fld        dword ptr [esp + 0xfc]
00468840  push       ecx
00468841  fstp       dword ptr [esp]
00468844  call       0x406170

// block 00468849
00468849  lea        edx, [esp + 0x10]
0046884d  fstp       dword ptr [esp + 0x10]
00468851  push       edx
00468852  mov        dl, 0x66
00468854  mov        eax, esi
00468856  call       0x469610

// block 0046885b
0046885b  jmp        0x468bea

// block 00468860
00468860  mov        ecx, 3
00468865  mov        eax, edi
00468867  call       0x468ec0

// block 0046886c
0046886c  push       ecx
0046886d  mov        ecx, 2
00468872  fstp       dword ptr [esp]
00468875  mov        eax, edi
00468877  call       0x468ec0

// block 0046887c
0046887c  push       ecx
0046887d  fstp       dword ptr [esp]
00468880  call       0x4646e0

// block 00468885
00468885  push       ecx
00468886  lea        ecx, [esp + 0x12c]
0046888d  fstp       dword ptr [esp]
00468890  call       0x469270

// block 00468895
00468895  xor        ecx, ecx
00468897  mov        edx, edi
00468899  call       0x4690b0

// block 0046889e
0046889e  fld        dword ptr [esp + 0x124]
004688a5  mov        ecx, ebp
004688a7  fstp       dword ptr [eax]
004688a9  mov        edx, edi
004688ab  call       0x4690b0

// block 004688b0
004688b0  fld        dword ptr [esp + 0x128]
004688b7  fstp       dword ptr [eax]
004688b9  jmp        0x468bea

// block 004688be
004688be  mov        ecx, ebp
004688c0  mov        eax, edi
004688c2  call       0x468ec0

// block 004688c7
004688c7  fstp       dword ptr [esp + 0x20]
004688cb  mov        ecx, 2
004688d0  mov        eax, edi
004688d2  call       0x468ec0

// block 004688d7
004688d7  fstp       dword ptr [esp + 0x10]
004688db  fld        dword ptr [esp + 0x10]
004688df  xor        ecx, ecx
004688e1  fstp       qword ptr [esp + 0x18]
004688e5  mov        edx, edi
004688e7  fld        dword ptr [esp + 0x20]
004688eb  fstp       qword ptr [esp + 0x10]
004688ef  call       0x4690b0

// block 004688f4
004688f4  fld        qword ptr [esp + 0x10]
004688f8  fmul       st(0), st(0)
004688fa  fld        qword ptr [esp + 0x18]
004688fe  fmul       st(0), st(0)
00468900  faddp      st(1)
00468902  fstp       dword ptr [eax]
00468904  jmp        0x468bea

// block 00468909
00468909  xor        ecx, ecx
0046890b  mov        eax, edi
0046890d  call       0x468ec0

// block 00468912
00468912  push       ecx
00468913  fstp       dword ptr [esp]
00468916  call       0x4646e0

// block 0046891b
0046891b  xor        ecx, ecx
0046891d  fstp       dword ptr [esp + 0x10]
00468921  mov        edx, edi
00468923  call       0x4690b0

// block 00468928
00468928  fld        dword ptr [esp + 0x10]
0046892c  fstp       dword ptr [eax]
0046892e  jmp        0x468bea

// block 00468933
00468933  mov        ecx, 3
00468938  mov        eax, edi
0046893a  call       0x468ec0

// block 0046893f
0046893f  fstp       qword ptr [esp + 0x10]
00468943  mov        ecx, 4
00468948  mov        eax, edi
0046894a  call       0x468ec0

// block 0046894f
0046894f  fstp       qword ptr [esp + 0x20]
00468953  mov        ecx, ebp
00468955  mov        eax, edi
00468957  call       0x468ec0

// block 0046895c
0046895c  fsubr      qword ptr [esp + 0x10]
00468960  push       ecx
00468961  mov        ecx, 2
00468966  fstp       dword ptr [esp + 0x14]
0046896a  mov        eax, edi
0046896c  fld        dword ptr [esp + 0x14]
00468970  fstp       dword ptr [esp]
00468973  call       0x468ec0

// block 00468978
00468978  fsubr      qword ptr [esp + 0x24]
0046897c  push       ecx
0046897d  fstp       dword ptr [esp + 0x18]
00468981  fld        dword ptr [esp + 0x18]
00468985  fstp       dword ptr [esp]
00468988  call       0x4088c0

// block 0046898d
0046898d  xor        ecx, ecx
0046898f  fstp       dword ptr [esp + 0x10]
00468993  mov        edx, edi
00468995  call       0x4690b0

// block 0046899a
0046899a  fld        dword ptr [esp + 0x10]
0046899e  fstp       dword ptr [eax]
004689a0  jmp        0x468bea

// block 004689a5
004689a5  mov        ecx, ebp
004689a7  mov        eax, edi
004689a9  call       0x468ec0

// block 004689ae
004689ae  push       ecx
004689af  mov        ecx, 2
004689b4  fstp       dword ptr [esp]
004689b7  mov        eax, edi
004689b9  call       0x468ec0

// block 004689be
004689be  push       ecx
004689bf  fstp       dword ptr [esp]
004689c2  call       0x469200

// block 004689c7
004689c7  xor        ecx, ecx
004689c9  fstp       dword ptr [esp + 0x10]
004689cd  mov        edx, edi
004689cf  call       0x4690b0

// block 004689d4
004689d4  fld        dword ptr [esp + 0x10]
004689d8  fstp       dword ptr [eax]
004689da  jmp        0x468bea

// block 004689df
004689df  push       0x400
004689e4  add        esi, 0x14
004689e7  call       0x46d04a

// block 004689ec
004689ec  add        esp, 4
004689ef  mov        dword ptr [esp + 0x18], eax
004689f3  mov        byte ptr [eax], 0
004689f6  call       0x466ea0

// block 004689fb
004689fb  cmp        esi, ebx
004689fd  je         0x468bc5

// block 00468a03
00468a03  mov        ebx, 1
00468a08  mov        dword ptr [esp + 0x2c], 0
00468a10  lea        ebp, [ebx + 5]

// block 00468a13
00468a13  push       0x25
00468a15  push       esi
00468a16  call       0x46d1a0

// block 00468a1b
00468a1b  add        esp, 8
00468a1e  mov        dword ptr [esp + 0x10], eax
00468a22  test       eax, eax
00468a24  je         0x468bbb

// block 00468a2a
00468a2a  mov        edx, dword ptr [esp + 0x18]
00468a2e  mov        eax, esi
00468a30  sub        edx, esi

// block 00468a32
00468a32  mov        cl, byte ptr [eax]
00468a34  mov        byte ptr [edx + eax], cl
00468a37  inc        eax
00468a38  test       cl, cl
00468a3a  jne        0x468a32

// block 00468a3c
00468a3c  mov        eax, dword ptr [esp + 0x10]
00468a40  mov        edx, dword ptr [esp + 0x18]
00468a44  mov        ecx, eax
00468a46  sub        ecx, esi
00468a48  mov        byte ptr [ecx + edx], 0
00468a4c  call       0x466ea0

// block 00468a51
00468a51  movsx      eax, byte ptr [eax + 1]
00468a55  sub        eax, 0x25
00468a58  je         0x468ba7

// block 00468a5e
00468a5e  sub        eax, 0x3f
00468a61  je         0x468b11

// block 00468a67
00468a67  sub        eax, 2
00468a6a  jne        0x468bac

// block 00468a70
00468a70  mov        ecx, dword ptr [edi + 4]
00468a73  mov        eax, dword ptr [ecx + 0x10]
00468a76  mov        edx, dword ptr [esp + 0x2c]
00468a7a  add        edx, eax
00468a7c  mov        dl, byte ptr [edx + ecx + 0x14]
00468a80  cmp        dl, 0x66
00468a83  je         0x468abb

// block 00468a85
00468a85  cmp        dl, 0x67
00468a88  je         0x468abb

// block 00468a8a
00468a8a  cdq        
00468a8b  and        edx, 3
00468a8e  add        eax, edx
00468a90  movzx      edx, word ptr [ecx + 8]
00468a94  sar        eax, 2
00468a97  add        eax, ebp
00468a99  mov        eax, dword ptr [ecx + eax*4]
00468a9c  mov        esi, 1
00468aa1  mov        ecx, ebx
00468aa3  shl        esi, cl
00468aa5  test       esi, edx
00468aa7  je         0x468ab1

// block 00468aa9
00468aa9  push       eax
00468aaa  mov        ecx, edi
00468aac  call       0x468f70

// block 00468ab1
00468ab1  mov        dword ptr [esp + 0x20], eax
00468ab5  fild       dword ptr [esp + 0x20]
00468ab9  jmp        0x468af5

// block 00468abb
00468abb  cdq        
00468abc  and        edx, 3
00468abf  add        eax, edx
00468ac1  sar        eax, 2
00468ac4  add        eax, ebp
00468ac6  mov        edx, 1
00468acb  fld        dword ptr [ecx + eax*4]
00468ace  movzx      eax, word ptr [ecx + 8]
00468ad2  fstp       dword ptr [esp + 0x20]
00468ad6  fld        dword ptr [esp + 0x20]
00468ada  mov        ecx, ebx
00468adc  shl        edx, cl
00468ade  test       edx, eax
00468ae0  je         0x468aed

// block 00468ae2
00468ae2  push       ecx
00468ae3  mov        eax, edi
00468ae5  fstp       dword ptr [esp]
00468ae8  call       0x468fe0

// block 00468aed
00468aed  fstp       dword ptr [esp + 0x20]
00468af1  fld        dword ptr [esp + 0x20]

// block 00468af5
00468af5  sub        esp, 8
00468af8  fstp       qword ptr [esp]
00468afb  call       0x466ea0

// block 00468b00
00468b00  add        dword ptr [esp + 0x34], 8
00468b05  add        esp, 8
00468b08  add        ebp, 2
00468b0b  inc        ebx
00468b0c  jmp        0x468bac

// block 00468b11
00468b11  mov        ecx, dword ptr [edi + 4]
00468b14  mov        eax, dword ptr [ecx + 0x10]
00468b17  mov        edx, dword ptr [esp + 0x2c]
00468b1b  add        edx, eax
00468b1d  mov        dl, byte ptr [edx + ecx + 0x14]
00468b21  cmp        dl, 0x66
00468b24  je         0x468b54

// block 00468b26
00468b26  cmp        dl, 0x67
00468b29  je         0x468b54

// block 00468b2b
00468b2b  cdq        
00468b2c  and        edx, 3
00468b2f  add        eax, edx
00468b31  movzx      edx, word ptr [ecx + 8]
00468b35  sar        eax, 2
00468b38  add        eax, ebp
00468b3a  mov        eax, dword ptr [ecx + eax*4]
00468b3d  mov        esi, 1
00468b42  mov        ecx, ebx
00468b44  shl        esi, cl
00468b46  test       esi, edx
00468b48  je         0x468b93

// block 00468b4a
00468b4a  push       eax
00468b4b  mov        ecx, edi
00468b4d  call       0x468f70

// block 00468b52
00468b52  jmp        0x468b93

// block 00468b54
00468b54  cdq        
00468b55  and        edx, 3
00468b58  add        eax, edx
00468b5a  sar        eax, 2
00468b5d  add        eax, ebp
00468b5f  mov        edx, 1
00468b64  fld        dword ptr [ecx + eax*4]
00468b67  movzx      eax, word ptr [ecx + 8]
00468b6b  fstp       dword ptr [esp + 0x20]
00468b6f  fld        dword ptr [esp + 0x20]
00468b73  mov        ecx, ebx
00468b75  shl        edx, cl
00468b77  test       edx, eax
00468b79  je         0x468b86

// block 00468b7b
00468b7b  push       ecx
00468b7c  mov        eax, edi
00468b7e  fstp       dword ptr [esp]
00468b81  call       0x468fe0

// block 00468b86
00468b86  fstp       dword ptr [esp + 0x20]
00468b8a  fld        dword ptr [esp + 0x20]
00468b8e  call       0x4931e0

// block 00468b93
00468b93  push       eax
00468b94  call       0x466ea0

// block 00468b99
00468b99  add        dword ptr [esp + 0x30], 8
00468b9e  add        esp, 4
00468ba1  add        ebp, 2
00468ba4  inc        ebx
00468ba5  jmp        0x468bac

// block 00468ba7
00468ba7  call       0x466ea0

// block 00468bac
00468bac  mov        esi, dword ptr [esp + 0x10]
00468bb0  add        esi, 2
00468bb3  jne        0x468a13

// block 00468bb9
00468bb9  jmp        0x468bc0

// block 00468bbb
00468bbb  call       0x466ea0

// block 00468bc0
00468bc0  xor        ebx, ebx
00468bc2  lea        ebp, [ebx + 1]

// block 00468bc5
00468bc5  call       0x466ea0

// block 00468bca
00468bca  mov        eax, dword ptr [esp + 0x18]
00468bce  push       eax
00468bcf  call       0x46c941

// block 00468bd4
00468bd4  add        esp, 4
00468bd7  jmp        0x468bea

// block 00468bd9
00468bd9  mov        ecx, dword ptr [edi + 0x1014]
00468bdf  mov        edx, dword ptr [ecx]
00468be1  mov        eax, dword ptr [edx]
00468be3  call       eax

// block 00468be5
00468be5  cmp        eax, -1
00468be8  je         0x468c15

// block 00468bea
00468bea  mov        eax, dword ptr [edi + 4]
00468bed  movzx      ecx, word ptr [eax + 6]
00468bf1  add        ecx, eax
00468bf3  mov        dword ptr [edi + 4], ecx

// block 00468bf6
00468bf6  mov        edx, dword ptr [edi + 4]
00468bf9  fild       dword ptr [edx]
00468bfb  fld        dword ptr [edi]
00468bfd  fcompp     
00468bff  fnstsw     ax
00468c01  test       ah, 1
00468c04  je         0x4671a0

// block 00468c0a
00468c0a  fld        dword ptr [edi]
00468c0c  fadd       dword ptr [esp + 0x134]
00468c13  fstp       dword ptr [edi]

// block 00468c15
00468c15  xor        eax, eax
00468c17  pop        edi
00468c18  pop        esi
00468c19  pop        ebp
00468c1a  pop        ebx
00468c1b  add        esp, 0x120
00468c21  ret        4

// block 00468c24
00468c24  mov        dword ptr [edi + 4], ebx

// block 00468c27
00468c27  pop        edi
00468c28  pop        esi
00468c29  pop        ebp
00468c2a  or         eax, 0xffffffff
00468c2d  pop        ebx
00468c2e  add        esp, 0x120
00468c34  ret        4
