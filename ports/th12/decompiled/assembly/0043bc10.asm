
// block 0043bc10
0043bc10  sub        esp, 0x11c
0043bc16  mov        eax, dword ptr [0x4ad138]
0043bc1b  xor        eax, esp
0043bc1d  mov        dword ptr [esp + 0x118], eax
0043bc24  push       ebx
0043bc25  push       ebp
0043bc26  mov        ebp, dword ptr [0x4b4518]
0043bc2c  push       esi
0043bc2d  push       edi
0043bc2e  xor        ebx, ebx
0043bc30  mov        edi, edx
0043bc32  mov        esi, ecx
0043bc34  mov        ecx, dword ptr [ebp + 0x1c]
0043bc37  mov        dword ptr [esp + 0x24], ebx
0043bc3b  mov        dword ptr [esp + 0x20], ebx
0043bc3f  mov        eax, edi

// block 0043bc41
0043bc41  mov        dl, byte ptr [eax]
0043bc43  mov        byte ptr [ecx], dl
0043bc45  inc        eax
0043bc46  inc        ecx
0043bc47  test       dl, dl
0043bc49  jne        0x43bc41

// block 0043bc4b
0043bc4b  mov        eax, edi
0043bc4d  lea        edx, [eax + 1]

// block 0043bc50
0043bc50  mov        cl, byte ptr [eax]
0043bc52  inc        eax
0043bc53  test       cl, cl
0043bc55  jne        0x43bc50

// block 0043bc57
0043bc57  sub        eax, edx
0043bc59  cmp        eax, 8
0043bc5c  jge        0x43bc6d

// block 0043bc5e
0043bc5e  mov        edi, edi

// block 0043bc60
0043bc60  mov        ecx, dword ptr [ebp + 0x1c]
0043bc63  mov        byte ptr [eax + ecx], 0x20
0043bc67  inc        eax
0043bc68  cmp        eax, 8
0043bc6b  jl         0x43bc60

// block 0043bc6d
0043bc6d  test       byte ptr [ebp + 0x1dc], 1
0043bc74  jne        0x43bce2

// block 0043bc76
0043bc76  cmp        dword ptr [esp + 0x130], ebx
0043bc7d  je         0x43bce2

// block 0043bc7f
0043bc7f  mov        edx, dword ptr [ebp + 0xa0]
0043bc85  mov        eax, dword ptr [edx]
0043bc87  mov        ecx, dword ptr [eax + 0x1518]
0043bc8d  mov        edx, 0xffff
0043bc92  mov        word ptr [ecx], dx
0043bc95  mov        ecx, dword ptr [eax + 0x1518]
0043bc9b  mov        word ptr [ecx + 2], dx
0043bc9f  mov        ecx, dword ptr [eax + 0x1518]
0043bca5  mov        word ptr [ecx + 4], dx
0043bca9  add        dword ptr [eax + 0x1518], 6
0043bcb0  mov        ecx, dword ptr [eax + 0x1518]
0043bcb6  sub        ecx, eax
0043bcb8  mov        eax, 0x2aaaaaab
0043bcbd  imul       ecx
0043bcbf  mov        eax, edx
0043bcc1  shr        eax, 0x1f
0043bcc4  add        eax, edx
0043bcc6  cmp        eax, 0x384
0043bccb  jl         0x43bce2

// block 0043bccd
0043bccd  mov        edi, dword ptr [ebp + 0x1d8]
0043bcd3  mov        ebx, ebp
0043bcd5  call       0x43cbb0

// block 0043bcda
0043bcda  mov        dword ptr [ebp + 0xa0], eax
0043bce0  xor        ebx, ebx

// block 0043bce2
0043bce2  push       0x4a129c
0043bce7  call       0x46d585

// block 0043bcec
0043bcec  push       esi
0043bced  lea        ecx, [esp + 0x30]
0043bcf1  push       0x4a12a4
0043bcf6  push       ecx
0043bcf7  call       0x46cbbb

// block 0043bcfc
0043bcfc  add        esp, 0x10
0043bcff  xor        ecx, ecx
0043bd01  lea        eax, [ebp + 0x44]
0043bd04  mov        dword ptr [esp + 0x1c], ebx
0043bd08  mov        esi, 0x70
0043bd0d  mov        dword ptr [esp + 0x10], ecx
0043bd11  lea        ebx, [ebp + 0x20]
0043bd14  mov        dword ptr [esp + 0x14], eax
0043bd18  jmp        0x43bd20

// block 0043bd20
0043bd20  mov        eax, dword ptr [ebx]
0043bd22  test       eax, eax
0043bd24  je         0x43bdfc

// block 0043bd2a
0043bd2a  cmp        dword ptr [esp + 0x24], 0
0043bd2f  jne        0x43bd35

// block 0043bd31
0043bd31  mov        dword ptr [esp + 0x24], ecx

// block 0043bd35
0043bd35  test       byte ptr [ebp + 0x1dc], 1
0043bd3c  mov        dword ptr [esp + 0x20], ecx
0043bd40  jne        0x43bd49

// block 0043bd42
0043bd42  mov        dword ptr [eax + 8], 0

// block 0043bd49
0043bd49  mov        edx, dword ptr [esp + 0x14]
0043bd4d  mov        edi, dword ptr [edx]
0043bd4f  add        esi, 0xa0
0043bd55  test       edi, edi
0043bd57  je         0x43bdf8

// block 0043bd5d
0043bd5d  lea        ecx, [ecx]

// block 0043bd60
0043bd60  mov        ecx, dword ptr [edi]
0043bd62  mov        edx, dword ptr [ecx + 0x1518]
0043bd68  sub        edx, ecx
0043bd6a  mov        eax, 0x2aaaaaab
0043bd6f  imul       edx
0043bd71  mov        eax, edx
0043bd73  shr        eax, 0x1f
0043bd76  add        eax, edx
0043bd78  mov        edx, dword ptr [ecx + 0x18a0]
0043bd7e  lea        eax, [eax + eax*2]
0043bd81  lea        eax, [edx + eax*2]
0043bd84  sub        eax, ecx
0043bd86  test       byte ptr [ebp + 0x1dc], 1
0043bd8d  lea        edx, [esi + eax - 0x151c]
0043bd94  mov        dword ptr [esp + 0x18], edx
0043bd98  jne        0x43bde5

// block 0043bd9a
0043bd9a  mov        edx, dword ptr [ecx + 0x1518]
0043bda0  mov        esi, dword ptr [ebx]
0043bda2  sub        edx, ecx
0043bda4  mov        eax, 0x2aaaaaab
0043bda9  imul       edx
0043bdab  mov        eax, edx
0043bdad  shr        eax, 0x1f
0043bdb0  add        eax, edx
0043bdb2  mov        edx, dword ptr [ecx + 0x18a0]
0043bdb8  lea        eax, [eax + eax*2]
0043bdbb  lea        eax, [edx + eax*2]
0043bdbe  sub        eax, ecx
0043bdc0  sub        eax, 0x151c
0043bdc5  add        dword ptr [esi + 8], eax
0043bdc8  mov        eax, dword ptr [edi]
0043bdca  mov        ecx, dword ptr [eax + 0x1518]
0043bdd0  mov        esi, dword ptr [ebx]
0043bdd2  sub        ecx, eax
0043bdd4  mov        eax, 0x2aaaaaab
0043bdd9  imul       ecx
0043bddb  mov        ecx, edx
0043bddd  shr        ecx, 0x1f
0043bde0  add        ecx, edx
0043bde2  add        dword ptr [esi + 4], ecx

// block 0043bde5
0043bde5  mov        edi, dword ptr [edi + 4]
0043bde8  mov        esi, dword ptr [esp + 0x18]
0043bdec  test       edi, edi
0043bdee  jne        0x43bd60

// block 0043bdf4
0043bdf4  mov        ecx, dword ptr [esp + 0x10]

// block 0043bdf8
0043bdf8  inc        dword ptr [esp + 0x1c]

// block 0043bdfc
0043bdfc  add        dword ptr [esp + 0x14], 0xc
0043be01  inc        ecx
0043be02  add        ebx, 4
0043be05  cmp        ecx, 8
0043be08  mov        dword ptr [esp + 0x10], ecx
0043be0c  jl         0x43bd20

// block 0043be12
0043be12  mov        edx, dword ptr [ebp + 0x1c]
0043be15  mov        eax, dword ptr [esp + 0x1c]
0043be19  mov        dword ptr [edx + 0x58], eax
0043be1c  mov        ecx, dword ptr [ebp + 0x1c]
0043be1f  mov        edx, dword ptr [0x4b0c44]
0043be25  mov        eax, dword ptr [0x4b43e0]
0043be2a  mov        dword ptr [ecx + 0x14], edx
0043be2d  fld        qword ptr [eax + 0x24]
0043be30  fdiv       qword ptr [eax + 0x2c]
0043be33  mov        eax, dword ptr [ebp + 0x1c]
0043be36  push       esi
0043be37  fstp       dword ptr [esp + 0x14]
0043be3b  fld        dword ptr [esp + 0x14]
0043be3f  fld        qword ptr [0x4a3ce8]
0043be45  fmul       st(1), st(0)
0043be47  fsubrp     st(1)
0043be49  fstp       dword ptr [esp + 0x14]
0043be4d  fld        dword ptr [esp + 0x14]
0043be51  fstp       dword ptr [eax + 0x54]
0043be54  call       0x46d04a

// block 0043be59
0043be59  mov        esi, dword ptr [ebp + 0x1c]
0043be5c  mov        edx, eax
0043be5e  lea        eax, [ebp + 0x44]
0043be61  mov        dword ptr [esp + 0x18], eax
0043be65  mov        ecx, 0x1c
0043be6a  mov        edi, edx
0043be6c  lea        eax, [ebp + 0x20]
0043be6f  add        esp, 4
0043be72  mov        dword ptr [esp + 0x1c], edx

// block 0043be76
0043be76  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0043be78
0043be78  mov        ebx, 0x70
0043be7d  mov        dword ptr [esp + 0x10], eax
0043be81  mov        dword ptr [esp + 0x18], 8
0043be89  lea        esp, [esp]

// block 0043be90
0043be90  mov        esi, dword ptr [eax]
0043be92  test       esi, esi
0043be94  je         0x43bf4c

// block 0043be9a
0043be9a  lea        edi, [edx + ebx]
0043be9d  mov        ecx, 0x28

// block 0043bea2
0043bea2  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0043bea4
0043bea4  mov        ecx, dword ptr [esp + 0x14]
0043bea8  mov        edi, dword ptr [ecx]
0043beaa  add        ebx, 0xa0
0043beb0  test       edi, edi
0043beb2  je         0x43bf05

// block 0043beb4
0043beb4  mov        esi, dword ptr [edi]
0043beb6  mov        ecx, dword ptr [esi + 0x1518]
0043bebc  sub        ecx, esi
0043bebe  mov        eax, 0x2aaaaaab
0043bec3  imul       ecx
0043bec5  mov        eax, edx
0043bec7  shr        eax, 0x1f
0043beca  add        eax, edx
0043becc  lea        edx, [eax + eax*2]
0043becf  mov        eax, dword ptr [esp + 0x1c]
0043bed3  add        edx, edx
0043bed5  push       edx
0043bed6  add        eax, ebx
0043bed8  push       esi
0043bed9  push       eax
0043beda  call       0x47a330

// block 0043bedf
0043bedf  mov        ecx, dword ptr [esi + 0x1518]
0043bee5  mov        edi, dword ptr [edi + 4]
0043bee8  sub        ecx, esi
0043beea  mov        eax, 0x2aaaaaab
0043beef  imul       ecx
0043bef1  mov        eax, edx
0043bef3  shr        eax, 0x1f
0043bef6  add        eax, edx
0043bef8  add        esp, 0xc
0043befb  lea        ecx, [eax + eax*2]
0043befe  lea        ebx, [ebx + ecx*2]
0043bf01  test       edi, edi
0043bf03  jne        0x43beb4

// block 0043bf05
0043bf05  mov        edx, dword ptr [esp + 0x14]
0043bf09  mov        edi, dword ptr [edx]
0043bf0b  test       edi, edi
0043bf0d  je         0x43bf4c

// block 0043bf0f
0043bf0f  nop        

// block 0043bf10
0043bf10  mov        esi, dword ptr [edi]
0043bf12  mov        eax, dword ptr [esi + 0x18a0]
0043bf18  mov        edx, dword ptr [esp + 0x1c]
0043bf1c  sub        eax, esi
0043bf1e  sub        eax, 0x151c
0043bf23  push       eax
0043bf24  lea        ecx, [esi + 0x151c]
0043bf2a  push       ecx
0043bf2b  add        edx, ebx
0043bf2d  push       edx
0043bf2e  call       0x47a330

// block 0043bf33
0043bf33  mov        eax, dword ptr [esi + 0x18a0]
0043bf39  mov        edi, dword ptr [edi + 4]
0043bf3c  sub        eax, esi
0043bf3e  add        esp, 0xc
0043bf41  lea        ebx, [ebx + eax - 0x151c]
0043bf48  test       edi, edi
0043bf4a  jne        0x43bf10

// block 0043bf4c
0043bf4c  mov        eax, dword ptr [esp + 0x10]
0043bf50  add        dword ptr [esp + 0x14], 0xc
0043bf55  mov        edx, dword ptr [esp + 0x1c]
0043bf59  add        eax, 4
0043bf5c  sub        dword ptr [esp + 0x18], 1
0043bf61  mov        dword ptr [esp + 0x10], eax
0043bf65  jne        0x43be90

// block 0043bf6b
0043bf6b  lea        ecx, [esp + 0x18]
0043bf6f  push       ecx
0043bf70  push       ebx
0043bf71  push       edx
0043bf72  call       0x44c3f0

// block 0043bf77
0043bf77  mov        esi, eax
0043bf79  mov        eax, dword ptr [esp + 0x1c]
0043bf7d  push       eax
0043bf7e  mov        dword ptr [esp + 0x14], esi
0043bf82  call       0x46c941

// block 0043bf87
0043bf87  mov        edi, dword ptr [esp + 0x1c]
0043bf8b  add        esp, 4
0043bf8e  push       edi
0043bf8f  push       0x40
0043bf91  push       0x3a
0043bf93  push       edi
0043bf94  push       esi
0043bf95  mov        al, 0x7d
0043bf97  call       0x463af0

// block 0043bf9c
0043bf9c  push       edi
0043bf9d  push       0x800
0043bfa2  push       0xe1
0043bfa7  push       edi
0043bfa8  push       esi
0043bfa9  mov        al, 0x5e
0043bfab  call       0x463af0

// block 0043bfb0
0043bfb0  mov        ecx, dword ptr [ebp + 0x18]
0043bfb3  mov        dword ptr [ecx + 0x20], ebx
0043bfb6  mov        edx, dword ptr [ebp + 0x18]
0043bfb9  mov        dword ptr [edx + 0x1c], edi
0043bfbc  mov        eax, dword ptr [ebp + 0x18]
0043bfbf  mov        ecx, dword ptr [eax + 0x1c]
0043bfc2  add        ecx, 0x24
0043bfc5  lea        esi, [esp + 0x28]
0043bfc9  mov        dword ptr [eax + 0xc], ecx
0043bfcc  call       0x463fb0

// block 0043bfd1
0043bfd1  mov        eax, dword ptr [0x4ae590]
0043bfd6  cmp        eax, -1
0043bfd9  je         0x43c06f

// block 0043bfdf
0043bfdf  mov        ecx, dword ptr [ebp + 0x18]
0043bfe2  mov        esi, dword ptr [0x4980ac]
0043bfe8  push       0
0043bfea  lea        edx, [esp + 0x1c]
0043bfee  push       edx
0043bfef  push       0x24
0043bff1  push       ecx
0043bff2  push       eax
0043bff3  call       esi

// block 0043bff5
0043bff5  cmp        dword ptr [esp + 0x18], 0x24
0043bffa  je         0x43c026

// block 0043bffc
0043bffc  mov        edx, dword ptr [0x4ae590]
0043c002  push       edx
0043c003  call       dword ptr [0x4980a4]

// block 0043c009
0043c009  test       dword ptr [0x4cee78], 0x8000
0043c013  je         0x43c026

// block 0043c015
0043c015  push       0x4cf128
0043c01a  call       dword ptr [0x49808c]

// block 0043c020
0043c020  dec        byte ptr [0x4cf21a]

// block 0043c026
0043c026  mov        eax, dword ptr [0x4ae590]
0043c02b  cmp        eax, -1
0043c02e  je         0x43c06f

// block 0043c030
0043c030  mov        edx, dword ptr [esp + 0x10]
0043c034  push       0
0043c036  lea        ecx, [esp + 0x18]
0043c03a  push       ecx
0043c03b  push       edi
0043c03c  push       edx
0043c03d  push       eax
0043c03e  call       esi

// block 0043c040
0043c040  cmp        edi, dword ptr [esp + 0x14]
0043c044  je         0x43c06f

// block 0043c046
0043c046  mov        eax, dword ptr [0x4ae590]
0043c04b  push       eax
0043c04c  call       dword ptr [0x4980a4]

// block 0043c052
0043c052  test       dword ptr [0x4cee78], 0x8000
0043c05c  je         0x43c06f

// block 0043c05e
0043c05e  push       0x4cf128
0043c063  call       dword ptr [0x49808c]

// block 0043c069
0043c069  dec        byte ptr [0x4cf21a]

// block 0043c06f
0043c06f  mov        eax, dword ptr [esp + 0x10]
0043c073  test       eax, eax
0043c075  je         0x43c080

// block 0043c077
0043c077  push       eax
0043c078  call       0x46c941

// block 0043c07d
0043c07d  add        esp, 4

// block 0043c080
0043c080  push       0xffff
0043c085  call       0x46d04a

// block 0043c08a
0043c08a  push       0xffff
0043c08f  mov        edi, eax
0043c091  push       0
0043c093  push       edi
0043c094  call       0x477420

// block 0043c099
0043c099  lea        ebx, [edi + 0xc]
0043c09c  mov        esi, ebx
0043c09e  push       0x4a12f0
0043c0a3  push       esi
0043c0a4  mov        dword ptr [edi], 0x52455355
0043c0aa  mov        byte ptr [edi + 8], 0
0043c0ae  call       0x46cbbb

// block 0043c0b3
0043c0b3  push       0x4a1314
0043c0b8  add        esi, eax
0043c0ba  push       0x4a131c
0043c0bf  push       esi
0043c0c0  call       0x46cbbb

// block 0043c0c5
0043c0c5  mov        ecx, dword ptr [ebp + 0x1c]
0043c0c8  push       ecx
0043c0c9  add        esi, eax
0043c0cb  push       0x4a132c
0043c0d0  push       esi
0043c0d1  call       0x46cbbb

// block 0043c0d6
0043c0d6  add        esi, eax
0043c0d8  mov        eax, dword ptr [ebp + 0x1c]
0043c0db  add        eax, 0xc
0043c0de  push       eax
0043c0df  call       0x46d8e4

// block 0043c0e4
0043c0e4  mov        edx, dword ptr [eax + 4]
0043c0e7  mov        ecx, dword ptr [eax + 8]
0043c0ea  push       edx
0043c0eb  mov        edx, dword ptr [eax + 0xc]
0043c0ee  push       ecx
0043c0ef  mov        ecx, dword ptr [eax + 0x10]
0043c0f2  mov        eax, dword ptr [eax + 0x14]
0043c0f5  push       edx
0043c0f6  inc        ecx
0043c0f7  push       ecx
0043c0f8  cdq        
0043c0f9  mov        ecx, 0x64
0043c0fe  idiv       ecx
0043c100  push       edx
0043c101  push       0x4a1338
0043c106  push       esi
0043c107  call       0x46cbbb

// block 0043c10c
0043c10c  add        esi, eax
0043c10e  mov        eax, dword ptr [ebp + 0x1c]
0043c111  mov        edx, dword ptr [eax + 0x5c]
0043c114  mov        eax, dword ptr [eax + 0x60]
0043c117  lea        ecx, [eax + edx*2]
0043c11a  mov        edx, dword ptr [ecx*4 + 0x4aee20]
0043c121  add        esp, 0x50
0043c124  push       edx
0043c125  push       0x4a1358
0043c12a  push       esi
0043c12b  call       0x46cbbb

// block 0043c130
0043c130  add        esi, eax
0043c132  mov        eax, dword ptr [ebp + 0x1c]
0043c135  mov        ecx, dword ptr [eax + 0x64]
0043c138  mov        edx, dword ptr [ecx*4 + 0x4aee38]
0043c13f  push       edx
0043c140  push       0x4a1364
0043c145  push       esi
0043c146  call       0x46cbbb

// block 0043c14b
0043c14b  add        esi, eax
0043c14d  mov        eax, dword ptr [ebp + 0x1c]
0043c150  mov        ecx, 7
0043c155  add        esp, 0x18
0043c158  cmp        dword ptr [eax + 0x68], ecx
0043c15b  jle        0x43c183

// block 0043c15d
0043c15d  cmp        dword ptr [esp + 0x24], ecx
0043c161  jne        0x43c173

// block 0043c163
0043c163  push       0x4a1370
0043c168  push       esi
0043c169  call       0x46cbbb

// block 0043c16e
0043c16e  add        esp, 8
0043c171  jmp        0x43c1c4

// block 0043c173
0043c173  push       0x4a1384
0043c178  push       esi
0043c179  call       0x46cbbb

// block 0043c17e
0043c17e  add        esp, 8
0043c181  jmp        0x43c1c4

// block 0043c183
0043c183  mov        eax, dword ptr [esp + 0x24]
0043c187  mov        edx, dword ptr [esp + 0x20]
0043c18b  cmp        eax, edx
0043c18d  jne        0x43c1b4

// block 0043c18f
0043c18f  cmp        eax, ecx
0043c191  jne        0x43c1a3

// block 0043c193
0043c193  push       0x4a1398
0043c198  push       esi
0043c199  call       0x46cbbb

// block 0043c19e
0043c19e  add        esp, 8
0043c1a1  jmp        0x43c1c4

// block 0043c1a3
0043c1a3  push       eax
0043c1a4  push       0x4a13a8
0043c1a9  push       esi
0043c1aa  call       0x46cbbb

// block 0043c1af
0043c1af  add        esp, 0xc
0043c1b2  jmp        0x43c1c4

// block 0043c1b4
0043c1b4  push       edx
0043c1b5  push       eax
0043c1b6  push       0x4a13b4
0043c1bb  push       esi
0043c1bc  call       0x46cbbb

// block 0043c1c1
0043c1c1  add        esp, 0x10

// block 0043c1c4
0043c1c4  mov        ecx, dword ptr [ebp + 0x1c]
0043c1c7  mov        edx, dword ptr [ecx + 0x14]
0043c1ca  push       edx
0043c1cb  add        esi, eax
0043c1cd  push       0x4a13c8
0043c1d2  push       esi
0043c1d3  call       0x46cbbb

// block 0043c1d8
0043c1d8  add        esi, eax
0043c1da  mov        eax, dword ptr [ebp + 0x1c]
0043c1dd  fld        dword ptr [eax + 0x54]
0043c1e0  add        esp, 4
0043c1e3  fstp       qword ptr [esp]
0043c1e6  push       0x4a13d4
0043c1eb  push       esi
0043c1ec  call       0x46cbbb

// block 0043c1f1
0043c1f1  lea        esi, [esi + eax + 1]
0043c1f5  mov        eax, esi
0043c1f7  sub        eax, edi
0043c1f9  add        esp, 0x10
0043c1fc  and        eax, 0x80000003
0043c201  jns        0x43c208

// block 0043c203
0043c203  dec        eax
0043c204  or         eax, 0xfffffffc
0043c207  inc        eax

// block 0043c208
0043c208  je         0x43c213

// block 0043c20a
0043c20a  mov        ecx, 4
0043c20f  sub        ecx, eax
0043c211  add        esi, ecx

// block 0043c213
0043c213  mov        eax, dword ptr [0x4ae590]
0043c218  sub        esi, edi
0043c21a  mov        dword ptr [edi + 4], esi
0043c21d  cmp        eax, -1
0043c220  je         0x43c261

// block 0043c222
0043c222  push       0
0043c224  lea        edx, [esp + 0x24]
0043c228  push       edx
0043c229  push       esi
0043c22a  push       edi
0043c22b  push       eax
0043c22c  call       dword ptr [0x4980ac]

// block 0043c232
0043c232  cmp        esi, dword ptr [esp + 0x20]
0043c236  je         0x43c261

// block 0043c238
0043c238  mov        eax, dword ptr [0x4ae590]
0043c23d  push       eax
0043c23e  call       dword ptr [0x4980a4]

// block 0043c244
0043c244  test       dword ptr [0x4cee78], 0x8000
0043c24e  je         0x43c261

// block 0043c250
0043c250  push       0x4cf128
0043c255  call       dword ptr [0x49808c]

// block 0043c25b
0043c25b  dec        byte ptr [0x4cf21a]

// block 0043c261
0043c261  push       0xffff
0043c266  push       0
0043c268  push       edi
0043c269  call       0x477420

// block 0043c26e
0043c26e  push       0x4a13e8
0043c273  push       ebx
0043c274  mov        dword ptr [edi], 0x52455355
0043c27a  mov        byte ptr [edi + 8], 1
0043c27e  call       0x46cbbb

// block 0043c283
0043c283  lea        esi, [ebx + eax + 1]
0043c287  mov        eax, esi
0043c289  sub        eax, edi
0043c28b  add        esp, 0x14
0043c28e  and        eax, 0x80000003
0043c293  jns        0x43c29a

// block 0043c295
0043c295  dec        eax
0043c296  or         eax, 0xfffffffc
0043c299  inc        eax

// block 0043c29a
0043c29a  je         0x43c2a5

// block 0043c29c
0043c29c  mov        ecx, 4
0043c2a1  sub        ecx, eax
0043c2a3  add        esi, ecx

// block 0043c2a5
0043c2a5  mov        ebx, dword ptr [0x4ae590]
0043c2ab  sub        esi, edi
0043c2ad  mov        dword ptr [edi + 4], esi
0043c2b0  cmp        ebx, -1
0043c2b3  je         0x43c2fa

// block 0043c2b5
0043c2b5  push       0
0043c2b7  lea        edx, [esp + 0x24]
0043c2bb  push       edx
0043c2bc  push       esi
0043c2bd  push       edi
0043c2be  push       ebx
0043c2bf  call       dword ptr [0x4980ac]

// block 0043c2c5
0043c2c5  cmp        esi, dword ptr [esp + 0x20]
0043c2c9  je         0x43c2f4

// block 0043c2cb
0043c2cb  mov        eax, dword ptr [0x4ae590]
0043c2d0  push       eax
0043c2d1  call       dword ptr [0x4980a4]

// block 0043c2d7
0043c2d7  test       dword ptr [0x4cee78], 0x8000
0043c2e1  je         0x43c2f4

// block 0043c2e3
0043c2e3  push       0x4cf128
0043c2e8  call       dword ptr [0x49808c]

// block 0043c2ee
0043c2ee  dec        byte ptr [0x4cf21a]

// block 0043c2f4
0043c2f4  mov        ebx, dword ptr [0x4ae590]

// block 0043c2fa
0043c2fa  push       edi
0043c2fb  call       0x46c941

// block 0043c300
0043c300  add        esp, 4
0043c303  cmp        ebx, -1
0043c306  je         0x43c32c

// block 0043c308
0043c308  push       ebx
0043c309  call       dword ptr [0x4980a4]

// block 0043c30f
0043c30f  test       dword ptr [0x4cee78], 0x8000
0043c319  je         0x43c32c

// block 0043c31b
0043c31b  push       0x4cf128
0043c320  call       dword ptr [0x49808c]

// block 0043c326
0043c326  dec        byte ptr [0x4cf21a]

// block 0043c32c
0043c32c  or         dword ptr [ebp + 0x1dc], 1
0043c333  mov        ecx, dword ptr [esp + 0x128]
0043c33a  pop        edi
0043c33b  pop        esi
0043c33c  pop        ebp
0043c33d  pop        ebx
0043c33e  xor        ecx, esp
0043c340  xor        eax, eax
0043c342  call       0x46c932

// block 0043c347
0043c347  add        esp, 0x11c
0043c34d  ret        4
