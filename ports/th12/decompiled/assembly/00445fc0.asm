
// block 00445fc0
00445fc0  push       ecx
00445fc1  push       edi
00445fc2  mov        edi, eax
00445fc4  mov        eax, dword ptr [edi + 0x24]
00445fc7  cmp        eax, 4
00445fca  ja         0x44639b

// block 00445fd0
00445fd0  push       esi
00445fd1  jmp        dword ptr [eax*4 + 0x4463a4]

// block 00445fd8
00445fd8  mov        dword ptr [edi + 0x30], 6
00445fdf  mov        eax, dword ptr [edi + 0x30]
00445fe2  test       eax, eax
00445fe4  je         0x445fff

// block 00445fe6
00445fe6  mov        ecx, dword ptr [0x4ce8b8]
00445fec  cmp        ecx, eax
00445fee  jl         0x445ff3

// block 00445ff0
00445ff0  dec        eax
00445ff1  jmp        0x446004

// block 00445ff3
00445ff3  xor        eax, eax
00445ff5  test       ecx, ecx
00445ff7  setl       al
00445ffa  dec        eax
00445ffb  and        eax, ecx
00445ffd  jmp        0x446004

// block 00445fff
00445fff  mov        eax, dword ptr [0x4ce8b8]

// block 00446004
00446004  push       0
00446006  push       0x76
00446008  mov        dword ptr [edi + 0x28], eax
0044600b  mov        edx, dword ptr [edi + 0x14]
0044600e  lea        ecx, [esp + 0x10]
00446012  push       ecx
00446013  push       edx
00446014  mov        eax, 0x17
00446019  xor        ecx, ecx
0044601b  call       0x4615a0

// block 00446020
00446020  mov        eax, dword ptr [esp + 8]
00446024  push       0
00446026  push       0x78
00446028  mov        dword ptr [edi + 0x4a0], eax
0044602e  mov        edx, dword ptr [edi + 0x14]
00446031  lea        ecx, [esp + 0x10]
00446035  push       ecx
00446036  push       edx
00446037  mov        eax, 0x17
0044603c  xor        ecx, ecx
0044603e  call       0x4615a0

// block 00446043
00446043  mov        eax, dword ptr [esp + 8]
00446047  mov        dword ptr [edi + 0x4a8], eax
0044604d  mov        ecx, 1
00446052  mov        eax, edi
00446054  call       0x43ef40

// block 00446059
00446059  cmp        dword ptr [edi + 0x2b8], 0xa
00446060  jle        0x44639a

// block 00446066
00446066  mov        ecx, 2
0044606b  mov        eax, edi
0044606d  call       0x43ef40

// block 00446072
00446072  pop        esi
00446073  mov        eax, 1
00446078  pop        edi
00446079  pop        ecx
0044607a  ret        

// block 0044607b
0044607b  mov        ecx, dword ptr [edi + 0x28]
0044607e  lea        esi, [edi + 0x28]
00446081  mov        al, 0x10
00446083  mov        dword ptr [esi + 4], ecx
00446086  test       byte ptr [0x4d48c4], al
0044608c  jne        0x446096

// block 0044608e
0044608e  test       byte ptr [0x4d48c0], al
00446094  je         0x44609f

// block 00446096
00446096  push       -1
00446098  mov        eax, esi
0044609a  call       0x464970

// block 0044609f
0044609f  mov        al, 0x20
004460a1  test       byte ptr [0x4d48c4], al
004460a7  jne        0x4460b1

// block 004460a9
004460a9  test       byte ptr [0x4d48c0], al
004460af  je         0x4460ba

// block 004460b1
004460b1  push       1
004460b3  mov        eax, esi
004460b5  call       0x464970

// block 004460ba
004460ba  mov        edx, dword ptr [esi + 4]
004460bd  cmp        edx, dword ptr [esi]
004460bf  je         0x4460cb

// block 004460c1
004460c1  mov        edx, 0xa
004460c6  call       0x453d90

// block 004460cb
004460cb  mov        eax, dword ptr [0x4d48c4]
004460d0  test       eax, 0x102
004460d5  je         0x4460fd

// block 004460d7
004460d7  mov        ecx, 4
004460dc  mov        eax, edi
004460de  call       0x43ef40

// block 004460e3
004460e3  mov        edx, 9
004460e8  call       0x453d90

// block 004460ed
004460ed  mov        eax, dword ptr [esi]
004460ef  pop        esi
004460f0  mov        dword ptr [0x4ce8b8], eax
004460f5  mov        eax, 1
004460fa  pop        edi
004460fb  pop        ecx
004460fc  ret        

// block 004460fd
004460fd  test       eax, 0x80001
00446102  je         0x44639a

// block 00446108
00446108  mov        eax, dword ptr [0x4b0ca8]
0044610d  mov        edx, dword ptr [esi]
0044610f  lea        ecx, [eax + eax*2]
00446112  lea        eax, [edx + ecx*2]
00446115  mov        ecx, dword ptr [0x4b0c94]
0044611b  mov        edx, dword ptr [0x4b0c90]
00446121  lea        ecx, [ecx + edx*2]
00446124  imul       ecx, ecx, 0x45f4
0044612a  add        ecx, dword ptr [0x4b451c]
00446130  cmp        byte ptr [ecx + eax*8 + 0x5b1], 0
00446138  jne        0x446141

// block 0044613a
0044613a  mov        edx, 0x11
0044613f  jmp        0x446152

// block 00446141
00446141  mov        ecx, 3
00446146  mov        eax, edi
00446148  call       0x43ef40

// block 0044614d
0044614d  mov        edx, 7

// block 00446152
00446152  call       0x453d90

// block 00446157
00446157  mov        edx, dword ptr [esi]
00446159  mov        esi, 0x4d4ca0
0044615e  mov        dword ptr [0x4ce8b8], edx
00446164  mov        dword ptr [0x4b0cec], 0
0044616e  call       0x463910

// block 00446173
00446173  test       eax, eax
00446175  mov        al, 0x80
00446177  jne        0x4461dc

// block 00446179
00446179  test       byte ptr [0x4d4cd1], al
0044617f  jne        0x4461e4

// block 00446181
00446181  test       byte ptr [0x4d4cd2], al
00446187  jne        0x4461ff

// block 00446189
00446189  test       byte ptr [0x4d4cd3], al
0044618f  jne        0x44621a

// block 00446195
00446195  test       byte ptr [0x4d4cd4], al
0044619b  jne        0x446235

// block 004461a1
004461a1  test       byte ptr [0x4d4cd5], al
004461a7  jne        0x446250

// block 004461ad
004461ad  test       byte ptr [0x4d4cd6], al
004461b3  jne        0x44626b

// block 004461b9
004461b9  test       byte ptr [0x4d4cd7], al
004461bf  jne        0x446286

// block 004461c5
004461c5  test       byte ptr [0x4d4cd8], al
004461cb  jne        0x4462a1

// block 004461d1
004461d1  test       byte ptr [0x4d4cd9], al
004461d7  jmp        0x4462ba

// block 004461dc
004461dc  test       byte ptr [0x4d4ca2], al
004461e2  je         0x4461f7

// block 004461e4
004461e4  pop        esi
004461e5  mov        dword ptr [0x4b0cec], 1
004461ef  mov        eax, 1
004461f4  pop        edi
004461f5  pop        ecx
004461f6  ret        

// block 004461f7
004461f7  test       byte ptr [0x4d4ca3], al
004461fd  je         0x446212

// block 004461ff
004461ff  pop        esi
00446200  mov        dword ptr [0x4b0cec], 2
0044620a  mov        eax, 1
0044620f  pop        edi
00446210  pop        ecx
00446211  ret        

// block 00446212
00446212  test       byte ptr [0x4d4ca4], al
00446218  je         0x44622d

// block 0044621a
0044621a  pop        esi
0044621b  mov        dword ptr [0x4b0cec], 3
00446225  mov        eax, 1
0044622a  pop        edi
0044622b  pop        ecx
0044622c  ret        

// block 0044622d
0044622d  test       byte ptr [0x4d4ca5], al
00446233  je         0x446248

// block 00446235
00446235  pop        esi
00446236  mov        dword ptr [0x4b0cec], 4
00446240  mov        eax, 1
00446245  pop        edi
00446246  pop        ecx
00446247  ret        

// block 00446248
00446248  test       byte ptr [0x4d4ca6], al
0044624e  je         0x446263

// block 00446250
00446250  pop        esi
00446251  mov        dword ptr [0x4b0cec], 5
0044625b  mov        eax, 1
00446260  pop        edi
00446261  pop        ecx
00446262  ret        

// block 00446263
00446263  test       byte ptr [0x4d4ca7], al
00446269  je         0x44627e

// block 0044626b
0044626b  pop        esi
0044626c  mov        dword ptr [0x4b0cec], 6
00446276  mov        eax, 1
0044627b  pop        edi
0044627c  pop        ecx
0044627d  ret        

// block 0044627e
0044627e  test       byte ptr [0x4d4ca8], al
00446284  je         0x446299

// block 00446286
00446286  pop        esi
00446287  mov        dword ptr [0x4b0cec], 7
00446291  mov        eax, 1
00446296  pop        edi
00446297  pop        ecx
00446298  ret        

// block 00446299
00446299  test       byte ptr [0x4d4ca9], al
0044629f  je         0x4462b4

// block 004462a1
004462a1  pop        esi
004462a2  mov        dword ptr [0x4b0cec], 8
004462ac  mov        eax, 1
004462b1  pop        edi
004462b2  pop        ecx
004462b3  ret        

// block 004462b4
004462b4  test       byte ptr [0x4d4caa], al

// block 004462ba
004462ba  je         0x44639a

// block 004462c0
004462c0  pop        esi
004462c1  mov        dword ptr [0x4b0cec], 9
004462cb  mov        eax, 1
004462d0  pop        edi
004462d1  pop        ecx
004462d2  ret        

// block 004462d3
004462d3  cmp        dword ptr [edi + 0x2b8], 0xa
004462da  jne        0x446308

// block 004462dc
004462dc  fld        dword ptr [0x4a4260]
004462e2  sub        esp, 8
004462e5  fstp       dword ptr [esp + 4]
004462e9  fld        dword ptr [0x4a3ec0]
004462ef  fstp       dword ptr [esp]
004462f2  call       0x411b80

// block 004462f7
004462f7  push       0x3b
004462f9  push       0
004462fb  push       0
004462fd  push       0
004462ff  push       0x20
00446301  push       5
00446303  call       0x4529a0

// block 00446308
00446308  cmp        dword ptr [edi + 0x2b8], 0x28
0044630f  jl         0x44639a

// block 00446315
00446315  lea        esi, [edi + 0x28]
00446318  mov        eax, esi
0044631a  call       0x464900

// block 0044631f
0044631f  mov        edx, 2
00446324  mov        eax, edi
00446326  call       0x43eee0

// block 0044632b
0044632b  fld        dword ptr [0x4a4250]
00446331  mov        eax, dword ptr [esi]
00446333  inc        eax
00446334  mov        ecx, eax
00446336  shl        ecx, 6
00446339  add        ecx, 0x4aebf0
0044633f  push       ecx
00446340  fstp       dword ptr [esp]
00446343  mov        dword ptr [0x4b452c], ecx
00446349  mov        dword ptr [0x4b0cb0], eax
0044634e  mov        dword ptr [0x4b0cb4], eax
00446353  call       0x430270

// block 00446358
00446358  pop        esi
00446359  mov        dword ptr [0x4cee40], 7
00446363  mov        eax, 1
00446368  pop        edi
00446369  pop        ecx
0044636a  ret        

// block 0044636b
0044636b  cmp        dword ptr [edi + 0x2b8], 6
00446372  jl         0x44639a

// block 00446374
00446374  mov        esi, 0x76
00446379  call       0x43efd0

// block 0044637e
0044637e  mov        esi, 0x78
00446383  call       0x43efd0

// block 00446388
00446388  lea        edx, [esi - 0x71]
0044638b  mov        eax, edi
0044638d  call       0x43eee0

// block 00446392
00446392  lea        eax, [edi + 0x28]
00446395  call       0x464940

// block 0044639a
0044639a  pop        esi

// block 0044639b
0044639b  mov        eax, 1
004463a0  pop        edi
004463a1  pop        ecx
004463a2  ret        
