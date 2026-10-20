
// block 00473197
00473197  mov        edi, edi
00473199  push       ebp
0047319a  mov        ebp, esp
0047319c  sub        esp, 0x14
0047319f  push       esi
004731a0  push       edi
004731a1  push       dword ptr [ebp + 8]
004731a4  lea        ecx, [ebp - 0x14]
004731a7  call       0x46d3b4

// block 004731ac
004731ac  mov        eax, dword ptr [ebp + 0x10]
004731af  mov        esi, dword ptr [ebp + 0xc]
004731b2  xor        edi, edi
004731b4  cmp        eax, edi
004731b6  je         0x4731ba

// block 004731b8
004731b8  mov        dword ptr [eax], esi

// block 004731ba
004731ba  cmp        esi, edi
004731bc  jne        0x4731ea

// block 004731be
004731be  call       0x46e96f

// block 004731c3
004731c3  push       edi
004731c4  push       edi
004731c5  push       edi
004731c6  push       edi
004731c7  push       edi
004731c8  mov        dword ptr [eax], 0x16
004731ce  call       0x470f2d

// block 004731d3
004731d3  add        esp, 0x14
004731d6  cmp        byte ptr [ebp - 8], 0
004731da  je         0x4731e3

// block 004731dc
004731dc  mov        eax, dword ptr [ebp - 0xc]
004731df  and        dword ptr [eax + 0x70], 0xfffffffd

// block 004731e3
004731e3  xor        eax, eax
004731e5  jmp        0x4733c2

// block 004731ea
004731ea  cmp        dword ptr [ebp + 0x14], edi
004731ed  je         0x4731fb

// block 004731ef
004731ef  cmp        dword ptr [ebp + 0x14], 2
004731f3  jl         0x4731be

// block 004731f5
004731f5  cmp        dword ptr [ebp + 0x14], 0x24
004731f9  jg         0x4731be

// block 004731fb
004731fb  mov        ecx, dword ptr [ebp - 0x14]
004731fe  push       ebx
004731ff  mov        bl, byte ptr [esi]
00473201  mov        dword ptr [ebp - 4], edi
00473204  lea        edi, [esi + 1]

// block 00473207
00473207  cmp        dword ptr [ecx + 0xac], 1
0047320e  jle        0x473227

// block 00473210
00473210  lea        eax, [ebp - 0x14]
00473213  push       eax
00473214  movzx      eax, bl
00473217  push       8
00473219  push       eax
0047321a  call       0x4758eb

// block 0047321f
0047321f  mov        ecx, dword ptr [ebp - 0x14]
00473222  add        esp, 0xc
00473225  jmp        0x473237

// block 00473227
00473227  mov        edx, dword ptr [ecx + 0xc8]
0047322d  movzx      eax, bl
00473230  movzx      eax, word ptr [edx + eax*2]
00473234  and        eax, 8

// block 00473237
00473237  test       eax, eax
00473239  je         0x473240

// block 0047323b
0047323b  mov        bl, byte ptr [edi]
0047323d  inc        edi
0047323e  jmp        0x473207

// block 00473240
00473240  cmp        bl, 0x2d
00473243  jne        0x47324b

// block 00473245
00473245  or         dword ptr [ebp + 0x18], 2
00473249  jmp        0x473250

// block 0047324b
0047324b  cmp        bl, 0x2b
0047324e  jne        0x473253

// block 00473250
00473250  mov        bl, byte ptr [edi]
00473252  inc        edi

// block 00473253
00473253  mov        eax, dword ptr [ebp + 0x14]
00473256  test       eax, eax
00473258  jl         0x4733a9

// block 0047325e
0047325e  cmp        eax, 1
00473261  je         0x4733a9

// block 00473267
00473267  cmp        eax, 0x24
0047326a  jg         0x4733a9

// block 00473270
00473270  test       eax, eax
00473272  jne        0x47329e

// block 00473274
00473274  cmp        bl, 0x30
00473277  je         0x473282

// block 00473279
00473279  mov        dword ptr [ebp + 0x14], 0xa
00473280  jmp        0x4732b6

// block 00473282
00473282  mov        al, byte ptr [edi]
00473284  cmp        al, 0x78
00473286  je         0x473295

// block 00473288
00473288  cmp        al, 0x58
0047328a  je         0x473295

// block 0047328c
0047328c  mov        dword ptr [ebp + 0x14], 8
00473293  jmp        0x4732b6

// block 00473295
00473295  mov        dword ptr [ebp + 0x14], 0x10
0047329c  jmp        0x4732a8

// block 0047329e
0047329e  cmp        eax, 0x10
004732a1  jne        0x4732b6

// block 004732a3
004732a3  cmp        bl, 0x30
004732a6  jne        0x4732b6

// block 004732a8
004732a8  mov        al, byte ptr [edi]
004732aa  cmp        al, 0x78
004732ac  je         0x4732b2

// block 004732ae
004732ae  cmp        al, 0x58
004732b0  jne        0x4732b6

// block 004732b2
004732b2  inc        edi
004732b3  mov        bl, byte ptr [edi]
004732b5  inc        edi

// block 004732b6
004732b6  mov        esi, dword ptr [ecx + 0xc8]
004732bc  mov        eax, 0xffffffff
004732c1  xor        edx, edx
004732c3  div        dword ptr [ebp + 0x14]

// block 004732c6
004732c6  movzx      ecx, bl
004732c9  movzx      ecx, word ptr [esi + ecx*2]
004732cd  test       cl, 4
004732d0  je         0x4732da

// block 004732d2
004732d2  movsx      ecx, bl
004732d5  sub        ecx, 0x30
004732d8  jmp        0x4732f5

// block 004732da
004732da  test       ecx, 0x103
004732e0  je         0x473313

// block 004732e2
004732e2  mov        cl, bl
004732e4  sub        cl, 0x61
004732e7  cmp        cl, 0x19
004732ea  movsx      ecx, bl
004732ed  ja         0x4732f2

// block 004732ef
004732ef  sub        ecx, 0x20

// block 004732f2
004732f2  add        ecx, -0x37

// block 004732f5
004732f5  cmp        ecx, dword ptr [ebp + 0x14]
004732f8  jae        0x473313

// block 004732fa
004732fa  or         dword ptr [ebp + 0x18], 8
004732fe  cmp        dword ptr [ebp - 4], eax
00473301  jb         0x47332a

// block 00473303
00473303  jne        0x473309

// block 00473305
00473305  cmp        ecx, edx
00473307  jbe        0x47332a

// block 00473309
00473309  or         dword ptr [ebp + 0x18], 4
0047330d  cmp        dword ptr [ebp + 0x10], 0
00473311  jne        0x473336

// block 00473313
00473313  mov        eax, dword ptr [ebp + 0x18]
00473316  dec        edi
00473317  test       al, 8
00473319  jne        0x47333b

// block 0047331b
0047331b  cmp        dword ptr [ebp + 0x10], 0
0047331f  je         0x473324

// block 00473321
00473321  mov        edi, dword ptr [ebp + 0xc]

// block 00473324
00473324  and        dword ptr [ebp - 4], 0
00473328  jmp        0x473385

// block 0047332a
0047332a  mov        ebx, dword ptr [ebp - 4]
0047332d  imul       ebx, dword ptr [ebp + 0x14]
00473331  add        ebx, ecx
00473333  mov        dword ptr [ebp - 4], ebx

// block 00473336
00473336  mov        bl, byte ptr [edi]
00473338  inc        edi
00473339  jmp        0x4732c6

// block 0047333b
0047333b  mov        esi, 0x7fffffff
00473340  test       al, 4
00473342  jne        0x47335f

// block 00473344
00473344  test       al, 1
00473346  jne        0x473385

// block 00473348
00473348  and        eax, 2
0047334b  je         0x473356

// block 0047334d
0047334d  cmp        dword ptr [ebp - 4], 0x80000000
00473354  ja         0x47335f

// block 00473356
00473356  test       eax, eax
00473358  jne        0x473385

// block 0047335a
0047335a  cmp        dword ptr [ebp - 4], esi
0047335d  jbe        0x473385

// block 0047335f
0047335f  call       0x46e96f

// block 00473364
00473364  test       byte ptr [ebp + 0x18], 1
00473368  mov        dword ptr [eax], 0x22
0047336e  je         0x473376

// block 00473370
00473370  or         dword ptr [ebp - 4], 0xffffffff
00473374  jmp        0x473385

// block 00473376
00473376  test       byte ptr [ebp + 0x18], 2
0047337a  push       0
0047337c  pop        eax
0047337d  setne      al
00473380  add        eax, esi
00473382  mov        dword ptr [ebp - 4], eax

// block 00473385
00473385  mov        eax, dword ptr [ebp + 0x10]
00473388  test       eax, eax
0047338a  je         0x47338e

// block 0047338c
0047338c  mov        dword ptr [eax], edi

// block 0047338e
0047338e  test       byte ptr [ebp + 0x18], 2
00473392  je         0x473397

// block 00473394
00473394  neg        dword ptr [ebp - 4]

// block 00473397
00473397  cmp        byte ptr [ebp - 8], 0
0047339b  je         0x4733a4

// block 0047339d
0047339d  mov        eax, dword ptr [ebp - 0xc]
004733a0  and        dword ptr [eax + 0x70], 0xfffffffd

// block 004733a4
004733a4  mov        eax, dword ptr [ebp - 4]
004733a7  jmp        0x4733c1

// block 004733a9
004733a9  mov        eax, dword ptr [ebp + 0x10]
004733ac  test       eax, eax
004733ae  je         0x4733b2

// block 004733b0
004733b0  mov        dword ptr [eax], esi

// block 004733b2
004733b2  cmp        byte ptr [ebp - 8], 0
004733b6  je         0x4733bf

// block 004733b8
004733b8  mov        eax, dword ptr [ebp - 0xc]
004733bb  and        dword ptr [eax + 0x70], 0xfffffffd

// block 004733bf
004733bf  xor        eax, eax

// block 004733c1
004733c1  pop        ebx

// block 004733c2
004733c2  pop        edi
004733c3  pop        esi
004733c4  leave      
004733c5  ret        
