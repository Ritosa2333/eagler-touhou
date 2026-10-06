
// block 004501e0
004501e0  sub        esp, 0xc
004501e3  push       ebx
004501e4  push       edi
004501e5  call       0x4508b0

// block 004501ea
004501ea  fst        qword ptr [esi + 0x40]
004501ed  fld        qword ptr [esi + 0x60]
004501f0  fadd       qword ptr [0x4a3de8]
004501f6  fsubrp     st(1)
004501f8  fmul       qword ptr [0x4a3d30]
004501fe  fsub       qword ptr [0x4a4238]
00450204  call       0x4931e0

// block 00450209
00450209  mov        ecx, dword ptr [esi + 0x78]
0045020c  lea        edx, [ecx + ecx*2]
0045020f  lea        edx, [esi + edx*4 + 0x80]
00450216  mov        dword ptr [esi + 0x74], eax
00450219  cmp        dword ptr [edx], eax
0045021b  jl         0x45022b

// block 0045021d
0045021d  xor        ecx, ecx
0045021f  test       eax, eax
00450221  setl       cl
00450224  dec        ecx
00450225  and        ecx, eax
00450227  mov        dword ptr [edx], ecx
00450229  jmp        0x45025b

// block 0045022b
0045022b  lea        eax, [ecx + ecx*2 + 0x21]
0045022f  inc        dword ptr [esi + eax*4]
00450232  lea        eax, [esi + eax*4]
00450235  mov        eax, dword ptr [esi + 0x78]
00450238  lea        ecx, [eax + eax*2 + 0x21]
0045023c  cmp        dword ptr [esi + ecx*4], 0xf
00450240  jl         0x45026b

// block 00450242
00450242  lea        edx, [eax + eax*2]
00450245  mov        ecx, dword ptr [esi + edx*4 + 0x80]
0045024c  cmp        ecx, dword ptr [esi + edx*4 + 0x7c]
00450250  lea        eax, [esi + edx*4]
00450253  jge        0x45025b

// block 00450255
00450255  inc        dword ptr [eax + 0x80]

// block 0045025b
0045025b  mov        eax, dword ptr [esi + 0x78]
0045025e  add        eax, 0xb
00450261  lea        edx, [eax + eax*2]
00450264  mov        dword ptr [esi + edx*4], 0

// block 0045026b
0045026b  cmp        dword ptr [esi + 0x74], 0
0045026f  jge        0x450278

// block 00450271
00450271  mov        dword ptr [esi + 0x74], 0

// block 00450278
00450278  mov        dword ptr [esp + 0xc], 0
00450280  call       0x4508b0

// block 00450285
00450285  fcom       qword ptr [esi + 0x68]
00450288  fnstsw     ax
0045028a  test       ah, 5
0045028d  jp         0x450292

// block 0045028f
0045028f  fst        qword ptr [esi + 0x68]

// block 00450292
00450292  fsub       qword ptr [esi + 0x68]
00450295  mov        ebx, dword ptr [0x498084]
0045029b  fcomp      qword ptr [0x4a4230]
004502a1  fnstsw     ax
004502a3  test       ah, 5
004502a6  jp         0x45032f

// block 004502ac
004502ac  call       0x4508b0

// block 004502b1
004502b1  fsub       qword ptr [esi + 0x68]
004502b4  fcomp      qword ptr [0x4a4228]
004502ba  fnstsw     ax
004502bc  test       ah, 5
004502bf  jp         0x4502da

// block 004502c1
004502c1  push       1
004502c3  call       ebx

// block 004502c5
004502c5  call       0x4508b0

// block 004502ca
004502ca  fsub       qword ptr [esi + 0x68]
004502cd  fcomp      qword ptr [0x4a4228]
004502d3  fnstsw     ax
004502d5  test       ah, 5
004502d8  jnp        0x4502c1

// block 004502da
004502da  mov        eax, dword ptr [0x4ce8f0]
004502df  mov        edi, dword ptr [esp + 0xc]
004502e3  lea        edx, [esp + 8]
004502e7  push       edx
004502e8  mov        dword ptr [esp + 0xc], 0
004502f0  mov        ecx, dword ptr [eax]
004502f2  push       0
004502f4  push       eax
004502f5  mov        eax, dword ptr [ecx + 0x4c]
004502f8  call       eax

// block 004502fa
004502fa  test       eax, eax
004502fc  jne        0x450348

// block 004502fe
004502fe  mov        edi, edi

// block 00450300
00450300  mov        eax, dword ptr [esp + 0xc]
00450304  cmp        edi, eax
00450306  ja         0x450348

// block 00450308
00450308  test       eax, eax
0045030a  je         0x450348

// block 0045030c
0045030c  cmp        dword ptr [esp + 8], 0
00450311  jne        0x450348

// block 00450313
00450313  lea        edx, [esp + 8]
00450317  mov        edi, eax
00450319  mov        eax, dword ptr [0x4ce8f0]
0045031e  mov        ecx, dword ptr [eax]
00450320  push       edx
00450321  push       0
00450323  push       eax
00450324  mov        eax, dword ptr [ecx + 0x4c]
00450327  call       eax

// block 00450329
00450329  test       eax, eax
0045032b  je         0x450300

// block 0045032d
0045032d  jmp        0x450348

// block 0045032f
0045032f  mov        eax, dword ptr [esi + 0x78]
00450332  lea        ecx, [eax + eax*2]
00450335  cmp        dword ptr [esi + ecx*4 + 0x80], 0
0045033d  lea        eax, [esi + ecx*4 + 0x80]
00450344  jle        0x450348

// block 00450346
00450346  dec        dword ptr [eax]

// block 00450348
00450348  call       0x4508b0

// block 0045034d
0045034d  fstp       qword ptr [esi + 0x68]
00450350  call       0x450810

// block 00450355
00450355  mov        eax, dword ptr [0x4ce8f0]
0045035a  mov        edx, dword ptr [eax]
0045035c  push       0
0045035e  push       0
00450360  push       0
00450362  push       0
00450364  push       eax
00450365  mov        eax, dword ptr [edx + 0x44]
00450368  call       eax

// block 0045036a
0045036a  test       eax, eax
0045036c  jge        0x4503a8

// block 0045036e
0045036e  call       0x431700

// block 00450373
00450373  call       0x44f370

// block 00450378
00450378  mov        eax, dword ptr [0x4ce8f0]
0045037d  mov        ecx, dword ptr [eax]
0045037f  mov        edx, dword ptr [ecx + 0x40]
00450382  push       0x4ce9dc
00450387  push       eax
00450388  call       edx

// block 0045038a
0045038a  call       0x44f400

// block 0045038f
0045038f  mov        eax, 0x4ce8e8
00450394  call       0x431630

// block 00450399
00450399  call       0x451200

// block 0045039e
0045039e  mov        dword ptr [0x4cee5c], 2

// block 004503a8
004503a8  cmp        dword ptr [0x4b43e0], 0
004503af  je         0x4503b6

// block 004503b1
004503b1  call       0x41cb70

// block 004503b6
004503b6  cmp        dword ptr [0x4b43cc], 0
004503bd  je         0x4503c4

// block 004503bf
004503bf  call       0x40dd50

// block 004503c4
004503c4  call       0x4508b0

// block 004503c9
004503c9  fstp       st(0)
004503cb  mov        eax, dword ptr [esi + 0x78]
004503ce  lea        eax, [eax + eax*2]
004503d1  mov        eax, dword ptr [esi + eax*4 + 0x80]
004503d8  test       eax, eax
004503da  jle        0x4503df

// block 004503dc
004503dc  push       eax
004503dd  call       ebx

// block 004503df
004503df  call       0x4508b0

// block 004503e4
004503e4  fstp       qword ptr [esi + 0x60]
004503e7  pop        edi
004503e8  pop        ebx
004503e9  add        esp, 0xc
004503ec  ret        
