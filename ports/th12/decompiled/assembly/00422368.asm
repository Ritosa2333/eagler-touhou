
// block 00422368
00422368  sub        esp, 8
0042236b  fstp       dword ptr [esp + 4]
0042236f  fld        dword ptr [0x4a3ec0]
00422375  fstp       dword ptr [esp]
00422378  call       0x411b80

// block 0042237d
0042237d  mov        ecx, dword ptr [0x4b0cb4]
00422383  cmp        ecx, dword ptr [0x4b0cb0]
00422389  jne        0x422392

// block 0042238b
0042238b  or         dword ptr [0x4b0ce0], 1

// block 00422392
00422392  mov        eax, dword ptr [0x4cee40]

// block 00422397
00422397  test       byte ptr [0x4b0ce0], bl
0042239d  jne        0x4224c8

// block 004223a3
004223a3  cmp        eax, 0xf
004223a6  je         0x4223c6

// block 004223a8
004223a8  cmp        eax, 0x10
004223ab  je         0x4223c6

// block 004223ad
004223ad  mov        esi, dword ptr [0x4b4518]
004223b3  cmp        esi, ebp
004223b5  je         0x4223c6

// block 004223b7
004223b7  push       esi
004223b8  call       0x43b450

// block 004223bd
004223bd  push       esi
004223be  call       0x46ca4f

// block 004223c3
004223c3  add        esp, 4

// block 004223c6
004223c6  mov        esi, dword ptr [0x4b43c0]
004223cc  cmp        esi, ebp
004223ce  je         0x4223df

// block 004223d0
004223d0  push       esi
004223d1  call       0x402cc0

// block 004223d6
004223d6  push       esi
004223d7  call       0x46ca4f

// block 004223dc
004223dc  add        esp, 4

// block 004223df
004223df  mov        esi, dword ptr [0x4b43bc]
004223e5  cmp        esi, ebp
004223e7  je         0x4223f8

// block 004223e9
004223e9  push       esi
004223ea  call       0x402cc0

// block 004223ef
004223ef  push       esi
004223f0  call       0x46ca4f

// block 004223f5
004223f5  add        esp, 4

// block 004223f8
004223f8  mov        esi, dword ptr [0x4b4510]
004223fe  cmp        esi, ebp
00422400  je         0x422412

// block 00422402
00422402  mov        eax, esi
00422404  call       0x431ed0

// block 00422409
00422409  push       esi
0042240a  call       0x46ca4f

// block 0042240f
0042240f  add        esp, 4

// block 00422412
00422412  mov        esi, dword ptr [0x4b43e4]
00422418  cmp        esi, ebp
0042241a  je         0x42242b

// block 0042241c
0042241c  push       esi
0042241d  call       0x41dc50

// block 00422422
00422422  push       esi
00422423  call       0x46ca4f

// block 00422428
00422428  add        esp, 4

// block 0042242b
0042242b  mov        esi, dword ptr [0x4b4514]
00422431  cmp        esi, ebp
00422433  je         0x422444

// block 00422435
00422435  push       esi
00422436  call       0x436270

// block 0042243b
0042243b  push       esi
0042243c  call       0x46ca4f

// block 00422441
00422441  add        esp, 4

// block 00422444
00422444  mov        esi, dword ptr [0x4b43c8]
0042244a  cmp        esi, ebp
0042244c  je         0x42245d

// block 0042244e
0042244e  push       esi
0042244f  call       0x4096d0

// block 00422454
00422454  push       esi
00422455  call       0x46ca4f

// block 0042245a
0042245a  add        esp, 4

// block 0042245d
0042245d  mov        esi, dword ptr [0x4b44f0]
00422463  cmp        esi, ebp
00422465  je         0x422476

// block 00422467
00422467  push       esi
00422468  call       0x425a00

// block 0042246d
0042246d  push       esi
0042246e  call       0x46ca4f

// block 00422473
00422473  add        esp, 4

// block 00422476
00422476  mov        ebx, dword ptr [0x4b44f4]
0042247c  cmp        ebx, ebp
0042247e  je         0x42248e

// block 00422480
00422480  call       0x4280d0

// block 00422485
00422485  push       ebx
00422486  call       0x46ca4f

// block 0042248b
0042248b  add        esp, 4

// block 0042248e
0042248e  mov        esi, dword ptr [0x4b4524]
00422494  cmp        esi, ebp
00422496  je         0x4224a7

// block 00422498
00422498  push       esi
00422499  call       0x43dc30

// block 0042249e
0042249e  push       esi
0042249f  call       0x46ca4f

// block 004224a4
004224a4  add        esp, 4

// block 004224a7
004224a7  mov        ebx, dword ptr [0x4b4534]
004224ad  cmp        ebx, ebp
004224af  je         0x422572

// block 004224b5
004224b5  call       0x44a120

// block 004224ba
004224ba  push       ebx
004224bb  call       0x46ca4f

// block 004224c0
004224c0  add        esp, 4
004224c3  jmp        0x422572

// block 004224c8
004224c8  call       0x41dbc0

// block 004224cd
004224cd  mov        esi, dword ptr [0x4b43bc]
004224d3  cmp        esi, ebp
004224d5  je         0x4224e6

// block 004224d7
004224d7  push       esi
004224d8  call       0x402cc0

// block 004224dd
004224dd  push       esi
004224de  call       0x46ca4f

// block 004224e3
004224e3  add        esp, 4

// block 004224e6
004224e6  mov        edx, dword ptr [0x4b43c0]
004224ec  mov        eax, dword ptr [0x4b4510]
004224f1  mov        dword ptr [0x4b43bc], edx
004224f7  mov        edx, 0xfffffffd
004224fc  cmp        dword ptr [eax + 8], ebp
004224ff  je         0x422507

// block 00422501
00422501  mov        ecx, dword ptr [eax + 8]
00422504  and        dword ptr [ecx + 4], edx

// block 00422507
00422507  cmp        dword ptr [eax + 0xc], ebp
0042250a  je         0x422512

// block 0042250c
0042250c  mov        eax, dword ptr [eax + 0xc]
0042250f  and        dword ptr [eax + 4], edx

// block 00422512
00422512  mov        eax, dword ptr [0x4b43c8]
00422517  cmp        dword ptr [eax + 8], ebp
0042251a  je         0x422522

// block 0042251c
0042251c  mov        ecx, dword ptr [eax + 8]
0042251f  and        dword ptr [ecx + 4], edx

// block 00422522
00422522  cmp        dword ptr [eax + 0xc], ebp
00422525  je         0x42252d

// block 00422527
00422527  mov        eax, dword ptr [eax + 0xc]
0042252a  and        dword ptr [eax + 4], edx

// block 0042252d
0042252d  mov        eax, dword ptr [0x4b4518]
00422532  cmp        dword ptr [eax + 0x1d4], ebp
00422538  je         0x422543

// block 0042253a
0042253a  mov        ecx, dword ptr [eax + 0x1d4]
00422540  and        dword ptr [ecx + 4], edx

// block 00422543
00422543  cmp        dword ptr [eax + 0xc], ebp
00422546  je         0x42254e

// block 00422548
00422548  mov        eax, dword ptr [eax + 0xc]
0042254b  and        dword ptr [eax + 4], edx

// block 0042254e
0042254e  mov        esi, dword ptr [0x4b44f0]
00422554  push       0x666fc0
00422559  lea        eax, [esi + 0x14]
0042255c  push       ebp
0042255d  push       eax
0042255e  call       0x477420

// block 00422563
00422563  add        esp, 0xc
00422566  mov        dword ptr [esi + 0x666fd8], ebp
0042256c  mov        dword ptr [esi + 0x666fe0], ebp

// block 00422572
00422572  test       byte ptr [0x4b0ce0], 9
00422579  jne        0x422597

// block 0042257b
0042257b  mov        esi, dword ptr [0x4b43dc]
00422581  cmp        esi, ebp
00422583  je         0x4225a3

// block 00422585
00422585  mov        eax, esi
00422587  call       0x412f10

// block 0042258c
0042258c  push       esi
0042258d  call       0x46ca4f

// block 00422592
00422592  add        esp, 4
00422595  jmp        0x4225a3

// block 00422597
00422597  mov        ecx, dword ptr [0x4b43dc]
0042259d  push       ecx
0042259e  call       0x40e940

// block 004225a3
004225a3  mov        esi, dword ptr [0x4b43d4]
004225a9  cmp        esi, ebp
004225ab  je         0x4225bd

// block 004225ad
004225ad  mov        eax, esi
004225af  call       0x40fa30

// block 004225b4
004225b4  push       esi
004225b5  call       0x46ca4f

// block 004225ba
004225ba  add        esp, 4

// block 004225bd
004225bd  mov        esi, dword ptr [0x4b43c4]
004225c3  cmp        esi, ebp
004225c5  je         0x4225d6

// block 004225c7
004225c7  push       esi
004225c8  call       0x406930

// block 004225cd
004225cd  push       esi
004225ce  call       0x46ca4f

// block 004225d3
004225d3  add        esp, 4

// block 004225d6
004225d6  mov        esi, dword ptr [0x4b43cc]
004225dc  cmp        esi, ebp
004225de  je         0x4225f0

// block 004225e0
004225e0  mov        eax, esi
004225e2  call       0x40d9c0

// block 004225e7
004225e7  push       esi
004225e8  call       0x46ca4f

// block 004225ed
004225ed  add        esp, 4

// block 004225f0
004225f0  mov        edx, dword ptr [esp + 0x14]
004225f4  mov        esi, dword ptr [edx + 8]
004225f7  mov        edi, dword ptr [0x4ce89c]
004225fd  mov        ebx, dword ptr [0x49808c]
00422603  cmp        esi, ebp
00422605  mov        ebp, dword ptr [0x498088]
0042260b  je         0x422648

// block 0042260d
0042260d  test       dword ptr [0x4cee78], 0x8000
00422617  je         0x422626

// block 00422619
00422619  push       0x4cf0f8
0042261e  call       ebp

// block 00422620
00422620  inc        byte ptr [0x4cf218]

// block 00422626
00422626  mov        ecx, esi
00422628  mov        edx, edi
0042262a  call       0x462890

// block 0042262f
0042262f  test       dword ptr [0x4cee78], 0x8000
00422639  je         0x422648

// block 0042263b
0042263b  push       0x4cf0f8
00422640  call       ebx

// block 00422642
00422642  dec        byte ptr [0x4cf218]

// block 00422648
00422648  mov        eax, dword ptr [esp + 0x14]
0042264c  mov        esi, dword ptr [eax + 0xc]
0042264f  mov        edi, dword ptr [0x4ce89c]
00422655  test       esi, esi
00422657  je         0x422694

// block 00422659
00422659  test       dword ptr [0x4cee78], 0x8000
00422663  je         0x422672

// block 00422665
00422665  push       0x4cf0f8
0042266a  call       ebp

// block 0042266c
0042266c  inc        byte ptr [0x4cf218]

// block 00422672
00422672  mov        ecx, esi
00422674  mov        edx, edi
00422676  call       0x462890

// block 0042267b
0042267b  test       dword ptr [0x4cee78], 0x8000
00422685  je         0x422694

// block 00422687
00422687  push       0x4cf0f8
0042268c  call       ebx

// block 0042268e
0042268e  dec        byte ptr [0x4cf218]

// block 00422694
00422694  test       byte ptr [0x4b0ce0], 0x22
0042269b  mov        dword ptr [0x4b44e8], 0
004226a5  jne        0x4226c7

// block 004226a7
004226a7  test       byte ptr [0x4ceae8], 0x10
004226ae  push       0
004226b0  mov        edi, 0x4a0a20
004226b5  mov        eax, 0x4cf4e8
004226ba  je         0x4226c0

// block 004226bc
004226bc  push       4
004226be  jmp        0x4226c2

// block 004226c0
004226c0  push       3

// block 004226c2
004226c2  call       0x454960

// block 004226c7
004226c7  call       0x423020

// block 004226cc
004226cc  mov        cl, byte ptr [0x4b0ce0]
004226d2  and        cl, 1
004226d5  movsx      edx, cl
004226d8  neg        edx
004226da  pop        edi
004226db  sbb        edx, edx
004226dd  pop        esi
004226de  and        edx, 0x1000000
004226e4  add        edx, 0xff000000
004226ea  pop        ebp
004226eb  mov        dword ptr [0x4cf2a8], edx
004226f1  mov        dword ptr [0x4ce55c], 1
004226fb  pop        ebx
004226fc  ret        4
