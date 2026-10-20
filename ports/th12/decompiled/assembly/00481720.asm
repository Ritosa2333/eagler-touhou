
// block 00481720
00481720  mov        edi, edi
00481722  push       ebp
00481723  mov        ebp, esp
00481725  mov        eax, dword ptr [0x4b4318]
0048172a  mov        cl, byte ptr [eax]
0048172c  sub        esp, 0x30
0048172f  test       cl, cl
00481731  jne        0x481748

// block 00481733
00481733  push       dword ptr [ebp + 0xc]
00481736  push       1
00481738  push       dword ptr [ebp + 8]
0048173b  call       0x47ec26

// block 00481740
00481740  add        esp, 0xc

// block 00481743
00481743  mov        eax, dword ptr [ebp + 8]
00481746  leave      
00481747  ret        

// block 00481748
00481748  cmp        cl, 0x36
0048174b  jl         0x481752

// block 0048174d
0048174d  cmp        cl, 0x39
00481750  jle        0x481763

// block 00481752
00481752  cmp        cl, 0x5f
00481755  je         0x481763

// block 00481757
00481757  mov        ecx, dword ptr [ebp + 8]
0048175a  push       2
0048175c  call       0x47e1ce

// block 00481761
00481761  jmp        0x481743

// block 00481763
00481763  push       ebx
00481764  movsx      ebx, cl
00481767  sub        ebx, 0x36
0048176a  inc        eax
0048176b  mov        dword ptr [0x4b4318], eax
00481770  cmp        ebx, 0x29
00481773  jne        0x4817a3

// block 00481775
00481775  mov        cl, byte ptr [eax]
00481777  test       cl, cl
00481779  je         0x481791

// block 0048177b
0048177b  movsx      ebx, cl
0048177e  sub        ebx, 0x3d
00481781  inc        eax
00481782  cmp        ebx, 4
00481785  mov        dword ptr [0x4b4318], eax
0048178a  jl         0x4817ac

// block 0048178c
0048178c  cmp        ebx, 7
0048178f  jmp        0x4817aa

// block 00481791
00481791  push       dword ptr [ebp + 0xc]
00481794  push       1
00481796  push       dword ptr [ebp + 8]
00481799  call       0x47ec26

// block 0048179e
0048179e  add        esp, 0xc
004817a1  jmp        0x4817be

// block 004817a3
004817a3  test       ebx, ebx
004817a5  jl         0x4817ac

// block 004817a7
004817a7  cmp        ebx, 3

// block 004817aa
004817aa  jle        0x4817af

// block 004817ac
004817ac  or         ebx, 0xffffffff

// block 004817af
004817af  cmp        ebx, -1
004817b2  jne        0x4817c6

// block 004817b4
004817b4  mov        ecx, dword ptr [ebp + 8]
004817b7  push       2
004817b9  call       0x47e1ce

// block 004817be
004817be  mov        eax, dword ptr [ebp + 8]
004817c1  jmp        0x481a71

// block 004817c6
004817c6  and        dword ptr [ebp - 0x10], 0
004817ca  and        dword ptr [ebp - 0xc], 0xffff0000
004817d1  push       esi
004817d2  mov        esi, dword ptr [ebp + 0xc]
004817d5  mov        eax, dword ptr [esi]
004817d7  push       edi
004817d8  mov        dword ptr [ebp - 8], eax
004817db  mov        eax, dword ptr [esi + 4]
004817de  mov        edi, ebx
004817e0  and        edi, 2
004817e3  mov        dword ptr [ebp - 4], eax
004817e6  je         0x481896

// block 004817ec
004817ec  lea        eax, [ebp - 8]
004817ef  push       eax
004817f0  lea        eax, [ebp - 0x20]
004817f3  push       0x49df1c
004817f8  push       eax
004817f9  call       0x47ec4a

// block 004817fe
004817fe  mov        ecx, dword ptr [eax]
00481800  mov        eax, dword ptr [eax + 4]
00481803  mov        dword ptr [ebp - 4], eax
00481806  mov        eax, dword ptr [0x4b4318]
0048180b  add        esp, 0xc
0048180e  cmp        byte ptr [eax], 0
00481811  lea        eax, [ebp - 8]
00481814  mov        dword ptr [ebp - 8], ecx
00481817  push       eax
00481818  je         0x48183f

// block 0048181a
0048181a  lea        eax, [ebp - 0x20]
0048181d  push       eax
0048181e  lea        eax, [ebp - 0x28]
00481821  push       eax
00481822  call       0x4814b0

// block 00481827
00481827  push       eax
00481828  lea        eax, [ebp - 0x30]
0048182b  push       0x20
0048182d  push       eax
0048182e  call       0x47ec02

// block 00481833
00481833  add        esp, 0x10
00481836  mov        ecx, eax
00481838  call       0x47e9ae

// block 0048183d
0048183d  jmp        0x48184d

// block 0048183f
0048183f  lea        eax, [ebp - 0x30]
00481842  push       1
00481844  push       eax
00481845  call       0x47ec26

// block 0048184a
0048184a  add        esp, 0xc

// block 0048184d
0048184d  mov        ecx, dword ptr [eax]
0048184f  mov        eax, dword ptr [eax + 4]
00481852  mov        dword ptr [ebp - 4], eax
00481855  mov        eax, dword ptr [0x4b4318]
0048185a  mov        al, byte ptr [eax]
0048185c  mov        dword ptr [ebp - 8], ecx
0048185f  test       al, al
00481861  je         0x4818f8

// block 00481867
00481867  cmp        al, 0x40
00481869  jne        0x4818f1

// block 0048186f
0048186f  mov        eax, dword ptr [0x4b4328]
00481874  inc        dword ptr [0x4b4318]
0048187a  and        eax, 0x60
0048187d  cmp        al, 0x60
0048187f  lea        eax, [ebp - 0x30]
00481882  push       eax
00481883  je         0x4818e0

// block 00481885
00481885  call       0x47e0f7

// block 0048188a
0048188a  pop        ecx
0048188b  mov        ecx, dword ptr [eax]
0048188d  mov        eax, dword ptr [eax + 4]
00481890  mov        dword ptr [ebp - 0x10], ecx
00481893  mov        dword ptr [ebp - 0xc], eax

// block 00481896
00481896  test       bl, 4
00481899  je         0x481921

// block 0048189f
0048189f  mov        eax, dword ptr [0x4b4328]
004818a4  shr        eax, 1
004818a6  not        eax
004818a8  test       al, 1
004818aa  je         0x48190e

// block 004818ac
004818ac  lea        eax, [ebp - 8]
004818af  push       eax
004818b0  lea        eax, [ebp - 0x30]
004818b3  push       eax
004818b4  lea        eax, [ebp - 0x28]
004818b7  push       eax
004818b8  call       0x480665

// block 004818bd
004818bd  push       eax
004818be  lea        eax, [ebp - 0x20]
004818c1  push       0x20
004818c3  push       eax
004818c4  call       0x47ec02

// block 004818c9
004818c9  add        esp, 0x10
004818cc  mov        ecx, eax
004818ce  call       0x47e9ae

// block 004818d3
004818d3  mov        ecx, dword ptr [eax]
004818d5  mov        eax, dword ptr [eax + 4]
004818d8  mov        dword ptr [ebp - 8], ecx
004818db  mov        dword ptr [ebp - 4], eax
004818de  jmp        0x481921

// block 004818e0
004818e0  call       0x47e0f7

// block 004818e5
004818e5  pop        ecx
004818e6  push       eax
004818e7  lea        ecx, [ebp - 0x10]
004818ea  call       0x47ddc1

// block 004818ef
004818ef  jmp        0x481896

// block 004818f1
004818f1  push       2
004818f3  jmp        0x481a64

// block 004818f8
004818f8  lea        eax, [ebp - 8]
004818fb  push       eax
004818fc  push       1
004818fe  push       dword ptr [ebp + 8]
00481901  call       0x47ec26

// block 00481906
00481906  add        esp, 0xc
00481909  jmp        0x481a6c

// block 0048190e
0048190e  lea        eax, [ebp - 0x30]
00481911  push       eax
00481912  call       0x480665

// block 00481917
00481917  pop        ecx
00481918  push       eax
00481919  lea        ecx, [ebp - 8]
0048191c  call       0x47ddc1

// block 00481921
00481921  mov        eax, dword ptr [0x4b4328]
00481926  shr        eax, 1
00481928  not        eax
0048192a  test       al, 1
0048192c  je         0x481954

// block 0048192e
0048192e  lea        eax, [ebp - 8]
00481931  push       eax
00481932  lea        eax, [ebp - 0x30]
00481935  push       eax
00481936  lea        eax, [ebp - 0x28]
00481939  push       eax
0048193a  call       0x47e8cb

// block 0048193f
0048193f  pop        ecx
00481940  mov        ecx, eax
00481942  call       0x47e9ae

// block 00481947
00481947  mov        ecx, dword ptr [eax]
00481949  mov        eax, dword ptr [eax + 4]
0048194c  mov        dword ptr [ebp - 8], ecx
0048194f  mov        dword ptr [ebp - 4], eax
00481952  jmp        0x481967

// block 00481954
00481954  lea        eax, [ebp - 0x30]
00481957  push       eax
00481958  call       0x47e8cb

// block 0048195d
0048195d  pop        ecx
0048195e  push       eax
0048195f  lea        ecx, [ebp - 8]
00481962  call       0x47ddc1

// block 00481967
00481967  xor        ebx, ebx
00481969  cmp        dword ptr [esi], ebx
0048196b  je         0x481997

// block 0048196d
0048196d  push       0x29
0048196f  lea        eax, [ebp - 0x30]
00481972  push       eax
00481973  lea        eax, [ebp - 8]
00481976  push       eax
00481977  lea        eax, [ebp - 0x28]
0048197a  push       0x28
0048197c  push       eax
0048197d  call       0x47ec02

// block 00481982
00481982  add        esp, 0xc
00481985  mov        ecx, eax
00481987  call       0x47ec6e

// block 0048198c
0048198c  mov        ecx, dword ptr [eax]
0048198e  mov        eax, dword ptr [eax + 4]
00481991  mov        dword ptr [ebp - 8], ecx
00481994  mov        dword ptr [ebp - 4], eax

// block 00481997
00481997  push       ebx
00481998  push       8
0048199a  mov        ecx, 0x4b42f8
0048199f  call       0x47dc29

// block 004819a4
004819a4  cmp        eax, ebx
004819a6  je         0x4819b9

// block 004819a8
004819a8  mov        byte ptr [eax + 4], 0
004819ac  and        dword ptr [eax + 4], 0xffff00ff
004819b3  mov        dword ptr [eax], ebx
004819b5  mov        esi, eax
004819b7  jmp        0x4819bb

// block 004819b9
004819b9  xor        esi, esi

// block 004819bb
004819bb  lea        eax, [ebp - 0x18]
004819be  push       esi
004819bf  push       eax
004819c0  call       0x47e46b

// block 004819c5
004819c5  pop        ecx
004819c6  pop        ecx
004819c7  push       0x29
004819c9  lea        eax, [ebp - 0x30]
004819cc  push       eax
004819cd  lea        eax, [ebp - 0x28]
004819d0  push       eax
004819d1  call       0x47eedc

// block 004819d6
004819d6  push       eax
004819d7  lea        eax, [ebp - 0x20]
004819da  push       0x28
004819dc  push       eax
004819dd  call       0x47ec02

// block 004819e2
004819e2  add        esp, 0x10
004819e5  mov        ecx, eax
004819e7  call       0x47ec6e

// block 004819ec
004819ec  push       eax
004819ed  lea        ecx, [ebp - 8]
004819f0  call       0x47e7bf

// block 004819f5
004819f5  mov        eax, dword ptr [0x4b4328]
004819fa  and        eax, 0x60
004819fd  cmp        al, 0x60
004819ff  je         0x481a11

// block 00481a01
00481a01  cmp        edi, ebx
00481a03  je         0x481a11

// block 00481a05
00481a05  lea        eax, [ebp - 0x10]
00481a08  push       eax
00481a09  lea        ecx, [ebp - 8]
00481a0c  call       0x47e7bf

// block 00481a11
00481a11  mov        eax, dword ptr [0x4b4328]
00481a16  shr        eax, 8
00481a19  not        eax
00481a1b  test       al, 1
00481a1d  lea        eax, [ebp - 0x30]
00481a20  push       eax
00481a21  je         0x481a34

// block 00481a23
00481a23  call       0x47efb8

// block 00481a28
00481a28  pop        ecx
00481a29  push       eax
00481a2a  lea        ecx, [ebp - 8]
00481a2d  call       0x47e7bf

// block 00481a32
00481a32  jmp        0x481a43

// block 00481a34
00481a34  call       0x47efb8

// block 00481a39
00481a39  pop        ecx
00481a3a  push       eax
00481a3b  lea        ecx, [ebp - 8]
00481a3e  call       0x47ddc1

// block 00481a43
00481a43  cmp        esi, ebx
00481a45  je         0x481a62

// block 00481a47
00481a47  mov        eax, dword ptr [ebp - 8]
00481a4a  mov        dword ptr [esi], eax
00481a4c  mov        eax, dword ptr [ebp - 4]
00481a4f  mov        dword ptr [esi + 4], eax
00481a52  mov        ecx, dword ptr [ebp - 0x18]
00481a55  mov        eax, dword ptr [ebp + 8]
00481a58  mov        dword ptr [eax], ecx
00481a5a  mov        ecx, dword ptr [ebp - 0x14]
00481a5d  mov        dword ptr [eax + 4], ecx
00481a60  jmp        0x481a6f

// block 00481a62
00481a62  push       3

// block 00481a64
00481a64  mov        ecx, dword ptr [ebp + 8]
00481a67  call       0x47e1ce

// block 00481a6c
00481a6c  mov        eax, dword ptr [ebp + 8]

// block 00481a6f
00481a6f  pop        edi
00481a70  pop        esi

// block 00481a71
00481a71  pop        ebx
00481a72  leave      
00481a73  ret        
