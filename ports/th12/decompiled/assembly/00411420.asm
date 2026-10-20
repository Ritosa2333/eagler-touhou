
// block 00411420
00411420  sub        esp, 0x14
00411423  push       ebp
00411424  mov        ebp, dword ptr [esp + 0x1c]
00411428  test       byte ptr [ebp + 0x74], 4
0041142c  je         0x411437

// block 0041142e
0041142e  xor        eax, eax
00411430  pop        ebp
00411431  add        esp, 0x14
00411434  ret        4

// block 00411437
00411437  mov        eax, dword ptr [ebp + 0x54]
0041143a  movzx      ecx, word ptr [eax]
0041143d  cmp        dword ptr [ebp + 0x1c], ecx
00411440  push       ebx
00411441  push       esi
00411442  push       edi
00411443  jl         0x41196a

// block 00411449
00411449  lea        esp, [esp]

// block 00411450
00411450  mov        eax, dword ptr [ebp + 0x54]
00411453  movzx      ecx, byte ptr [eax + 2]
00411457  cmp        ecx, 0x11
0041145a  ja         0x41194d

// block 00411460
00411460  jmp        dword ptr [ecx*4 + 0x411a58]

// block 00411467
00411467  cmp        dword ptr [ebp + 0x78], 0
0041146b  jne        0x411519

// block 00411471
00411471  lea        ebx, [ebp + 0x40]
00411474  mov        dword ptr [esp + 0x28], ebx
00411478  mov        dword ptr [esp + 0x10], 5
00411480  jmp        0x411486

// block 00411482
00411482  mov        ebx, dword ptr [esp + 0x28]

// block 00411486
00411486  mov        edx, dword ptr [ebx]
00411488  push       edx
00411489  mov        edx, dword ptr [0x4ce8cc]
0041148f  call       0x461920

// block 00411494
00411494  test       eax, eax
00411496  jne        0x41149a

// block 00411498
00411498  mov        dword ptr [ebx], eax

// block 0041149a
0041149a  push       0x49fb38
0041149f  push       0
004114a1  push       0
004114a3  push       0
004114a5  push       0xffffff
004114aa  xor        edi, edi
004114ac  mov        esi, eax
004114ae  call       0x460760

// block 004114b3
004114b3  mov        eax, dword ptr [ebx]
004114b5  add        esp, 0x14
004114b8  push       eax
004114b9  lea        ebx, [edi + 3]
004114bc  call       0x461970

// block 004114c1
004114c1  add        dword ptr [esp + 0x28], 4
004114c6  sub        dword ptr [esp + 0x10], 1
004114cb  jne        0x411482

// block 004114cd
004114cd  mov        ecx, dword ptr [ebp + 0x40]
004114d0  mov        edx, dword ptr [0x4ce8cc]
004114d6  push       ecx
004114d7  call       0x461920

// block 004114dc
004114dc  mov        esi, eax
004114de  test       esi, esi
004114e0  jne        0x4114e5

// block 004114e2
004114e2  mov        dword ptr [ebp + 0x40], eax

// block 004114e5
004114e5  mov        eax, dword ptr [ebp + 0x54]
004114e8  add        eax, 4
004114eb  call       0x420e40

// block 004114f0
004114f0  mov        edx, dword ptr [ebp + 0x7c]
004114f3  push       eax
004114f4  push       0
004114f6  push       0
004114f8  push       0
004114fa  push       edx
004114fb  xor        edi, edi
004114fd  call       0x460760

// block 00411502
00411502  mov        eax, dword ptr [ebp + 0x40]
00411505  add        esp, 0x14
00411508  push       eax
00411509  lea        ebx, [edi + 2]
0041150c  call       0x461970

// block 00411511
00411511  inc        dword ptr [ebp + 0x78]
00411514  jmp        0x41194d

// block 00411519
00411519  add        eax, 4
0041151c  call       0x420e40

// block 00411521
00411521  mov        ecx, dword ptr [ebp + 0x7c]
00411524  mov        edx, dword ptr [ebp + 0x78]
00411527  push       eax
00411528  push       0
0041152a  push       0
0041152c  push       0
0041152e  push       ecx
0041152f  lea        esi, [ebp + edx*4 + 0x40]
00411533  call       0x461c50

// block 00411538
00411538  xor        edi, edi
0041153a  mov        esi, eax
0041153c  call       0x460760

// block 00411541
00411541  mov        eax, dword ptr [ebp + 0x78]
00411544  mov        ecx, dword ptr [ebp + eax*4 + 0x40]
00411548  add        esp, 0x14
0041154b  push       ecx
0041154c  lea        ebx, [edi + 2]
0041154f  call       0x461970

// block 00411554
00411554  inc        dword ptr [ebp + 0x78]
00411557  cmp        dword ptr [ebp + 0x78], 5
0041155b  jl         0x41194d

// block 00411561
00411561  mov        dword ptr [ebp + 0x78], edi
00411564  jmp        0x41194d

// block 00411569
00411569  lea        esi, [ebp + 0x40]
0041156c  mov        edi, 5

// block 00411571
00411571  mov        edx, dword ptr [esi]
00411573  push       edx
00411574  mov        ebx, 3
00411579  call       0x461970

// block 0041157e
0041157e  add        esi, 4
00411581  sub        edi, 1
00411584  jne        0x411571

// block 00411586
00411586  jmp        0x41194d

// block 0041158b
0041158b  cmp        dword ptr [ebp + 0x30], 0
0041158f  jg         0x41159d

// block 00411591
00411591  mov        eax, dword ptr [eax + 4]
00411594  push       eax
00411595  lea        eax, [ebp + 0x2c]
00411598  call       0x4067e0

// block 0041159d
0041159d  fld        dword ptr [0x4a3da8]
004115a3  push       ecx
004115a4  lea        esi, [ebp + 0x2c]
004115a7  fstp       dword ptr [esp]
004115aa  call       0x464a20

// block 004115af
004115af  mov        ecx, dword ptr [ebp + 0x54]
004115b2  cmp        dword ptr [ecx + 4], 0
004115b6  jge        0x4115c4

// block 004115b8
004115b8  push       0x3e7
004115bd  mov        eax, esi
004115bf  call       0x4067e0

// block 004115c4
004115c4  test       dword ptr [0x4d48c4], 0x80001
004115ce  jne        0x411614

// block 004115d0
004115d0  mov        eax, dword ptr [ebp + 0x30]
004115d3  test       eax, eax
004115d5  jle        0x411614

// block 004115d7
004115d7  mov        edx, dword ptr [0x4b43d8]
004115dd  test       byte ptr [edx + 0x20], 1
004115e1  jne        0x411a4c

// block 004115e7
004115e7  test       dword ptr [0x4d48b8], 0x200
004115f1  je         0x411a4c

// block 004115f7
004115f7  cdq        
004115f8  mov        ecx, 6
004115fd  idiv       ecx
004115ff  test       edx, edx
00411601  jne        0x411a4c

// block 00411607
00411607  push       edx
00411608  mov        eax, esi
0041160a  call       0x4067e0

// block 0041160f
0041160f  jmp        0x41194d

// block 00411614
00411614  xor        edx, edx
00411616  call       0x453d90

// block 0041161b
0041161b  push       0
0041161d  mov        eax, esi
0041161f  call       0x4067e0

// block 00411624
00411624  jmp        0x41194d

// block 00411629
00411629  xor        edi, edi
0041162b  cmp        dword ptr [ebp + 0x30], edi
0041162e  jg         0x41163c

// block 00411630
00411630  mov        edx, dword ptr [eax + 4]
00411633  push       edx
00411634  lea        eax, [ebp + 0x2c]
00411637  call       0x4067e0

// block 0041163c
0041163c  fld        dword ptr [0x4a3da8]
00411642  push       ecx
00411643  lea        esi, [ebp + 0x2c]
00411646  fstp       dword ptr [esp]
00411649  call       0x464a20

// block 0041164e
0041164e  test       dword ptr [0x4d48c4], 0x80001
00411658  jne        0x4116a7

// block 0041165a
0041165a  mov        eax, dword ptr [ebp + 0x30]
0041165d  cmp        eax, edi
0041165f  jle        0x4116a7

// block 00411661
00411661  mov        ecx, dword ptr [0x4b43d8]
00411667  test       byte ptr [ecx + 0x20], 1
0041166b  jne        0x411a4c

// block 00411671
00411671  test       dword ptr [0x4d48b8], 0x200
0041167b  je         0x411a4c

// block 00411681
00411681  cdq        
00411682  mov        ecx, 6
00411687  idiv       ecx
00411689  test       edx, edx
0041168b  jne        0x411a4c

// block 00411691
00411691  push       edi
00411692  mov        eax, esi
00411694  call       0x4067e0

// block 00411699
00411699  mov        dword ptr [ebp + 0x78], edi
0041169c  mov        dword ptr [0x4ce55c], edi
004116a2  jmp        0x41194d

// block 004116a7
004116a7  xor        edx, edx
004116a9  call       0x453d90

// block 004116ae
004116ae  jmp        0x411691

// block 004116b0
004116b0  mov        edx, dword ptr [eax + 4]
004116b3  mov        eax, dword ptr [ebp + edx*4 + 0x90]
004116ba  lea        esi, [ebp + edx*4 + 0x90]
004116c1  push       eax
004116c2  call       0x461a70

// block 004116c7
004116c7  mov        dword ptr [esi], 0
004116cd  mov        eax, dword ptr [ebp + 0x54]
004116d0  mov        ecx, dword ptr [eax + 0xc]
004116d3  mov        eax, dword ptr [eax + 8]
004116d6  push       0
004116d8  push       ecx
004116d9  mov        ecx, dword ptr [ebp + eax*4 + 0x80]
004116e0  lea        edx, [esp + 0x1c]
004116e4  push       edx
004116e5  push       ecx
004116e6  mov        eax, 0x17
004116eb  xor        ecx, ecx
004116ed  call       0x4615a0

// block 004116f2
004116f2  mov        edx, dword ptr [ebp + 0x54]
004116f5  mov        eax, dword ptr [edx + 4]
004116f8  mov        ecx, dword ptr [esp + 0x14]
004116fc  mov        dword ptr [ebp + eax*4 + 0x90], ecx
00411703  jmp        0x41194d

// block 00411708
00411708  cmp        dword ptr [0x4b0ca8], 1
0041170f  jne        0x41194d

// block 00411715
00411715  mov        edx, dword ptr [eax + 4]
00411718  mov        eax, dword ptr [ebp + edx*4 + 0x90]
0041171f  lea        esi, [ebp + edx*4 + 0x90]
00411726  push       eax
00411727  call       0x461a70

// block 0041172c
0041172c  mov        dword ptr [esi], 0
00411732  mov        eax, dword ptr [ebp + 0x54]
00411735  mov        ecx, dword ptr [eax + 0xc]
00411738  mov        eax, dword ptr [eax + 8]
0041173b  push       0
0041173d  push       ecx
0041173e  mov        ecx, dword ptr [ebp + eax*4 + 0x80]
00411745  lea        edx, [esp + 0x20]
00411749  push       edx
0041174a  push       ecx
0041174b  mov        eax, 0x17
00411750  xor        ecx, ecx
00411752  call       0x4615a0

// block 00411757
00411757  mov        edx, dword ptr [ebp + 0x54]
0041175a  mov        eax, dword ptr [edx + 4]
0041175d  mov        ecx, dword ptr [esp + 0x18]
00411761  mov        dword ptr [ebp + eax*4 + 0x90], ecx
00411768  jmp        0x41194d

// block 0041176d
0041176d  cmp        dword ptr [0x4b0ca8], 2
00411774  jne        0x41194d

// block 0041177a
0041177a  mov        edx, dword ptr [eax + 4]
0041177d  mov        eax, dword ptr [ebp + edx*4 + 0x90]
00411784  lea        esi, [ebp + edx*4 + 0x90]
0041178b  push       eax
0041178c  call       0x461a70

// block 00411791
00411791  mov        dword ptr [esi], 0
00411797  mov        eax, dword ptr [ebp + 0x54]
0041179a  mov        ecx, dword ptr [eax + 0xc]
0041179d  mov        eax, dword ptr [eax + 8]
004117a0  push       0
004117a2  push       ecx
004117a3  mov        ecx, dword ptr [ebp + eax*4 + 0x80]
004117aa  lea        edx, [esp + 0x24]
004117ae  push       edx
004117af  push       ecx
004117b0  mov        eax, 0x17
004117b5  xor        ecx, ecx
004117b7  call       0x4615a0

// block 004117bc
004117bc  mov        edx, dword ptr [ebp + 0x54]
004117bf  mov        eax, dword ptr [edx + 4]
004117c2  mov        ecx, dword ptr [esp + 0x1c]
004117c6  mov        dword ptr [ebp + eax*4 + 0x90], ecx
004117cd  jmp        0x41194d

// block 004117d2
004117d2  cmp        dword ptr [0x4b0ca8], 3
004117d9  jne        0x41194d

// block 004117df
004117df  mov        edx, dword ptr [eax + 4]
004117e2  mov        eax, dword ptr [ebp + edx*4 + 0x90]
004117e9  lea        esi, [ebp + edx*4 + 0x90]
004117f0  push       eax
004117f1  call       0x461a70

// block 004117f6
004117f6  mov        dword ptr [esi], 0
004117fc  mov        eax, dword ptr [ebp + 0x54]
004117ff  mov        ecx, dword ptr [eax + 0xc]
00411802  mov        eax, dword ptr [eax + 8]
00411805  push       0
00411807  push       ecx
00411808  mov        ecx, dword ptr [ebp + eax*4 + 0x80]
0041180f  lea        edx, [esp + 0x28]
00411813  push       edx
00411814  push       ecx
00411815  mov        eax, 0x17
0041181a  xor        ecx, ecx
0041181c  call       0x4615a0

// block 00411821
00411821  mov        edx, dword ptr [ebp + 0x54]
00411824  mov        eax, dword ptr [edx + 4]
00411827  mov        ecx, dword ptr [esp + 0x20]
0041182b  mov        dword ptr [ebp + eax*4 + 0x90], ecx
00411832  jmp        0x41194d

// block 00411837
00411837  mov        edx, dword ptr [eax + 4]
0041183a  mov        dword ptr [ebp + 0x7c], edx
0041183d  jmp        0x41194d

// block 00411842
00411842  add        eax, 4
00411845  push       eax
00411846  push       0
00411848  call       0x4300d0

// block 0041184d
0041184d  mov        eax, dword ptr [ebp + 0x54]
00411850  mov        ecx, 0x49fb3c
00411855  add        eax, 4

// block 00411858
00411858  mov        dl, byte ptr [eax]
0041185a  cmp        dl, byte ptr [ecx]
0041185c  jne        0x411878

// block 0041185e
0041185e  test       dl, dl
00411860  je         0x411874

// block 00411862
00411862  mov        dl, byte ptr [eax + 1]
00411865  cmp        dl, byte ptr [ecx + 1]
00411868  jne        0x411878

// block 0041186a
0041186a  add        eax, 2
0041186d  add        ecx, 2
00411870  test       dl, dl
00411872  jne        0x411858

// block 00411874
00411874  xor        eax, eax
00411876  jmp        0x41187d

// block 00411878
00411878  sbb        eax, eax
0041187a  sbb        eax, -1

// block 0041187d
0041187d  test       eax, eax
0041187f  jne        0x41188e

// block 00411881
00411881  push       0xf
00411883  push       eax
00411884  call       0x430150

// block 00411889
00411889  jmp        0x41194d

// block 0041188e
0041188e  push       0x10
00411890  push       0
00411892  call       0x430150

// block 00411897
00411897  jmp        0x41194d

// block 0041189c
0041189c  fld        dword ptr [0x4a4020]
004118a2  push       ecx
004118a3  fstp       dword ptr [esp]
004118a6  call       0x430270

// block 004118ab
004118ab  and        dword ptr [ebp + 0x74], 0xfffffffe
004118af  jmp        0x41194d

// block 004118b4
004118b4  lea        esi, [ebp + 0x40]
004118b7  mov        edi, 5
004118bc  lea        esp, [esp]

// block 004118c0
004118c0  mov        eax, dword ptr [esi]
004118c2  push       eax
004118c3  call       0x461a70

// block 004118c8
004118c8  mov        dword ptr [esi], 0
004118ce  add        esi, 4
004118d1  sub        edi, 1
004118d4  jne        0x4118c0

// block 004118d6
004118d6  mov        eax, dword ptr [ebp + 0x54]
004118d9  add        eax, 4
004118dc  call       0x410b30

// block 004118e1
004118e1  mov        esi, eax
004118e3  test       esi, esi
004118e5  je         0x411a27

// block 004118eb
004118eb  push       0xf0
004118f0  push       edi
004118f1  push       ebp
004118f2  call       0x477420

// block 004118f7
004118f7  mov        ecx, dword ptr [esi + 4]
004118fa  add        esp, 0xc
004118fd  add        ecx, esi
004118ff  push       edi
00411900  lea        eax, [ebp + 4]
00411903  mov        dword ptr [ebp + 0x54], ecx
00411906  call       0x4067e0

// block 0041190b
0041190b  push       edi
0041190c  lea        eax, [ebp + 0x18]
0041190f  call       0x4067e0

// block 00411914
00411914  push       edi
00411915  lea        eax, [ebp + 0x2c]
00411918  call       0x4067e0

// block 0041191d
0041191d  or         dword ptr [ebp + 0x74], 2
00411921  mov        dword ptr [ebp + 0x7c], 0xffffff
00411928  jmp        0x41195b

// block 0041192a
0041192a  mov        edx, dword ptr [eax + 4]
0041192d  push       0x46
0041192f  push       0
00411931  push       0
00411933  push       0
00411935  push       edx
00411936  push       0
00411938  jmp        0x411948

// block 0041193a
0041193a  mov        eax, dword ptr [eax + 4]
0041193d  push       0x46
0041193f  push       0
00411941  push       0
00411943  push       0
00411945  push       eax
00411946  push       5

// block 00411948
00411948  call       0x4529a0

// block 0041194d
0041194d  mov        eax, dword ptr [ebp + 0x54]
00411950  movzx      ecx, byte ptr [eax + 3]
00411954  lea        edx, [ecx + eax + 4]
00411958  mov        dword ptr [ebp + 0x54], edx

// block 0041195b
0041195b  mov        eax, dword ptr [ebp + 0x54]
0041195e  movzx      ecx, word ptr [eax]
00411961  cmp        dword ptr [ebp + 0x1c], ecx
00411964  jge        0x411450

// block 0041196a
0041196a  mov        edx, dword ptr [ebp + 0x1c]
0041196d  mov        ecx, dword ptr [ebp + 0x24]
00411970  mov        dword ptr [ebp + 0x18], edx
00411973  fld        dword ptr [ecx]
00411975  fcomp      qword ptr [0x4a3db0]
0041197b  fnstsw     ax
0041197d  test       ah, 0x41
00411980  jne        0x411a34

// block 00411986
00411986  fld        dword ptr [0x4a3dac]
0041198c  fcomp      dword ptr [ecx]
0041198e  fnstsw     ax
00411990  test       ah, 0x41
00411993  jne        0x411a34

// block 00411999
00411999  fld        dword ptr [ebp + 0x20]
0041199c  pop        edi
0041199d  fadd       qword ptr [0x4a3ce0]
004119a3  pop        esi
004119a4  inc        edx
004119a5  pop        ebx
004119a6  fstp       dword ptr [ebp + 0x20]
004119a9  mov        dword ptr [ebp + 0x1c], edx
004119ac  xor        eax, eax
004119ae  pop        ebp
004119af  add        esp, 0x14
004119b2  ret        4

// block 004119b5
004119b5  fld        dword ptr [0x4a4260]
004119bb  sub        esp, 8
004119be  fstp       dword ptr [esp + 4]
004119c2  fld        dword ptr [0x4a3ec0]
004119c8  fstp       dword ptr [esp]
004119cb  call       0x411b80

// block 004119d0
004119d0  mov        edx, dword ptr [ebp + 0x54]
004119d3  mov        esi, dword ptr [edx + 4]
004119d6  add        esi, 0x1c
004119d9  js         0x4119e6

// block 004119db
004119db  mov        ebx, dword ptr [0x4ce8cc]
004119e1  call       0x4604a0

// block 004119e6
004119e6  mov        eax, dword ptr [ebp + 0x54]
004119e9  or         dword ptr [ebp + 0x74], 4
004119ed  lea        ecx, [eax + 8]
004119f0  mov        dword ptr [ebp + 0x70], ecx
004119f3  mov        edx, dword ptr [eax + 4]
004119f6  push       ebp
004119f7  lea        eax, [ebp + 0xd0]
004119fd  mov        edi, 0x411aa0
00411a02  mov        dword ptr [ebp + 0xec], edx
00411a08  call       0x464cb0

// block 00411a0d
00411a0d  mov        eax, dword ptr [ebp + 0x54]
00411a10  movzx      ecx, byte ptr [eax + 3]
00411a14  pop        edi
00411a15  pop        esi
00411a16  lea        edx, [ecx + eax + 4]
00411a1a  pop        ebx
00411a1b  mov        dword ptr [ebp + 0x54], edx
00411a1e  xor        eax, eax
00411a20  pop        ebp
00411a21  add        esp, 0x14
00411a24  ret        4

// block 00411a27
00411a27  pop        edi
00411a28  pop        esi
00411a29  pop        ebx
00411a2a  or         eax, 0xffffffff
00411a2d  pop        ebp
00411a2e  add        esp, 0x14
00411a31  ret        4

// block 00411a34
00411a34  fld        dword ptr [ecx]
00411a36  fadd       dword ptr [ebp + 0x20]
00411a39  fstp       dword ptr [esp + 0x28]
00411a3d  fld        dword ptr [esp + 0x28]
00411a41  fst        dword ptr [ebp + 0x20]
00411a44  call       0x4931e0

// block 00411a49
00411a49  mov        dword ptr [ebp + 0x1c], eax

// block 00411a4c
00411a4c  pop        edi
00411a4d  pop        esi
00411a4e  pop        ebx
00411a4f  xor        eax, eax
00411a51  pop        ebp
00411a52  add        esp, 0x14
00411a55  ret        4
