
// block 004411d0
004411d0  push       ecx
004411d1  mov        eax, dword ptr [esi + 0x24]
004411d4  push       ebx
004411d5  cmp        eax, 4
004411d8  ja         0x44150f

// block 004411de
004411de  push       edi
004411df  jmp        dword ptr [eax*4 + 0x441518]

// block 004411e6
004411e6  mov        dword ptr [esi + 0x30], 5
004411ed  mov        eax, dword ptr [esi + 0x30]
004411f0  test       eax, eax
004411f2  je         0x441203

// block 004411f4
004411f4  jg         0x4411fc

// block 004411f6
004411f6  dec        eax
004411f7  mov        dword ptr [esi + 0x28], eax
004411fa  jmp        0x44120a

// block 004411fc
004411fc  xor        eax, eax
004411fe  mov        dword ptr [esi + 0x28], eax
00441201  jmp        0x44120a

// block 00441203
00441203  mov        dword ptr [esi + 0x28], 0

// block 0044120a
0044120a  mov        ecx, dword ptr [esi + 0x14]
0044120d  push       0
0044120f  push       1
00441211  lea        eax, [esp + 0x10]
00441215  push       eax
00441216  push       ecx
00441217  mov        eax, 0x17
0044121c  xor        ecx, ecx
0044121e  call       0x4615a0

// block 00441223
00441223  mov        edx, dword ptr [esp + 8]
00441227  mov        ebx, esi
00441229  mov        dword ptr [esi + 0x2cc], edx
0044122f  call       0x4426b0

// block 00441234
00441234  mov        ecx, 1
00441239  mov        eax, esi
0044123b  call       0x43ef40

// block 00441240
00441240  cmp        dword ptr [esi + 0x2b8], 6
00441247  jle        0x44150e

// block 0044124d
0044124d  mov        ecx, 2
00441252  mov        eax, esi
00441254  call       0x43ef40

// block 00441259
00441259  mov        eax, dword ptr [esi + 0x2cc]
0044125f  push       eax
00441260  mov        ebx, 3
00441265  call       0x4619e0

// block 0044126a
0044126a  movzx      ebx, word ptr [esi + 0x28]
0044126e  add        bx, 0x11
00441272  jmp        0x441348

// block 00441277
00441277  mov        edx, dword ptr [esi + 0x28]
0044127a  lea        edi, [esi + 0x28]
0044127d  mov        al, 0x10
0044127f  mov        dword ptr [edi + 4], edx
00441282  test       byte ptr [0x4d48c4], al
00441288  jne        0x441292

// block 0044128a
0044128a  test       byte ptr [0x4d48c0], al
00441290  je         0x44129b

// block 00441292
00441292  push       -1
00441294  mov        eax, edi
00441296  call       0x464970

// block 0044129b
0044129b  mov        al, 0x20
0044129d  test       byte ptr [0x4d48c4], al
004412a3  jne        0x4412ad

// block 004412a5
004412a5  test       byte ptr [0x4d48c0], al
004412ab  je         0x4412b6

// block 004412ad
004412ad  push       1
004412af  mov        eax, edi
004412b1  call       0x464970

// block 004412b6
004412b6  mov        eax, dword ptr [edi + 4]
004412b9  cmp        eax, dword ptr [edi]
004412bb  je         0x4412f1

// block 004412bd
004412bd  mov        edx, 0xa
004412c2  call       0x453d90

// block 004412c7
004412c7  mov        ecx, dword ptr [esi + 0x2cc]
004412cd  push       ecx
004412ce  mov        ebx, 3
004412d3  call       0x4619e0

// block 004412d8
004412d8  movzx      ebx, word ptr [edi]
004412db  mov        edx, dword ptr [esi + 0x2cc]
004412e1  add        bx, 7
004412e5  push       edx
004412e6  call       0x461970

// block 004412eb
004412eb  push       esi
004412ec  call       0x441530

// block 004412f1
004412f1  test       dword ptr [0x4d48c4], 0x102
004412fb  je         0x441363

// block 004412fd
004412fd  cmp        dword ptr [edi], 4
00441300  je         0x441455

// block 00441306
00441306  mov        edx, 9
0044130b  call       0x453d90

// block 00441310
00441310  mov        eax, dword ptr [edi + 8]
00441313  test       eax, eax
00441315  je         0x44132a

// block 00441317
00441317  cmp        eax, 4
0044131a  jg         0x441321

// block 0044131c
0044131c  dec        eax
0044131d  mov        dword ptr [edi], eax
0044131f  jmp        0x441330

// block 00441321
00441321  mov        eax, 4
00441326  mov        dword ptr [edi], eax
00441328  jmp        0x441330

// block 0044132a
0044132a  mov        dword ptr [edi], 4

// block 00441330
00441330  mov        eax, dword ptr [esi + 0x2cc]
00441336  push       eax
00441337  mov        ebx, 3
0044133c  call       0x4619e0

// block 00441341
00441341  movzx      ebx, word ptr [edi]
00441344  add        bx, 7

// block 00441348
00441348  mov        ecx, dword ptr [esi + 0x2cc]
0044134e  push       ecx
0044134f  call       0x461970

// block 00441354
00441354  push       esi
00441355  call       0x441530

// block 0044135a
0044135a  pop        edi
0044135b  mov        eax, 1
00441360  pop        ebx
00441361  pop        ecx
00441362  ret        

// block 00441363
00441363  cmp        dword ptr [edi], 1
00441366  jne        0x441383

// block 00441368
00441368  push       0x3c
0044136a  lea        ecx, [esi + 0x2b4]
00441370  call       0x407700

// block 00441375
00441375  test       eax, eax
00441377  je         0x441383

// block 00441379
00441379  mov        edx, 2
0044137e  call       0x453d90

// block 00441383
00441383  mov        al, 0x40
00441385  test       byte ptr [0x4d48c4], al
0044138b  jne        0x441395

// block 0044138d
0044138d  test       byte ptr [0x4d48c0], al
00441393  je         0x4413db

// block 00441395
00441395  mov        eax, dword ptr [edi]
00441397  xor        ecx, ecx
00441399  sub        eax, ecx
0044139b  je         0x4413bc

// block 0044139d
0044139d  sub        eax, 1
004413a0  jne        0x4413db

// block 004413a2
004413a2  cmp        byte ptr [0x4cead1], 5
004413a9  jge        0x4413b3

// block 004413ab
004413ab  mov        byte ptr [0x4cead1], cl
004413b1  jmp        0x4413d4

// block 004413b3
004413b3  add        byte ptr [0x4cead1], 0xfb
004413ba  jmp        0x4413d4

// block 004413bc
004413bc  cmp        byte ptr [0x4cead0], 5
004413c3  jge        0x4413cd

// block 004413c5
004413c5  mov        byte ptr [0x4cead0], cl
004413cb  jmp        0x4413d4

// block 004413cd
004413cd  add        byte ptr [0x4cead0], 0xfb

// block 004413d4
004413d4  mov        ebx, esi
004413d6  call       0x4426b0

// block 004413db
004413db  mov        al, 0x80
004413dd  test       byte ptr [0x4d48c4], al
004413e3  jne        0x4413ed

// block 004413e5
004413e5  test       byte ptr [0x4d48c0], al
004413eb  je         0x441430

// block 004413ed
004413ed  mov        eax, dword ptr [edi]
004413ef  sub        eax, 0
004413f2  je         0x441412

// block 004413f4
004413f4  sub        eax, 1
004413f7  jne        0x441430

// block 004413f9
004413f9  mov        al, byte ptr [0x4cead1]
004413fe  add        al, 5
00441400  cmp        al, 0x64
00441402  mov        byte ptr [0x4cead1], al
00441407  jle        0x441429

// block 00441409
00441409  mov        byte ptr [0x4cead1], 0x64
00441410  jmp        0x441429

// block 00441412
00441412  mov        al, byte ptr [0x4cead0]
00441417  add        al, 5
00441419  cmp        al, 0x64
0044141b  mov        byte ptr [0x4cead0], al
00441420  jle        0x441429

// block 00441422
00441422  mov        byte ptr [0x4cead0], 0x64

// block 00441429
00441429  mov        ebx, esi
0044142b  call       0x4426b0

// block 00441430
00441430  test       dword ptr [0x4d48c4], 0x80001
0044143a  je         0x44150e

// block 00441440
00441440  mov        edi, dword ptr [edi]
00441442  sub        edi, 2
00441445  je         0x44149a

// block 00441447
00441447  sub        edi, 1
0044144a  je         0x44146b

// block 0044144c
0044144c  sub        edi, 1
0044144f  jne        0x44150e

// block 00441455
00441455  mov        edx, dword ptr [esi + 0x2cc]
0044145b  push       edx
0044145c  mov        ebx, 6
00441461  call       0x461970

// block 00441466
00441466  lea        edx, [ebx + 3]
00441469  jmp        0x4414ae

// block 0044146b
0044146b  mov        ebx, esi
0044146d  mov        byte ptr [0x4cead0], 0x64
00441474  mov        byte ptr [0x4cead1], 0x50
0044147b  mov        byte ptr [0x4cead2], 0
00441482  call       0x4426b0

// block 00441487
00441487  mov        edx, 7
0044148c  call       0x453d90

// block 00441491
00441491  pop        edi
00441492  mov        eax, 1
00441497  pop        ebx
00441498  pop        ecx
00441499  ret        

// block 0044149a
0044149a  mov        eax, dword ptr [esi + 0x2cc]
004414a0  push       eax
004414a1  mov        ebx, 6
004414a6  call       0x461970

// block 004414ab
004414ab  lea        edx, [ebx + 1]

// block 004414ae
004414ae  call       0x453d90

// block 004414b3
004414b3  mov        ecx, 4
004414b8  mov        eax, esi
004414ba  call       0x43ef40

// block 004414bf
004414bf  pop        edi
004414c0  mov        eax, 1
004414c5  pop        ebx
004414c6  pop        ecx
004414c7  ret        

// block 004414c8
004414c8  cmp        dword ptr [esi + 0x2b8], 0xa
004414cf  jl         0x44150e

// block 004414d1
004414d1  mov        eax, dword ptr [esi + 0x28]
004414d4  sub        eax, 2
004414d7  lea        edi, [esi + 0x28]
004414da  je         0x4414fb

// block 004414dc
004414dc  sub        eax, 2
004414df  jne        0x44150e

// block 004414e1
004414e1  lea        edx, [eax + 1]
004414e4  mov        eax, esi
004414e6  call       0x43eee0

// block 004414eb
004414eb  mov        eax, edi
004414ed  call       0x464940

// block 004414f2
004414f2  pop        edi
004414f3  mov        eax, 1
004414f8  pop        ebx
004414f9  pop        ecx
004414fa  ret        

// block 004414fb
004414fb  mov        edx, 4
00441500  mov        eax, esi
00441502  call       0x43eee0

// block 00441507
00441507  mov        eax, edi
00441509  call       0x464900

// block 0044150e
0044150e  pop        edi

// block 0044150f
0044150f  mov        eax, 1
00441514  pop        ebx
00441515  pop        ecx
00441516  ret        
