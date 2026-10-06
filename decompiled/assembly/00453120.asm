
// block 00453120
00453120  sub        esp, 0x4c
00453123  mov        eax, dword ptr [0x4ad138]
00453128  xor        eax, esp
0045312a  mov        dword ptr [esp + 0x48], eax
0045312e  push       ebx
0045312f  push       esi
00453130  mov        esi, ecx
00453132  push       edi
00453133  mov        ebx, edx
00453135  xor        eax, eax
00453137  lea        ecx, [esi + 0x1988]
0045313d  lea        ecx, [ecx]

// block 00453140
00453140  mov        dword ptr [ecx + 4], 0xffffffff
00453147  mov        edx, 0x4ae5a0
0045314c  lea        esp, [esp]

// block 00453150
00453150  cmp        dword ptr [edx], eax
00453152  je         0x453159

// block 00453154
00453154  add        edx, 0x14
00453157  jne        0x453150

// block 00453159
00453159  mov        dword ptr [ecx + 0xc], eax
0045315c  mov        dword ptr [ecx + 8], edx
0045315f  inc        eax
00453160  add        ecx, 0x18
00453163  cmp        eax, 0x3c
00453166  jl         0x453140

// block 00453168
00453168  or         eax, 0xffffffff
0045316b  mov        dword ptr [esi + 0x20], eax
0045316e  mov        dword ptr [esi + 0x24], eax
00453171  mov        dword ptr [esi + 0x28], eax
00453174  mov        dword ptr [esi + 0x2c], eax
00453177  mov        dword ptr [esi + 0x30], eax
0045317a  mov        dword ptr [esi + 0x34], eax
0045317d  mov        dword ptr [esi + 0x38], eax
00453180  mov        dword ptr [esi + 0x3c], eax
00453183  mov        dword ptr [esi + 0x40], eax
00453186  mov        dword ptr [esi + 0x44], eax
00453189  mov        dword ptr [esi + 0x48], eax
0045318c  push       4
0045318e  mov        dword ptr [esi + 0x4c], eax
00453191  call       0x46c9ea

// block 00453196
00453196  add        esp, 4
00453199  test       eax, eax
0045319b  je         0x4531a7

// block 0045319d
0045319d  mov        dword ptr [eax], 0
004531a3  mov        edi, eax
004531a5  jmp        0x4531a9

// block 004531a7
004531a7  xor        edi, edi

// block 004531a9
004531a9  mov        dword ptr [esi + 0x10], edi
004531ac  mov        eax, dword ptr [edi]
004531ae  test       eax, eax
004531b0  je         0x4531c0

// block 004531b2
004531b2  mov        ecx, dword ptr [eax]
004531b4  mov        edx, dword ptr [ecx + 8]
004531b7  push       eax
004531b8  call       edx

// block 004531ba
004531ba  mov        dword ptr [edi], 0

// block 004531c0
004531c0  push       0
004531c2  push       edi
004531c3  push       0
004531c5  call       0x46c6b0

// block 004531ca
004531ca  test       eax, eax
004531cc  jl         0x4533ac

// block 004531d2
004531d2  mov        eax, dword ptr [edi]
004531d4  mov        ecx, dword ptr [eax]
004531d6  mov        edx, dword ptr [ecx + 0x18]
004531d9  push       2
004531db  push       ebx
004531dc  push       eax
004531dd  call       edx

// block 004531df
004531df  test       eax, eax
004531e1  jl         0x4533ac

// block 004531e7
004531e7  push       edi
004531e8  call       0x465690

// block 004531ed
004531ed  mov        eax, dword ptr [esi + 0x10]
004531f0  mov        ecx, dword ptr [eax]
004531f2  xor        eax, eax
004531f4  mov        dword ptr [esp + 0x10], eax
004531f8  mov        dword ptr [esp + 0x1c], eax
004531fc  mov        dword ptr [esi], ecx
004531fe  mov        dword ptr [esp + 0x30], eax
00453202  mov        dword ptr [esp + 0x34], eax
00453206  mov        dword ptr [esp + 0x38], eax
0045320a  mov        dword ptr [esp + 0x40], eax
0045320e  mov        dword ptr [esp + 0x14], eax
00453212  mov        dword ptr [esp + 0x18], eax
00453216  mov        word ptr [esp + 0x20], ax
0045321b  mov        dword ptr [esp + 0x3c], eax
0045321f  mov        dword ptr [esp + 0x44], eax
00453223  mov        dword ptr [esp + 0x48], eax
00453227  mov        dword ptr [esp + 0x4c], eax
0045322b  mov        dword ptr [esp + 0x50], eax
0045322f  xor        edx, edx
00453231  mov        eax, 1
00453236  mov        word ptr [esp + 0x10], ax
0045323b  mov        ecx, 2
00453240  mov        word ptr [esp + 0x12], cx
00453245  mov        word ptr [esp + 0x20], dx
0045324a  push       0
0045324c  mov        eax, 0x10
00453251  lea        ecx, [esp + 0x14]
00453255  mov        word ptr [esp + 0x22], ax
0045325a  mov        eax, dword ptr [esi]
0045325c  mov        dword ptr [esp + 0x44], ecx
00453260  mov        edx, 4
00453265  lea        edi, [esi + 8]
00453268  push       edi
00453269  mov        word ptr [esp + 0x24], dx
0045326e  lea        ecx, [esp + 0x38]
00453272  mov        dword ptr [esi + 0x18], 0
00453279  mov        dword ptr [esp + 0x38], 0x24
00453281  mov        dword ptr [esp + 0x3c], 0x8008
00453289  mov        dword ptr [esp + 0x40], 0x8000
00453291  mov        dword ptr [esp + 0x1c], 0xac44
00453299  mov        dword ptr [esp + 0x20], 0x2b110
004532a1  mov        edx, dword ptr [eax]
004532a3  mov        edx, dword ptr [edx + 0xc]
004532a6  push       ecx
004532a7  push       eax
004532a8  call       edx

// block 004532aa
004532aa  test       eax, eax
004532ac  jl         0x4533e9

// block 004532b2
004532b2  mov        eax, dword ptr [edi]
004532b4  mov        ecx, dword ptr [eax]
004532b6  push       0
004532b8  lea        edx, [esp + 0x2c]
004532bc  push       edx
004532bd  lea        edx, [esp + 0x2c]
004532c1  push       edx
004532c2  lea        edx, [esp + 0x38]
004532c6  push       edx
004532c7  lea        edx, [esp + 0x1c]
004532cb  push       edx
004532cc  push       0x8000
004532d1  push       0
004532d3  push       eax
004532d4  mov        eax, dword ptr [ecx + 0x2c]
004532d7  call       eax

// block 004532d9
004532d9  test       eax, eax
004532db  jl         0x4533e9

// block 004532e1
004532e1  mov        ecx, dword ptr [esp + 0xc]
004532e5  push       0x8000
004532ea  push       0
004532ec  push       ecx
004532ed  call       0x477420

// block 004532f2
004532f2  mov        ecx, dword ptr [esp + 0x34]
004532f6  mov        eax, dword ptr [edi]
004532f8  mov        edx, dword ptr [eax]
004532fa  mov        edx, dword ptr [edx + 0x4c]
004532fd  add        esp, 0xc
00453300  push       ecx
00453301  mov        ecx, dword ptr [esp + 0x28]
00453305  push       ecx
00453306  mov        ecx, dword ptr [esp + 0x34]
0045330a  push       ecx
0045330b  mov        ecx, dword ptr [esp + 0x18]
0045330f  push       ecx
00453310  push       eax
00453311  call       edx

// block 00453313
00453313  mov        edi, dword ptr [edi]
00453315  mov        eax, dword ptr [edi]
00453317  mov        ecx, dword ptr [eax + 0x30]
0045331a  push       1
0045331c  push       0
0045331e  push       0
00453320  push       edi
00453321  call       ecx

// block 00453323
00453323  push       0
00453325  push       0xfa
0045332a  mov        eax, 0x64
0045332f  push       0
00453331  push       ebx
00453332  mov        dword ptr [esi + 0x5294], eax
00453338  mov        dword ptr [esi + 0x5298], eax
0045333e  call       dword ptr [0x498224]

// block 00453344
00453344  mov        dword ptr [esi + 0xc], ebx
00453347  xor        ebx, ebx
00453349  mov        esi, 0x4d0e70
0045334e  mov        edi, 0x4ae5a4

// block 00453353
00453353  cmp        dword ptr [0x4d4770], 2
0045335a  je         0x4533e9

// block 00453360
00453360  mov        edx, dword ptr [edi]
00453362  mov        eax, dword ptr [edx*4 + 0x4aea50]
00453369  push       eax
0045336a  call       0x454580

// block 0045336f
0045336f  test       eax, eax
00453371  jne        0x4533fe

// block 00453377
00453377  add        edi, 0x14
0045337a  inc        ebx
0045337b  add        esi, 0x18
0045337e  cmp        edi, 0x4aea54
00453384  jl         0x453353

// block 00453386
00453386  push       0x4a2e6c
0045338b  mov        ecx, 0x4b0ec8
00453390  call       0x464220

// block 00453395
00453395  add        esp, 4
00453398  pop        edi
00453399  pop        esi
0045339a  xor        eax, eax
0045339c  pop        ebx
0045339d  mov        ecx, dword ptr [esp + 0x48]
004533a1  xor        ecx, esp
004533a3  call       0x46c932

// block 004533a8
004533a8  add        esp, 0x4c
004533ab  ret        

// block 004533ac
004533ac  push       0x4a2e04
004533b1  mov        ecx, 0x4b0ec8
004533b6  call       0x464220

// block 004533bb
004533bb  mov        edi, dword ptr [esi + 0x10]
004533be  add        esp, 4
004533c1  test       edi, edi
004533c3  je         0x4533e9

// block 004533c5
004533c5  mov        eax, dword ptr [edi]
004533c7  test       eax, eax
004533c9  je         0x4533d9

// block 004533cb
004533cb  mov        ecx, dword ptr [eax]
004533cd  mov        edx, dword ptr [ecx + 8]
004533d0  push       eax
004533d1  call       edx

// block 004533d3
004533d3  mov        dword ptr [edi], 0

// block 004533d9
004533d9  push       edi
004533da  call       0x46ca4f

// block 004533df
004533df  add        esp, 4
004533e2  mov        dword ptr [esi + 0x10], 0

// block 004533e9
004533e9  pop        edi
004533ea  pop        esi
004533eb  or         eax, 0xffffffff
004533ee  pop        ebx
004533ef  mov        ecx, dword ptr [esp + 0x48]
004533f3  xor        ecx, esp
004533f5  call       0x46c932

// block 004533fa
004533fa  add        esp, 0x4c
004533fd  ret        

// block 004533fe
004533fe  lea        ecx, [ebx + ebx*4]
00453401  mov        edx, dword ptr [ecx*4 + 0x4ae5a4]
00453408  mov        eax, dword ptr [edx*4 + 0x4aea50]
0045340f  push       eax
00453410  push       0x4a2e34
00453415  mov        ecx, 0x4b0ec8
0045341a  call       0x464220

// block 0045341f
0045341f  mov        ecx, dword ptr [esp + 0x5c]
00453423  add        esp, 8
00453426  pop        edi
00453427  pop        esi
00453428  pop        ebx
00453429  xor        ecx, esp
0045342b  or         eax, 0xffffffff
0045342e  call       0x46c932

// block 00453433
00453433  add        esp, 0x4c
00453436  ret        
