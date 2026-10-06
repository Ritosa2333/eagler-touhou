
// block 004901bb
004901bb  mov        edi, edi
004901bd  push       ebp
004901be  mov        ebp, esp
004901c0  sub        esp, 0x14
004901c3  push       ebx
004901c4  push       esi
004901c5  push       edi
004901c6  wait       
004901c7  fnstcw     word ptr [ebp - 8]
004901ca  mov        ebx, dword ptr [ebp - 8]
004901cd  xor        edx, edx
004901cf  test       bl, 1
004901d2  je         0x4901d7

// block 004901d4
004901d4  push       0x10
004901d6  pop        edx

// block 004901d7
004901d7  test       bl, 4
004901da  je         0x4901df

// block 004901dc
004901dc  or         edx, 8

// block 004901df
004901df  test       bl, 8
004901e2  je         0x4901e7

// block 004901e4
004901e4  or         edx, 4

// block 004901e7
004901e7  test       bl, 0x10
004901ea  je         0x4901ef

// block 004901ec
004901ec  or         edx, 2

// block 004901ef
004901ef  test       bl, 0x20
004901f2  je         0x4901f7

// block 004901f4
004901f4  or         edx, 1

// block 004901f7
004901f7  test       bl, 2
004901fa  je         0x490202

// block 004901fc
004901fc  or         edx, 0x80000

// block 00490202
00490202  movzx      ecx, bx
00490205  mov        eax, ecx
00490207  mov        esi, 0xc00
0049020c  and        eax, esi
0049020e  mov        edi, 0x300
00490213  je         0x490239

// block 00490215
00490215  cmp        eax, 0x400
0049021a  je         0x490233

// block 0049021c
0049021c  cmp        eax, 0x800
00490221  je         0x49022b

// block 00490223
00490223  cmp        eax, esi
00490225  jne        0x490239

// block 00490227
00490227  or         edx, edi
00490229  jmp        0x490239

// block 0049022b
0049022b  or         edx, 0x200
00490231  jmp        0x490239

// block 00490233
00490233  or         edx, 0x100

// block 00490239
00490239  and        ecx, edi
0049023b  je         0x49024d

// block 0049023d
0049023d  cmp        ecx, 0x200
00490243  jne        0x490253

// block 00490245
00490245  or         edx, 0x10000
0049024b  jmp        0x490253

// block 0049024d
0049024d  or         edx, 0x20000

// block 00490253
00490253  test       ebx, 0x1000
00490259  je         0x490261

// block 0049025b
0049025b  or         edx, 0x40000

// block 00490261
00490261  mov        edi, dword ptr [ebp + 0xc]
00490264  mov        ecx, dword ptr [ebp + 8]
00490267  mov        eax, edi
00490269  not        eax
0049026b  and        eax, edx
0049026d  and        ecx, edi
0049026f  or         eax, ecx
00490271  mov        dword ptr [ebp + 0xc], eax
00490274  cmp        eax, edx
00490276  je         0x49032a

// block 0049027c
0049027c  mov        ebx, eax
0049027e  call       0x48f849

// block 00490283
00490283  movzx      eax, ax
00490286  mov        dword ptr [ebp - 4], eax
00490289  fldcw      word ptr [ebp - 4]
0049028c  wait       
0049028d  fnstcw     word ptr [ebp - 4]
00490290  mov        ebx, dword ptr [ebp - 4]
00490293  xor        edx, edx
00490295  test       bl, 1
00490298  je         0x49029d

// block 0049029a
0049029a  push       0x10
0049029c  pop        edx

// block 0049029d
0049029d  test       bl, 4
004902a0  je         0x4902a5

// block 004902a2
004902a2  or         edx, 8

// block 004902a5
004902a5  test       bl, 8
004902a8  je         0x4902ad

// block 004902aa
004902aa  or         edx, 4

// block 004902ad
004902ad  test       bl, 0x10
004902b0  je         0x4902b5

// block 004902b2
004902b2  or         edx, 2

// block 004902b5
004902b5  test       bl, 0x20
004902b8  je         0x4902bd

// block 004902ba
004902ba  or         edx, 1

// block 004902bd
004902bd  test       bl, 2
004902c0  je         0x4902c8

// block 004902c2
004902c2  or         edx, 0x80000

// block 004902c8
004902c8  movzx      ecx, bx
004902cb  mov        eax, ecx
004902cd  and        eax, esi
004902cf  je         0x4902f9

// block 004902d1
004902d1  cmp        eax, 0x400
004902d6  je         0x4902f3

// block 004902d8
004902d8  cmp        eax, 0x800
004902dd  je         0x4902eb

// block 004902df
004902df  cmp        eax, esi
004902e1  jne        0x4902f9

// block 004902e3
004902e3  or         edx, 0x300
004902e9  jmp        0x4902f9

// block 004902eb
004902eb  or         edx, 0x200
004902f1  jmp        0x4902f9

// block 004902f3
004902f3  or         edx, 0x100

// block 004902f9
004902f9  and        ecx, 0x300
004902ff  je         0x490311

// block 00490301
00490301  cmp        ecx, 0x200
00490307  jne        0x490317

// block 00490309
00490309  or         edx, 0x10000
0049030f  jmp        0x490317

// block 00490311
00490311  or         edx, 0x20000

// block 00490317
00490317  test       ebx, 0x1000
0049031d  je         0x490325

// block 0049031f
0049031f  or         edx, 0x40000

// block 00490325
00490325  mov        dword ptr [ebp + 0xc], edx
00490328  mov        eax, edx

// block 0049032a
0049032a  xor        esi, esi
0049032c  cmp        dword ptr [0x4d52dc], esi
00490332  je         0x4904c5

// block 00490338
00490338  and        edi, 0x308031f
0049033e  mov        dword ptr [ebp - 0x14], edi
00490341  stmxcsr    dword ptr [ebp - 0x10]
00490345  mov        eax, dword ptr [ebp - 0x10]
00490348  test       al, al
0049034a  jns        0x49034f

// block 0049034c
0049034c  push       0x10
0049034e  pop        esi

// block 0049034f
0049034f  test       eax, 0x200
00490354  je         0x490359

// block 00490356
00490356  or         esi, 8

// block 00490359
00490359  test       eax, 0x400
0049035e  je         0x490363

// block 00490360
00490360  or         esi, 4

// block 00490363
00490363  test       eax, 0x800
00490368  je         0x49036d

// block 0049036a
0049036a  or         esi, 2

// block 0049036d
0049036d  test       eax, 0x1000
00490372  je         0x490377

// block 00490374
00490374  or         esi, 1

// block 00490377
00490377  test       eax, 0x100
0049037c  je         0x490384

// block 0049037e
0049037e  or         esi, 0x80000

// block 00490384
00490384  mov        ecx, eax
00490386  mov        ebx, 0x6000
0049038b  and        ecx, ebx
0049038d  je         0x4903b9

// block 0049038f
0049038f  cmp        ecx, 0x2000
00490395  je         0x4903b3

// block 00490397
00490397  cmp        ecx, 0x4000
0049039d  je         0x4903ab

// block 0049039f
0049039f  cmp        ecx, ebx
004903a1  jne        0x4903b9

// block 004903a3
004903a3  or         esi, 0x300
004903a9  jmp        0x4903b9

// block 004903ab
004903ab  or         esi, 0x200
004903b1  jmp        0x4903b9

// block 004903b3
004903b3  or         esi, 0x100

// block 004903b9
004903b9  mov        edi, 0x8040
004903be  and        eax, edi
004903c0  sub        eax, 0x40
004903c3  je         0x4903e1

// block 004903c5
004903c5  sub        eax, 0x7fc0
004903ca  je         0x4903d9

// block 004903cc
004903cc  sub        eax, 0x40
004903cf  jne        0x4903e7

// block 004903d1
004903d1  or         esi, 0x1000000
004903d7  jmp        0x4903e7

// block 004903d9
004903d9  or         esi, 0x3000000
004903df  jmp        0x4903e7

// block 004903e1
004903e1  or         esi, 0x2000000

// block 004903e7
004903e7  mov        eax, dword ptr [ebp - 0x14]
004903ea  mov        edx, eax
004903ec  and        eax, dword ptr [ebp + 8]
004903ef  not        edx
004903f1  and        edx, esi
004903f3  or         edx, eax
004903f5  cmp        edx, esi
004903f7  jne        0x490400

// block 004903f9
004903f9  mov        eax, esi
004903fb  jmp        0x4904b0

// block 00490400
00490400  call       0x48f9ba

// block 00490405
00490405  push       eax
00490406  mov        dword ptr [ebp - 0xc], eax
00490409  call       0x491220

// block 0049040e
0049040e  pop        ecx
0049040f  stmxcsr    dword ptr [ebp - 0xc]
00490413  mov        ecx, dword ptr [ebp - 0xc]
00490416  xor        edx, edx
00490418  test       cl, cl
0049041a  jns        0x49041f

// block 0049041c
0049041c  push       0x10
0049041e  pop        edx

// block 0049041f
0049041f  test       ecx, 0x200
00490425  je         0x49042a

// block 00490427
00490427  or         edx, 8

// block 0049042a
0049042a  test       ecx, 0x400
00490430  je         0x490435

// block 00490432
00490432  or         edx, 4

// block 00490435
00490435  test       ecx, 0x800
0049043b  je         0x490440

// block 0049043d
0049043d  or         edx, 2

// block 00490440
00490440  test       ecx, 0x1000
00490446  je         0x49044b

// block 00490448
00490448  or         edx, 1

// block 0049044b
0049044b  mov        esi, 0x100
00490450  test       esi, ecx
00490452  je         0x49045a

// block 00490454
00490454  or         edx, 0x80000

// block 0049045a
0049045a  mov        eax, ecx
0049045c  and        eax, ebx
0049045e  je         0x490484

// block 00490460
00490460  cmp        eax, 0x2000
00490465  je         0x490482

// block 00490467
00490467  cmp        eax, 0x4000
0049046c  je         0x49047a

// block 0049046e
0049046e  cmp        eax, ebx
00490470  jne        0x490484

// block 00490472
00490472  or         edx, 0x300
00490478  jmp        0x490484

// block 0049047a
0049047a  or         edx, 0x200
00490480  jmp        0x490484

// block 00490482
00490482  or         edx, esi

// block 00490484
00490484  and        ecx, edi
00490486  sub        ecx, 0x40
00490489  je         0x4904a8

// block 0049048b
0049048b  sub        ecx, 0x7fc0
00490491  je         0x4904a0

// block 00490493
00490493  sub        ecx, 0x40
00490496  jne        0x4904ae

// block 00490498
00490498  or         edx, 0x1000000
0049049e  jmp        0x4904ae

// block 004904a0
004904a0  or         edx, 0x3000000
004904a6  jmp        0x4904ae

// block 004904a8
004904a8  or         edx, 0x2000000

// block 004904ae
004904ae  mov        eax, edx

// block 004904b0
004904b0  mov        ecx, eax
004904b2  xor        ecx, dword ptr [ebp + 0xc]
004904b5  or         eax, dword ptr [ebp + 0xc]
004904b8  test       ecx, 0x8031f
004904be  je         0x4904c5

// block 004904c0
004904c0  or         eax, 0x80000000

// block 004904c5
004904c5  pop        edi
004904c6  pop        esi
004904c7  pop        ebx
004904c8  leave      
004904c9  ret        
