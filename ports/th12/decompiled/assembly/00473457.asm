
// block 00473457
00473457  mov        edi, edi
00473459  push       ebp
0047345a  mov        ebp, esp
0047345c  sub        esp, 0x38
0047345f  push       ebx
00473460  push       edi
00473461  push       dword ptr [ebp + 8]
00473464  lea        ecx, [ebp - 0x38]
00473467  call       0x46d3b4

// block 0047346c
0047346c  mov        eax, dword ptr [ebp + 0x10]
0047346f  mov        edi, dword ptr [ebp + 0xc]
00473472  xor        ebx, ebx
00473474  cmp        eax, ebx
00473476  je         0x47347a

// block 00473478
00473478  mov        dword ptr [eax], edi

// block 0047347a
0047347a  cmp        edi, ebx
0047347c  jne        0x4734ab

// block 0047347e
0047347e  call       0x46e96f

// block 00473483
00473483  push       ebx
00473484  push       ebx
00473485  push       ebx
00473486  push       ebx
00473487  push       ebx
00473488  mov        dword ptr [eax], 0x16
0047348e  call       0x470f2d

// block 00473493
00473493  add        esp, 0x14
00473496  cmp        byte ptr [ebp - 0x2c], bl
00473499  je         0x4734a2

// block 0047349b
0047349b  mov        eax, dword ptr [ebp - 0x30]
0047349e  and        dword ptr [eax + 0x70], 0xfffffffd

// block 004734a2
004734a2  xor        eax, eax
004734a4  xor        edx, edx
004734a6  jmp        0x4736ea

// block 004734ab
004734ab  cmp        dword ptr [ebp + 0x14], ebx
004734ae  je         0x4734bc

// block 004734b0
004734b0  cmp        dword ptr [ebp + 0x14], 2
004734b4  jl         0x47347e

// block 004734b6
004734b6  cmp        dword ptr [ebp + 0x14], 0x24
004734ba  jg         0x47347e

// block 004734bc
004734bc  push       esi
004734bd  mov        esi, dword ptr [ebp - 0x38]
004734c0  mov        dword ptr [ebp - 0x10], ebx
004734c3  mov        dword ptr [ebp - 0xc], ebx

// block 004734c6
004734c6  mov        al, byte ptr [edi]
004734c8  inc        edi
004734c9  cmp        dword ptr [esi + 0xac], 1
004734d0  mov        byte ptr [ebp - 1], al
004734d3  jle        0x4734ed

// block 004734d5
004734d5  lea        eax, [ebp - 0x38]
004734d8  push       eax
004734d9  movzx      eax, byte ptr [ebp - 1]
004734dd  push       8
004734df  push       eax
004734e0  call       0x4758eb

// block 004734e5
004734e5  mov        esi, dword ptr [ebp - 0x38]
004734e8  add        esp, 0xc
004734eb  jmp        0x4734fe

// block 004734ed
004734ed  movzx      eax, byte ptr [ebp - 1]
004734f1  mov        ecx, dword ptr [esi + 0xc8]
004734f7  movzx      eax, word ptr [ecx + eax*2]
004734fb  and        eax, 8

// block 004734fe
004734fe  cmp        eax, ebx
00473500  jne        0x4734c6

// block 00473502
00473502  cmp        byte ptr [ebp - 1], 0x2d
00473506  mov        dword ptr [ebp - 8], edi
00473509  jne        0x473511

// block 0047350b
0047350b  or         dword ptr [ebp + 0x18], 2
0047350f  jmp        0x473517

// block 00473511
00473511  cmp        byte ptr [ebp - 1], 0x2b
00473515  jne        0x473520

// block 00473517
00473517  mov        al, byte ptr [edi]
00473519  inc        edi
0047351a  mov        dword ptr [ebp - 8], edi
0047351d  mov        byte ptr [ebp - 1], al

// block 00473520
00473520  push       0x10
00473522  pop        ecx
00473523  cmp        dword ptr [ebp + 0x14], ebx
00473526  jne        0x47354d

// block 00473528
00473528  cmp        byte ptr [ebp - 1], 0x30
0047352c  je         0x473537

// block 0047352e
0047352e  mov        dword ptr [ebp + 0x14], 0xa
00473535  jmp        0x47356c

// block 00473537
00473537  mov        al, byte ptr [edi]
00473539  cmp        al, 0x78
0047353b  je         0x47354a

// block 0047353d
0047353d  cmp        al, 0x58
0047353f  je         0x47354a

// block 00473541
00473541  mov        dword ptr [ebp + 0x14], 8
00473548  jmp        0x47356c

// block 0047354a
0047354a  mov        dword ptr [ebp + 0x14], ecx

// block 0047354d
0047354d  cmp        dword ptr [ebp + 0x14], ecx
00473550  jne        0x47356c

// block 00473552
00473552  cmp        byte ptr [ebp - 1], 0x30
00473556  jne        0x47356c

// block 00473558
00473558  mov        al, byte ptr [edi]
0047355a  cmp        al, 0x78
0047355c  je         0x473562

// block 0047355e
0047355e  cmp        al, 0x58
00473560  jne        0x47356c

// block 00473562
00473562  inc        edi
00473563  mov        al, byte ptr [edi]
00473565  inc        edi
00473566  mov        byte ptr [ebp - 1], al
00473569  mov        dword ptr [ebp - 8], edi

// block 0047356c
0047356c  mov        eax, dword ptr [ebp + 0x14]
0047356f  cdq        
00473570  push       edx
00473571  mov        edi, eax
00473573  push       edi
00473574  push       -1
00473576  push       -1
00473578  mov        dword ptr [ebp - 0x24], edx
0047357b  call       0x47c890

// block 00473580
00473580  mov        dword ptr [ebp - 0x1c], ebx
00473583  mov        ebx, dword ptr [esi + 0xc8]
00473589  mov        dword ptr [ebp - 0x20], ecx
0047358c  mov        dword ptr [ebp - 0x18], eax
0047358f  mov        dword ptr [ebp - 0x14], edx

// block 00473592
00473592  mov        cl, byte ptr [ebp - 1]
00473595  movzx      eax, cl
00473598  movzx      eax, word ptr [ebx + eax*2]
0047359c  test       al, 4
0047359e  je         0x4735a8

// block 004735a0
004735a0  movsx      esi, cl
004735a3  sub        esi, 0x30
004735a6  jmp        0x4735c0

// block 004735a8
004735a8  test       eax, 0x103
004735ad  je         0x473600

// block 004735af
004735af  mov        al, cl
004735b1  sub        al, 0x61
004735b3  cmp        al, 0x19
004735b5  movsx      eax, cl
004735b8  ja         0x4735bd

// block 004735ba
004735ba  sub        eax, 0x20

// block 004735bd
004735bd  lea        esi, [eax - 0x37]

// block 004735c0
004735c0  cmp        esi, dword ptr [ebp + 0x14]
004735c3  jae        0x473600

// block 004735c5
004735c5  mov        ecx, dword ptr [ebp - 0xc]
004735c8  or         dword ptr [ebp + 0x18], 8
004735cc  cmp        ecx, dword ptr [ebp - 0x14]
004735cf  jb         0x473622

// block 004735d1
004735d1  ja         0x4735db

// block 004735d3
004735d3  mov        eax, dword ptr [ebp - 0x10]
004735d6  cmp        eax, dword ptr [ebp - 0x18]
004735d9  jb         0x473622

// block 004735db
004735db  mov        eax, dword ptr [ebp - 0x18]
004735de  cmp        dword ptr [ebp - 0x10], eax
004735e1  jne        0x4735f6

// block 004735e3
004735e3  cmp        ecx, dword ptr [ebp - 0x14]
004735e6  jne        0x4735f6

// block 004735e8
004735e8  xor        eax, eax
004735ea  cmp        eax, dword ptr [ebp - 0x1c]
004735ed  jb         0x473622

// block 004735ef
004735ef  ja         0x4735f6

// block 004735f1
004735f1  cmp        esi, dword ptr [ebp - 0x20]
004735f4  jbe        0x473622

// block 004735f6
004735f6  or         dword ptr [ebp + 0x18], 4
004735fa  cmp        dword ptr [ebp + 0x10], 0
004735fe  jne        0x47363b

// block 00473600
00473600  mov        eax, dword ptr [ebp + 0x18]
00473603  dec        dword ptr [ebp - 8]
00473606  test       al, 8
00473608  jne        0x47364b

// block 0047360a
0047360a  xor        eax, eax
0047360c  cmp        dword ptr [ebp + 0x10], eax
0047360f  je         0x473617

// block 00473611
00473611  mov        ecx, dword ptr [ebp + 0xc]
00473614  mov        dword ptr [ebp - 8], ecx

// block 00473617
00473617  mov        dword ptr [ebp - 0x10], eax
0047361a  mov        dword ptr [ebp - 0xc], eax
0047361d  jmp        0x4736b1

// block 00473622
00473622  push       ecx
00473623  push       dword ptr [ebp - 0x10]
00473626  push       dword ptr [ebp - 0x24]
00473629  push       edi
0047362a  call       0x483220

// block 0047362f
0047362f  xor        ecx, ecx
00473631  add        eax, esi
00473633  adc        edx, ecx
00473635  mov        dword ptr [ebp - 0x10], eax
00473638  mov        dword ptr [ebp - 0xc], edx

// block 0047363b
0047363b  mov        eax, dword ptr [ebp - 8]
0047363e  mov        al, byte ptr [eax]
00473640  inc        dword ptr [ebp - 8]
00473643  mov        byte ptr [ebp - 1], al
00473646  jmp        0x473592

// block 0047364b
0047364b  mov        esi, 0x7fffffff
00473650  mov        ebx, 0x80000000
00473655  test       al, 4
00473657  jne        0x473680

// block 00473659
00473659  test       al, 1
0047365b  jne        0x4736b1

// block 0047365d
0047365d  and        eax, 2
00473660  je         0x47366f

// block 00473662
00473662  cmp        dword ptr [ebp - 0xc], ebx
00473665  ja         0x473680

// block 00473667
00473667  jb         0x47366f

// block 00473669
00473669  cmp        dword ptr [ebp - 0x10], 0
0047366d  ja         0x473680

// block 0047366f
0047366f  test       eax, eax
00473671  jne        0x4736b1

// block 00473673
00473673  cmp        dword ptr [ebp - 0xc], esi
00473676  jb         0x4736b1

// block 00473678
00473678  ja         0x473680

// block 0047367a
0047367a  cmp        dword ptr [ebp - 0x10], -1
0047367e  jbe        0x4736b1

// block 00473680
00473680  call       0x46e96f

// block 00473685
00473685  test       byte ptr [ebp + 0x18], 1
00473689  mov        dword ptr [eax], 0x22
0047368f  je         0x47369b

// block 00473691
00473691  or         dword ptr [ebp - 0x10], 0xffffffff
00473695  or         dword ptr [ebp - 0xc], 0xffffffff
00473699  jmp        0x4736b1

// block 0047369b
0047369b  test       byte ptr [ebp + 0x18], 2
0047369f  je         0x4736aa

// block 004736a1
004736a1  and        dword ptr [ebp - 0x10], 0
004736a5  mov        dword ptr [ebp - 0xc], ebx
004736a8  jmp        0x4736b1

// block 004736aa
004736aa  or         dword ptr [ebp - 0x10], 0xffffffff
004736ae  mov        dword ptr [ebp - 0xc], esi

// block 004736b1
004736b1  mov        eax, dword ptr [ebp + 0x10]
004736b4  pop        esi
004736b5  test       eax, eax
004736b7  je         0x4736be

// block 004736b9
004736b9  mov        ecx, dword ptr [ebp - 8]
004736bc  mov        dword ptr [eax], ecx

// block 004736be
004736be  test       byte ptr [ebp + 0x18], 2
004736c2  je         0x4736d7

// block 004736c4
004736c4  mov        eax, dword ptr [ebp - 0x10]
004736c7  mov        ecx, dword ptr [ebp - 0xc]
004736ca  neg        eax
004736cc  adc        ecx, 0
004736cf  neg        ecx
004736d1  mov        dword ptr [ebp - 0x10], eax
004736d4  mov        dword ptr [ebp - 0xc], ecx

// block 004736d7
004736d7  cmp        byte ptr [ebp - 0x2c], 0
004736db  je         0x4736e4

// block 004736dd
004736dd  mov        eax, dword ptr [ebp - 0x30]
004736e0  and        dword ptr [eax + 0x70], 0xfffffffd

// block 004736e4
004736e4  mov        eax, dword ptr [ebp - 0x10]
004736e7  mov        edx, dword ptr [ebp - 0xc]

// block 004736ea
004736ea  pop        edi
004736eb  pop        ebx
004736ec  leave      
004736ed  ret        
