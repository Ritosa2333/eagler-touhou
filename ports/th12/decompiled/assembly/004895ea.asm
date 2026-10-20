
// block 004895ea
004895ea  mov        edi, edi
004895ec  push       ebp
004895ed  mov        ebp, esp
004895ef  sub        esp, 0x2c
004895f2  mov        eax, dword ptr [ebp + 8]
004895f5  movzx      ecx, word ptr [eax + 0xa]
004895f9  push       ebx
004895fa  mov        ebx, ecx
004895fc  and        ecx, 0x8000
00489602  mov        dword ptr [ebp - 0x14], ecx
00489605  mov        ecx, dword ptr [eax + 6]
00489608  mov        dword ptr [ebp - 0x20], ecx
0048960b  mov        ecx, dword ptr [eax + 2]
0048960e  movzx      eax, word ptr [eax]
00489611  and        ebx, 0x7fff
00489617  sub        ebx, 0x3fff
0048961d  shl        eax, 0x10
00489620  push       edi
00489621  mov        dword ptr [ebp - 0x1c], ecx
00489624  mov        dword ptr [ebp - 0x18], eax
00489627  cmp        ebx, 0xffffc001
0048962d  jne        0x489656

// block 0048962f
0048962f  xor        ebx, ebx
00489631  xor        eax, eax

// block 00489633
00489633  cmp        dword ptr [ebp + eax*4 - 0x20], ebx
00489637  jne        0x489646

// block 00489639
00489639  inc        eax
0048963a  cmp        eax, 3
0048963d  jl         0x489633

// block 0048963f
0048963f  xor        eax, eax
00489641  jmp        0x489aeb

// block 00489646
00489646  xor        eax, eax
00489648  lea        edi, [ebp - 0x20]
0048964b  stosd      dword ptr es:[edi], eax
0048964c  stosd      dword ptr es:[edi], eax
0048964d  push       2
0048964f  stosd      dword ptr es:[edi], eax
00489650  pop        eax
00489651  jmp        0x489aeb

// block 00489656
00489656  and        dword ptr [ebp + 8], 0
0048965a  push       esi
0048965b  lea        esi, [ebp - 0x20]
0048965e  lea        edi, [ebp - 0x2c]
00489661  movsd      dword ptr es:[edi], dword ptr [esi]
00489662  movsd      dword ptr es:[edi], dword ptr [esi]
00489663  movsd      dword ptr es:[edi], dword ptr [esi]
00489664  mov        esi, dword ptr [0x4adfb8]
0048966a  dec        esi
0048966b  lea        ecx, [esi + 1]
0048966e  mov        eax, ecx
00489670  cdq        
00489671  and        edx, 0x1f
00489674  add        eax, edx
00489676  sar        eax, 5
00489679  mov        edx, ecx
0048967b  and        edx, 0x8000001f
00489681  mov        dword ptr [ebp - 0x10], ebx
00489684  mov        dword ptr [ebp - 0xc], eax
00489687  jns        0x48968e

// block 00489689
00489689  dec        edx
0048968a  or         edx, 0xffffffe0
0048968d  inc        edx

// block 0048968e
0048968e  lea        edi, [ebp + eax*4 - 0x20]
00489692  push       0x1f
00489694  xor        eax, eax
00489696  pop        ecx
00489697  sub        ecx, edx
00489699  inc        eax
0048969a  shl        eax, cl
0048969c  mov        dword ptr [ebp - 8], ecx
0048969f  test       dword ptr [edi], eax
004896a1  je         0x489734

// block 004896a7
004896a7  mov        eax, dword ptr [ebp - 0xc]
004896aa  or         edx, 0xffffffff
004896ad  shl        edx, cl
004896af  not        edx
004896b1  test       dword ptr [ebp + eax*4 - 0x20], edx
004896b5  jmp        0x4896bc

// block 004896b7
004896b7  cmp        dword ptr [ebp + eax*4 - 0x20], 0

// block 004896bc
004896bc  jne        0x4896c6

// block 004896be
004896be  inc        eax
004896bf  cmp        eax, 3
004896c2  jl         0x4896b7

// block 004896c4
004896c4  jmp        0x489734

// block 004896c6
004896c6  mov        eax, esi
004896c8  cdq        
004896c9  push       0x1f
004896cb  pop        ecx
004896cc  and        edx, ecx
004896ce  add        eax, edx
004896d0  sar        eax, 5
004896d3  and        esi, 0x8000001f
004896d9  jns        0x4896e0

// block 004896db
004896db  dec        esi
004896dc  or         esi, 0xffffffe0
004896df  inc        esi

// block 004896e0
004896e0  and        dword ptr [ebp - 4], 0
004896e4  sub        ecx, esi
004896e6  xor        edx, edx
004896e8  inc        edx
004896e9  shl        edx, cl
004896eb  lea        ecx, [ebp + eax*4 - 0x20]
004896ef  mov        esi, dword ptr [ecx]
004896f1  add        esi, edx
004896f3  mov        dword ptr [ebp + 8], esi
004896f6  mov        esi, dword ptr [ecx]
004896f8  cmp        dword ptr [ebp + 8], esi
004896fb  jb         0x48971f

// block 004896fd
004896fd  cmp        dword ptr [ebp + 8], edx
00489700  jmp        0x48971d

// block 00489702
00489702  test       ecx, ecx
00489704  je         0x489731

// block 00489706
00489706  and        dword ptr [ebp - 4], 0
0048970a  lea        ecx, [ebp + eax*4 - 0x20]
0048970e  mov        edx, dword ptr [ecx]
00489710  lea        esi, [edx + 1]
00489713  mov        dword ptr [ebp + 8], esi
00489716  cmp        esi, edx
00489718  jb         0x48971f

// block 0048971a
0048971a  cmp        esi, 1

// block 0048971d
0048971d  jae        0x489726

// block 0048971f
0048971f  mov        dword ptr [ebp - 4], 1

// block 00489726
00489726  dec        eax
00489727  mov        edx, dword ptr [ebp + 8]
0048972a  mov        dword ptr [ecx], edx
0048972c  mov        ecx, dword ptr [ebp - 4]
0048972f  jns        0x489702

// block 00489731
00489731  mov        dword ptr [ebp + 8], ecx

// block 00489734
00489734  mov        ecx, dword ptr [ebp - 8]
00489737  or         eax, 0xffffffff
0048973a  shl        eax, cl
0048973c  and        dword ptr [edi], eax
0048973e  mov        eax, dword ptr [ebp - 0xc]
00489741  inc        eax
00489742  cmp        eax, 3
00489745  jge        0x489754

// block 00489747
00489747  push       3
00489749  pop        ecx
0048974a  lea        edi, [ebp + eax*4 - 0x20]
0048974e  sub        ecx, eax
00489750  xor        eax, eax

// block 00489752
00489752  rep stosd  dword ptr es:[edi], eax

// block 00489754
00489754  cmp        dword ptr [ebp + 8], 0
00489758  je         0x48975b

// block 0048975a
0048975a  inc        ebx

// block 0048975b
0048975b  mov        eax, dword ptr [0x4adfb4]
00489760  mov        ecx, eax
00489762  sub        ecx, dword ptr [0x4adfb8]
00489768  cmp        ebx, ecx
0048976a  jge        0x489779

// block 0048976c
0048976c  xor        eax, eax
0048976e  lea        edi, [ebp - 0x20]
00489771  stosd      dword ptr es:[edi], eax
00489772  stosd      dword ptr es:[edi], eax
00489773  stosd      dword ptr es:[edi], eax
00489774  jmp        0x489986

// block 00489779
00489779  cmp        ebx, eax
0048977b  jg         0x489990

// block 00489781
00489781  sub        eax, dword ptr [ebp - 0x10]
00489784  lea        esi, [ebp - 0x2c]
00489787  mov        ecx, eax
00489789  lea        edi, [ebp - 0x20]
0048978c  movsd      dword ptr es:[edi], dword ptr [esi]
0048978d  cdq        
0048978e  and        edx, 0x1f
00489791  add        eax, edx
00489793  movsd      dword ptr es:[edi], dword ptr [esi]
00489794  mov        edx, ecx
00489796  sar        eax, 5
00489799  and        edx, 0x8000001f
0048979f  movsd      dword ptr es:[edi], dword ptr [esi]
004897a0  jns        0x4897a7

// block 004897a2
004897a2  dec        edx
004897a3  or         edx, 0xffffffe0
004897a6  inc        edx

// block 004897a7
004897a7  and        dword ptr [ebp - 0xc], 0
004897ab  and        dword ptr [ebp + 8], 0
004897af  or         edi, 0xffffffff
004897b2  mov        ecx, edx
004897b4  shl        edi, cl
004897b6  mov        dword ptr [ebp - 4], 0x20
004897bd  sub        dword ptr [ebp - 4], edx
004897c0  not        edi

// block 004897c2
004897c2  mov        ebx, dword ptr [ebp + 8]
004897c5  lea        ebx, [ebp + ebx*4 - 0x20]
004897c9  mov        esi, dword ptr [ebx]
004897cb  mov        ecx, esi
004897cd  and        ecx, edi
004897cf  mov        dword ptr [ebp - 0x10], ecx
004897d2  mov        ecx, edx
004897d4  shr        esi, cl
004897d6  mov        ecx, dword ptr [ebp - 4]
004897d9  or         esi, dword ptr [ebp - 0xc]
004897dc  mov        dword ptr [ebx], esi
004897de  mov        esi, dword ptr [ebp - 0x10]
004897e1  shl        esi, cl
004897e3  inc        dword ptr [ebp + 8]
004897e6  cmp        dword ptr [ebp + 8], 3
004897ea  mov        dword ptr [ebp - 0xc], esi
004897ed  jl         0x4897c2

// block 004897ef
004897ef  mov        esi, eax
004897f1  push       2
004897f3  shl        esi, 2
004897f6  lea        ecx, [ebp - 0x18]
004897f9  pop        edx
004897fa  sub        ecx, esi

// block 004897fc
004897fc  cmp        edx, eax
004897fe  jl         0x489808

// block 00489800
00489800  mov        esi, dword ptr [ecx]
00489802  mov        dword ptr [ebp + edx*4 - 0x20], esi
00489806  jmp        0x48980d

// block 00489808
00489808  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 0048980d
0048980d  dec        edx
0048980e  sub        ecx, 4
00489811  test       edx, edx
00489813  jge        0x4897fc

// block 00489815
00489815  mov        esi, dword ptr [0x4adfb8]
0048981b  dec        esi
0048981c  lea        ecx, [esi + 1]
0048981f  mov        eax, ecx
00489821  cdq        
00489822  and        edx, 0x1f
00489825  add        eax, edx
00489827  sar        eax, 5
0048982a  mov        edx, ecx
0048982c  and        edx, 0x8000001f
00489832  mov        dword ptr [ebp - 0xc], eax
00489835  jns        0x48983c

// block 00489837
00489837  dec        edx
00489838  or         edx, 0xffffffe0
0048983b  inc        edx

// block 0048983c
0048983c  push       0x1f
0048983e  pop        ecx
0048983f  sub        ecx, edx
00489841  xor        edx, edx
00489843  inc        edx
00489844  shl        edx, cl
00489846  lea        ebx, [ebp + eax*4 - 0x20]
0048984a  mov        dword ptr [ebp - 0x10], ecx
0048984d  test       dword ptr [ebx], edx
0048984f  je         0x4898d7

// block 00489855
00489855  or         edx, 0xffffffff
00489858  shl        edx, cl
0048985a  not        edx
0048985c  test       dword ptr [ebp + eax*4 - 0x20], edx
00489860  jmp        0x489867

// block 00489862
00489862  cmp        dword ptr [ebp + eax*4 - 0x20], 0

// block 00489867
00489867  jne        0x489871

// block 00489869
00489869  inc        eax
0048986a  cmp        eax, 3
0048986d  jl         0x489862

// block 0048986f
0048986f  jmp        0x4898d7

// block 00489871
00489871  mov        eax, esi
00489873  cdq        
00489874  push       0x1f
00489876  pop        ecx
00489877  and        edx, ecx
00489879  add        eax, edx
0048987b  sar        eax, 5
0048987e  and        esi, 0x8000001f
00489884  jns        0x48988b

// block 00489886
00489886  dec        esi
00489887  or         esi, 0xffffffe0
0048988a  inc        esi

// block 0048988b
0048988b  and        dword ptr [ebp + 8], 0
0048988f  xor        edx, edx
00489891  sub        ecx, esi
00489893  inc        edx
00489894  shl        edx, cl
00489896  lea        ecx, [ebp + eax*4 - 0x20]
0048989a  mov        esi, dword ptr [ecx]
0048989c  lea        edi, [esi + edx]
0048989f  cmp        edi, esi
004898a1  jb         0x4898a7

// block 004898a3
004898a3  cmp        edi, edx
004898a5  jae        0x4898ae

// block 004898a7
004898a7  mov        dword ptr [ebp + 8], 1

// block 004898ae
004898ae  mov        dword ptr [ecx], edi
004898b0  mov        ecx, dword ptr [ebp + 8]
004898b3  jmp        0x4898d4

// block 004898b5
004898b5  test       ecx, ecx
004898b7  je         0x4898d7

// block 004898b9
004898b9  lea        ecx, [ebp + eax*4 - 0x20]
004898bd  mov        edx, dword ptr [ecx]
004898bf  lea        esi, [edx + 1]
004898c2  xor        edi, edi
004898c4  cmp        esi, edx
004898c6  jb         0x4898cd

// block 004898c8
004898c8  cmp        esi, 1
004898cb  jae        0x4898d0

// block 004898cd
004898cd  xor        edi, edi
004898cf  inc        edi

// block 004898d0
004898d0  mov        dword ptr [ecx], esi
004898d2  mov        ecx, edi

// block 004898d4
004898d4  dec        eax
004898d5  jns        0x4898b5

// block 004898d7
004898d7  mov        ecx, dword ptr [ebp - 0x10]
004898da  or         eax, 0xffffffff
004898dd  shl        eax, cl
004898df  and        dword ptr [ebx], eax
004898e1  mov        eax, dword ptr [ebp - 0xc]
004898e4  inc        eax
004898e5  cmp        eax, 3
004898e8  jge        0x4898f7

// block 004898ea
004898ea  push       3
004898ec  pop        ecx
004898ed  lea        edi, [ebp + eax*4 - 0x20]
004898f1  sub        ecx, eax
004898f3  xor        eax, eax

// block 004898f5
004898f5  rep stosd  dword ptr es:[edi], eax

// block 004898f7
004898f7  mov        ecx, dword ptr [0x4adfbc]
004898fd  inc        ecx
004898fe  mov        eax, ecx
00489900  cdq        
00489901  and        edx, 0x1f
00489904  add        eax, edx
00489906  mov        edx, ecx
00489908  sar        eax, 5
0048990b  and        edx, 0x8000001f
00489911  jns        0x489918

// block 00489913
00489913  dec        edx
00489914  or         edx, 0xffffffe0
00489917  inc        edx

// block 00489918
00489918  and        dword ptr [ebp - 0xc], 0
0048991c  and        dword ptr [ebp + 8], 0
00489920  or         edi, 0xffffffff
00489923  mov        ecx, edx
00489925  shl        edi, cl
00489927  mov        dword ptr [ebp - 4], 0x20
0048992e  sub        dword ptr [ebp - 4], edx
00489931  not        edi

// block 00489933
00489933  mov        ebx, dword ptr [ebp + 8]
00489936  lea        ebx, [ebp + ebx*4 - 0x20]
0048993a  mov        esi, dword ptr [ebx]
0048993c  mov        ecx, esi
0048993e  and        ecx, edi
00489940  mov        dword ptr [ebp - 0x10], ecx
00489943  mov        ecx, edx
00489945  shr        esi, cl
00489947  mov        ecx, dword ptr [ebp - 4]
0048994a  or         esi, dword ptr [ebp - 0xc]
0048994d  mov        dword ptr [ebx], esi
0048994f  mov        esi, dword ptr [ebp - 0x10]
00489952  shl        esi, cl
00489954  inc        dword ptr [ebp + 8]
00489957  cmp        dword ptr [ebp + 8], 3
0048995b  mov        dword ptr [ebp - 0xc], esi
0048995e  jl         0x489933

// block 00489960
00489960  mov        esi, eax
00489962  push       2
00489964  shl        esi, 2
00489967  lea        ecx, [ebp - 0x18]
0048996a  pop        edx
0048996b  sub        ecx, esi

// block 0048996d
0048996d  cmp        edx, eax
0048996f  jl         0x489979

// block 00489971
00489971  mov        esi, dword ptr [ecx]
00489973  mov        dword ptr [ebp + edx*4 - 0x20], esi
00489977  jmp        0x48997e

// block 00489979
00489979  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 0048997e
0048997e  dec        edx
0048997f  sub        ecx, 4
00489982  test       edx, edx
00489984  jge        0x48996d

// block 00489986
00489986  push       2
00489988  xor        ebx, ebx
0048998a  pop        eax
0048998b  jmp        0x489aea

// block 00489990
00489990  cmp        ebx, dword ptr [0x4adfb0]
00489996  mov        ecx, dword ptr [0x4adfbc]
0048999c  jl         0x489a4f

// block 004899a2
004899a2  xor        eax, eax
004899a4  lea        edi, [ebp - 0x20]
004899a7  stosd      dword ptr es:[edi], eax
004899a8  stosd      dword ptr es:[edi], eax
004899a9  stosd      dword ptr es:[edi], eax
004899aa  or         dword ptr [ebp - 0x20], 0x80000000
004899b1  mov        eax, ecx
004899b3  cdq        
004899b4  and        edx, 0x1f
004899b7  add        eax, edx
004899b9  mov        edx, ecx
004899bb  sar        eax, 5
004899be  and        edx, 0x8000001f
004899c4  jns        0x4899cb

// block 004899c6
004899c6  dec        edx
004899c7  or         edx, 0xffffffe0
004899ca  inc        edx

// block 004899cb
004899cb  and        dword ptr [ebp - 0xc], 0
004899cf  and        dword ptr [ebp + 8], 0
004899d3  or         edi, 0xffffffff
004899d6  mov        ecx, edx
004899d8  shl        edi, cl
004899da  mov        dword ptr [ebp - 4], 0x20
004899e1  sub        dword ptr [ebp - 4], edx
004899e4  not        edi

// block 004899e6
004899e6  mov        ebx, dword ptr [ebp + 8]
004899e9  lea        ebx, [ebp + ebx*4 - 0x20]
004899ed  mov        esi, dword ptr [ebx]
004899ef  mov        ecx, esi
004899f1  and        ecx, edi
004899f3  mov        dword ptr [ebp - 0x10], ecx
004899f6  mov        ecx, edx
004899f8  shr        esi, cl
004899fa  mov        ecx, dword ptr [ebp - 4]
004899fd  or         esi, dword ptr [ebp - 0xc]
00489a00  mov        dword ptr [ebx], esi
00489a02  mov        esi, dword ptr [ebp - 0x10]
00489a05  shl        esi, cl
00489a07  inc        dword ptr [ebp + 8]
00489a0a  cmp        dword ptr [ebp + 8], 3
00489a0e  mov        dword ptr [ebp - 0xc], esi
00489a11  jl         0x4899e6

// block 00489a13
00489a13  mov        esi, eax
00489a15  push       2
00489a17  shl        esi, 2
00489a1a  lea        ecx, [ebp - 0x18]
00489a1d  pop        edx
00489a1e  sub        ecx, esi

// block 00489a20
00489a20  cmp        edx, eax
00489a22  jl         0x489a2c

// block 00489a24
00489a24  mov        esi, dword ptr [ecx]
00489a26  mov        dword ptr [ebp + edx*4 - 0x20], esi
00489a2a  jmp        0x489a31

// block 00489a2c
00489a2c  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 00489a31
00489a31  dec        edx
00489a32  sub        ecx, 4
00489a35  test       edx, edx
00489a37  jge        0x489a20

// block 00489a39
00489a39  mov        eax, dword ptr [0x4adfb0]
00489a3e  mov        ecx, dword ptr [0x4adfc4]
00489a44  lea        ebx, [ecx + eax]
00489a47  xor        eax, eax
00489a49  inc        eax
00489a4a  jmp        0x489aea

// block 00489a4f
00489a4f  mov        eax, dword ptr [0x4adfc4]
00489a54  and        dword ptr [ebp - 0x20], 0x7fffffff
00489a5b  add        ebx, eax
00489a5d  mov        eax, ecx
00489a5f  cdq        
00489a60  and        edx, 0x1f
00489a63  add        eax, edx
00489a65  mov        edx, ecx
00489a67  sar        eax, 5
00489a6a  and        edx, 0x8000001f
00489a70  jns        0x489a77

// block 00489a72
00489a72  dec        edx
00489a73  or         edx, 0xffffffe0
00489a76  inc        edx

// block 00489a77
00489a77  and        dword ptr [ebp - 0xc], 0
00489a7b  and        dword ptr [ebp + 8], 0
00489a7f  or         esi, 0xffffffff
00489a82  mov        ecx, edx
00489a84  shl        esi, cl
00489a86  mov        dword ptr [ebp - 4], 0x20
00489a8d  sub        dword ptr [ebp - 4], edx
00489a90  not        esi

// block 00489a92
00489a92  mov        ecx, dword ptr [ebp + 8]
00489a95  mov        edi, dword ptr [ebp + ecx*4 - 0x20]
00489a99  mov        ecx, edi
00489a9b  and        ecx, esi
00489a9d  mov        dword ptr [ebp - 0x10], ecx
00489aa0  mov        ecx, edx
00489aa2  shr        edi, cl
00489aa4  mov        ecx, dword ptr [ebp + 8]
00489aa7  or         edi, dword ptr [ebp - 0xc]
00489aaa  mov        dword ptr [ebp + ecx*4 - 0x20], edi
00489aae  mov        edi, dword ptr [ebp - 0x10]
00489ab1  mov        ecx, dword ptr [ebp - 4]
00489ab4  shl        edi, cl
00489ab6  inc        dword ptr [ebp + 8]
00489ab9  cmp        dword ptr [ebp + 8], 3
00489abd  mov        dword ptr [ebp - 0xc], edi
00489ac0  jl         0x489a92

// block 00489ac2
00489ac2  mov        esi, eax
00489ac4  push       2
00489ac6  shl        esi, 2
00489ac9  lea        ecx, [ebp - 0x18]
00489acc  pop        edx
00489acd  sub        ecx, esi

// block 00489acf
00489acf  cmp        edx, eax
00489ad1  jl         0x489adb

// block 00489ad3
00489ad3  mov        esi, dword ptr [ecx]
00489ad5  mov        dword ptr [ebp + edx*4 - 0x20], esi
00489ad9  jmp        0x489ae0

// block 00489adb
00489adb  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 00489ae0
00489ae0  dec        edx
00489ae1  sub        ecx, 4
00489ae4  test       edx, edx
00489ae6  jge        0x489acf

// block 00489ae8
00489ae8  xor        eax, eax

// block 00489aea
00489aea  pop        esi

// block 00489aeb
00489aeb  push       0x1f
00489aed  pop        ecx
00489aee  sub        ecx, dword ptr [0x4adfbc]
00489af4  shl        ebx, cl
00489af6  mov        ecx, dword ptr [ebp - 0x14]
00489af9  neg        ecx
00489afb  sbb        ecx, ecx
00489afd  and        ecx, 0x80000000
00489b03  or         ebx, ecx
00489b05  mov        ecx, dword ptr [0x4adfc0]
00489b0b  or         ebx, dword ptr [ebp - 0x20]
00489b0e  cmp        ecx, 0x40
00489b11  jne        0x489b20

// block 00489b13
00489b13  mov        ecx, dword ptr [ebp + 0xc]
00489b16  mov        edx, dword ptr [ebp - 0x1c]
00489b19  mov        dword ptr [ecx + 4], ebx
00489b1c  mov        dword ptr [ecx], edx
00489b1e  jmp        0x489b2a

// block 00489b20
00489b20  cmp        ecx, 0x20
00489b23  jne        0x489b2a

// block 00489b25
00489b25  mov        ecx, dword ptr [ebp + 0xc]
00489b28  mov        dword ptr [ecx], ebx

// block 00489b2a
00489b2a  pop        edi
00489b2b  pop        ebx
00489b2c  leave      
00489b2d  ret        
