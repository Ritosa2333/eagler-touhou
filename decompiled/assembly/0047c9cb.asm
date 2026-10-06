
// block 0047c9cb
0047c9cb  mov        edi, edi
0047c9cd  push       ebp
0047c9ce  mov        ebp, esp
0047c9d0  sub        esp, 0x278
0047c9d6  mov        eax, dword ptr [0x4ad138]
0047c9db  xor        eax, ebp
0047c9dd  mov        dword ptr [ebp - 4], eax
0047c9e0  push       ebx
0047c9e1  mov        ebx, dword ptr [ebp + 0xc]
0047c9e4  push       esi
0047c9e5  mov        esi, dword ptr [ebp + 8]
0047c9e8  xor        eax, eax
0047c9ea  push       edi
0047c9eb  mov        edi, dword ptr [ebp + 0x14]
0047c9ee  push       dword ptr [ebp + 0x10]
0047c9f1  lea        ecx, [ebp - 0x250]
0047c9f7  mov        dword ptr [ebp - 0x260], esi
0047c9fd  mov        dword ptr [ebp - 0x224], edi
0047ca03  mov        dword ptr [ebp - 0x25c], eax
0047ca09  mov        dword ptr [ebp - 0x210], eax
0047ca0f  mov        dword ptr [ebp - 0x234], eax
0047ca15  mov        dword ptr [ebp - 0x218], eax
0047ca1b  mov        dword ptr [ebp - 0x230], eax
0047ca21  mov        dword ptr [ebp - 0x258], eax
0047ca27  mov        dword ptr [ebp - 0x238], eax
0047ca2d  call       0x46d3b4

// block 0047ca32
0047ca32  test       esi, esi
0047ca34  jne        0x47ca6b

// block 0047ca36
0047ca36  call       0x46e96f

// block 0047ca3b
0047ca3b  mov        dword ptr [eax], 0x16
0047ca41  xor        eax, eax
0047ca43  push       eax
0047ca44  push       eax
0047ca45  push       eax
0047ca46  push       eax
0047ca47  push       eax

// block 0047ca48
0047ca48  call       0x470f2d

// block 0047ca4d
0047ca4d  add        esp, 0x14
0047ca50  cmp        byte ptr [ebp - 0x244], 0
0047ca57  je         0x47ca63

// block 0047ca59
0047ca59  mov        eax, dword ptr [ebp - 0x248]
0047ca5f  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0047ca63
0047ca63  or         eax, 0xffffffff
0047ca66  jmp        0x47d56a

// block 0047ca6b
0047ca6b  test       byte ptr [esi + 0xc], 0x40
0047ca6f  jne        0x47cacf

// block 0047ca71
0047ca71  push       esi
0047ca72  call       0x47c1da

// block 0047ca77
0047ca77  pop        ecx
0047ca78  mov        edx, 0x4adbb0
0047ca7d  cmp        eax, -1
0047ca80  je         0x47ca9d

// block 0047ca82
0047ca82  cmp        eax, -2
0047ca85  je         0x47ca9d

// block 0047ca87
0047ca87  mov        ecx, eax
0047ca89  and        ecx, 0x1f
0047ca8c  mov        esi, eax
0047ca8e  sar        esi, 5
0047ca91  shl        ecx, 6
0047ca94  add        ecx, dword ptr [esi*4 + 0x4d6320]
0047ca9b  jmp        0x47ca9f

// block 0047ca9d
0047ca9d  mov        ecx, edx

// block 0047ca9f
0047ca9f  test       byte ptr [ecx + 0x24], 0x7f
0047caa3  jne        0x47ca36

// block 0047caa5
0047caa5  cmp        eax, -1
0047caa8  je         0x47cac3

// block 0047caaa
0047caaa  cmp        eax, -2
0047caad  je         0x47cac3

// block 0047caaf
0047caaf  mov        ecx, eax
0047cab1  and        eax, 0x1f
0047cab4  sar        ecx, 5
0047cab7  shl        eax, 6
0047caba  add        eax, dword ptr [ecx*4 + 0x4d6320]
0047cac1  jmp        0x47cac5

// block 0047cac3
0047cac3  mov        eax, edx

// block 0047cac5
0047cac5  test       byte ptr [eax + 0x24], 0x80
0047cac9  jne        0x47ca36

// block 0047cacf
0047cacf  xor        eax, eax
0047cad1  cmp        ebx, eax
0047cad3  je         0x47ca36

// block 0047cad9
0047cad9  mov        dl, byte ptr [ebx]
0047cadb  mov        dword ptr [ebp - 0x228], eax
0047cae1  mov        dword ptr [ebp - 0x220], eax
0047cae7  mov        dword ptr [ebp - 0x240], eax
0047caed  mov        dword ptr [ebp - 0x254], eax
0047caf3  mov        byte ptr [ebp - 0x211], dl
0047caf9  test       dl, dl
0047cafb  je         0x47d551

// block 0047cb01
0047cb01  inc        ebx
0047cb02  xor        eax, eax
0047cb04  cmp        dword ptr [ebp - 0x228], eax
0047cb0a  mov        dword ptr [ebp - 0x23c], ebx
0047cb10  jl         0x47d529

// block 0047cb16
0047cb16  mov        cl, dl
0047cb18  sub        cl, 0x20
0047cb1b  cmp        cl, 0x58
0047cb1e  ja         0x47cb2d

// block 0047cb20
0047cb20  movsx      eax, dl
0047cb23  movzx      eax, byte ptr [eax + 0x49d620]
0047cb2a  and        eax, 0xf

// block 0047cb2d
0047cb2d  mov        ecx, dword ptr [ebp - 0x240]
0047cb33  imul       eax, eax, 9
0047cb36  movzx      eax, byte ptr [eax + ecx + 0x49d640]
0047cb3e  push       8
0047cb40  shr        eax, 4
0047cb43  pop        esi
0047cb44  mov        dword ptr [ebp - 0x240], eax
0047cb4a  cmp        eax, esi
0047cb4c  je         0x47ca36

// block 0047cb52
0047cb52  push       7
0047cb54  pop        ecx
0047cb55  cmp        eax, ecx
0047cb57  ja         0x47d50a

// block 0047cb5d
0047cb5d  jmp        dword ptr [eax*4 + 0x47d57b]

// block 0047cb64
0047cb64  xor        eax, eax
0047cb66  or         dword ptr [ebp - 0x218], 0xffffffff
0047cb6d  mov        dword ptr [ebp - 0x26c], eax
0047cb73  mov        dword ptr [ebp - 0x258], eax
0047cb79  mov        dword ptr [ebp - 0x234], eax
0047cb7f  mov        dword ptr [ebp - 0x230], eax
0047cb85  mov        dword ptr [ebp - 0x210], eax
0047cb8b  mov        dword ptr [ebp - 0x238], eax
0047cb91  jmp        0x47d50a

// block 0047cb96
0047cb96  movsx      eax, dl
0047cb99  sub        eax, 0x20
0047cb9c  je         0x47cbe6

// block 0047cb9e
0047cb9e  sub        eax, 3
0047cba1  je         0x47cbd7

// block 0047cba3
0047cba3  sub        eax, esi
0047cba5  je         0x47cbcb

// block 0047cba7
0047cba7  dec        eax
0047cba8  dec        eax
0047cba9  je         0x47cbbf

// block 0047cbab
0047cbab  sub        eax, 3
0047cbae  jne        0x47d50a

// block 0047cbb4
0047cbb4  or         dword ptr [ebp - 0x210], esi
0047cbba  jmp        0x47d50a

// block 0047cbbf
0047cbbf  or         dword ptr [ebp - 0x210], 4
0047cbc6  jmp        0x47d50a

// block 0047cbcb
0047cbcb  or         dword ptr [ebp - 0x210], 1
0047cbd2  jmp        0x47d50a

// block 0047cbd7
0047cbd7  or         dword ptr [ebp - 0x210], 0x80
0047cbe1  jmp        0x47d50a

// block 0047cbe6
0047cbe6  or         dword ptr [ebp - 0x210], 2
0047cbed  jmp        0x47d50a

// block 0047cbf2
0047cbf2  cmp        dl, 0x2a
0047cbf5  jne        0x47cc23

// block 0047cbf7
0047cbf7  add        edi, 4
0047cbfa  mov        dword ptr [ebp - 0x224], edi
0047cc00  mov        edi, dword ptr [edi - 4]
0047cc03  mov        dword ptr [ebp - 0x234], edi
0047cc09  test       edi, edi
0047cc0b  jge        0x47d50a

// block 0047cc11
0047cc11  or         dword ptr [ebp - 0x210], 4
0047cc18  neg        dword ptr [ebp - 0x234]
0047cc1e  jmp        0x47d50a

// block 0047cc23
0047cc23  mov        eax, dword ptr [ebp - 0x234]
0047cc29  imul       eax, eax, 0xa
0047cc2c  movsx      ecx, dl
0047cc2f  lea        eax, [eax + ecx - 0x30]
0047cc33  mov        dword ptr [ebp - 0x234], eax
0047cc39  jmp        0x47d50a

// block 0047cc3e
0047cc3e  and        dword ptr [ebp - 0x218], 0
0047cc45  jmp        0x47d50a

// block 0047cc4a
0047cc4a  cmp        dl, 0x2a
0047cc4d  jne        0x47cc75

// block 0047cc4f
0047cc4f  add        edi, 4
0047cc52  mov        dword ptr [ebp - 0x224], edi
0047cc58  mov        edi, dword ptr [edi - 4]
0047cc5b  mov        dword ptr [ebp - 0x218], edi
0047cc61  test       edi, edi
0047cc63  jge        0x47d50a

// block 0047cc69
0047cc69  or         dword ptr [ebp - 0x218], 0xffffffff
0047cc70  jmp        0x47d50a

// block 0047cc75
0047cc75  mov        eax, dword ptr [ebp - 0x218]
0047cc7b  imul       eax, eax, 0xa
0047cc7e  movsx      ecx, dl
0047cc81  lea        eax, [eax + ecx - 0x30]
0047cc85  mov        dword ptr [ebp - 0x218], eax
0047cc8b  jmp        0x47d50a

// block 0047cc90
0047cc90  cmp        dl, 0x49
0047cc93  je         0x47ccea

// block 0047cc95
0047cc95  cmp        dl, 0x68
0047cc98  je         0x47ccde

// block 0047cc9a
0047cc9a  cmp        dl, 0x6c
0047cc9d  je         0x47ccb7

// block 0047cc9f
0047cc9f  cmp        dl, 0x77
0047cca2  jne        0x47d50a

// block 0047cca8
0047cca8  or         dword ptr [ebp - 0x210], 0x800
0047ccb2  jmp        0x47d50a

// block 0047ccb7
0047ccb7  cmp        byte ptr [ebx], 0x6c
0047ccba  jne        0x47ccd2

// block 0047ccbc
0047ccbc  inc        ebx
0047ccbd  or         dword ptr [ebp - 0x210], 0x1000
0047ccc7  mov        dword ptr [ebp - 0x23c], ebx
0047cccd  jmp        0x47d50a

// block 0047ccd2
0047ccd2  or         dword ptr [ebp - 0x210], 0x10
0047ccd9  jmp        0x47d50a

// block 0047ccde
0047ccde  or         dword ptr [ebp - 0x210], 0x20
0047cce5  jmp        0x47d50a

// block 0047ccea
0047ccea  mov        al, byte ptr [ebx]
0047ccec  cmp        al, 0x36
0047ccee  jne        0x47cd0d

// block 0047ccf0
0047ccf0  cmp        byte ptr [ebx + 1], 0x34
0047ccf4  jne        0x47cd0d

// block 0047ccf6
0047ccf6  inc        ebx
0047ccf7  inc        ebx
0047ccf8  or         dword ptr [ebp - 0x210], 0x8000
0047cd02  mov        dword ptr [ebp - 0x23c], ebx
0047cd08  jmp        0x47d50a

// block 0047cd0d
0047cd0d  cmp        al, 0x33
0047cd0f  jne        0x47cd2e

// block 0047cd11
0047cd11  cmp        byte ptr [ebx + 1], 0x32
0047cd15  jne        0x47cd2e

// block 0047cd17
0047cd17  inc        ebx
0047cd18  inc        ebx
0047cd19  and        dword ptr [ebp - 0x210], 0xffff7fff
0047cd23  mov        dword ptr [ebp - 0x23c], ebx
0047cd29  jmp        0x47d50a

// block 0047cd2e
0047cd2e  cmp        al, 0x64
0047cd30  je         0x47d50a

// block 0047cd36
0047cd36  cmp        al, 0x69
0047cd38  je         0x47d50a

// block 0047cd3e
0047cd3e  cmp        al, 0x6f
0047cd40  je         0x47d50a

// block 0047cd46
0047cd46  cmp        al, 0x75
0047cd48  je         0x47d50a

// block 0047cd4e
0047cd4e  cmp        al, 0x78
0047cd50  je         0x47d50a

// block 0047cd56
0047cd56  cmp        al, 0x58
0047cd58  je         0x47d50a

// block 0047cd5e
0047cd5e  and        dword ptr [ebp - 0x240], 0

// block 0047cd65
0047cd65  and        dword ptr [ebp - 0x238], 0
0047cd6c  lea        eax, [ebp - 0x250]
0047cd72  push       eax
0047cd73  movzx      eax, dl
0047cd76  push       eax
0047cd77  call       0x47c5a3

// block 0047cd7c
0047cd7c  pop        ecx
0047cd7d  test       eax, eax
0047cd7f  mov        al, byte ptr [ebp - 0x211]
0047cd85  pop        ecx
0047cd86  je         0x47cdaa

// block 0047cd88
0047cd88  mov        ecx, dword ptr [ebp - 0x260]
0047cd8e  lea        esi, [ebp - 0x228]
0047cd94  call       0x47c925

// block 0047cd99
0047cd99  mov        al, byte ptr [ebx]
0047cd9b  inc        ebx
0047cd9c  mov        dword ptr [ebp - 0x23c], ebx
0047cda2  test       al, al
0047cda4  je         0x47ca36

// block 0047cdaa
0047cdaa  mov        ecx, dword ptr [ebp - 0x260]
0047cdb0  lea        esi, [ebp - 0x228]
0047cdb6  call       0x47c925

// block 0047cdbb
0047cdbb  jmp        0x47d50a

// block 0047cdc0
0047cdc0  movsx      eax, dl
0047cdc3  cmp        eax, 0x64
0047cdc6  jg         0x47cfb6

// block 0047cdcc
0047cdcc  je         0x47d049

// block 0047cdd2
0047cdd2  cmp        eax, 0x53
0047cdd5  jg         0x47cece

// block 0047cddb
0047cddb  je         0x47ce62

// block 0047cde1
0047cde1  sub        eax, 0x41
0047cde4  je         0x47cdf6

// block 0047cde6
0047cde6  dec        eax
0047cde7  dec        eax
0047cde8  je         0x47ce43

// block 0047cdea
0047cdea  dec        eax
0047cdeb  dec        eax
0047cdec  je         0x47cdf6

// block 0047cdee
0047cdee  dec        eax
0047cdef  dec        eax
0047cdf0  jne        0x47d385

// block 0047cdf6
0047cdf6  add        dl, 0x20
0047cdf9  mov        dword ptr [ebp - 0x26c], 1
0047ce03  mov        byte ptr [ebp - 0x211], dl

// block 0047ce09
0047ce09  or         dword ptr [ebp - 0x210], 0x40
0047ce10  cmp        dword ptr [ebp - 0x218], 0
0047ce17  lea        ebx, [ebp - 0x20c]
0047ce1d  mov        eax, 0x200
0047ce22  mov        dword ptr [ebp - 0x21c], ebx
0047ce28  mov        dword ptr [ebp - 0x264], eax
0047ce2e  jge        0x47d079

// block 0047ce34
0047ce34  mov        dword ptr [ebp - 0x218], 6
0047ce3e  jmp        0x47d0e6

// block 0047ce43
0047ce43  test       dword ptr [ebp - 0x210], 0x830
0047ce4d  jne        0x47ceeb

// block 0047ce53
0047ce53  or         dword ptr [ebp - 0x210], 0x800
0047ce5d  jmp        0x47ceeb

// block 0047ce62
0047ce62  test       dword ptr [ebp - 0x210], 0x830
0047ce6c  jne        0x47ce78

// block 0047ce6e
0047ce6e  or         dword ptr [ebp - 0x210], 0x800

// block 0047ce78
0047ce78  mov        ecx, dword ptr [ebp - 0x218]
0047ce7e  cmp        ecx, -1
0047ce81  jne        0x47ce88

// block 0047ce83
0047ce83  mov        ecx, 0x7fffffff

// block 0047ce88
0047ce88  add        edi, 4
0047ce8b  test       dword ptr [ebp - 0x210], 0x810
0047ce95  mov        dword ptr [ebp - 0x224], edi
0047ce9b  mov        edi, dword ptr [edi - 4]
0047ce9e  mov        dword ptr [ebp - 0x21c], edi
0047cea4  je         0x47d357

// block 0047ceaa
0047ceaa  test       edi, edi
0047ceac  jne        0x47ceb9

// block 0047ceae
0047ceae  mov        eax, dword ptr [0x4ad3dc]
0047ceb3  mov        dword ptr [ebp - 0x21c], eax

// block 0047ceb9
0047ceb9  mov        eax, dword ptr [ebp - 0x21c]
0047cebf  mov        dword ptr [ebp - 0x238], 1
0047cec9  jmp        0x47d349

// block 0047cece
0047cece  sub        eax, 0x58
0047ced1  je         0x47d1ac

// block 0047ced7
0047ced7  dec        eax
0047ced8  dec        eax
0047ced9  je         0x47cf54

// block 0047cedb
0047cedb  sub        eax, ecx
0047cedd  je         0x47ce09

// block 0047cee3
0047cee3  dec        eax
0047cee4  dec        eax
0047cee5  jne        0x47d385

// block 0047ceeb
0047ceeb  add        edi, 4
0047ceee  test       dword ptr [ebp - 0x210], 0x810
0047cef8  mov        dword ptr [ebp - 0x224], edi
0047cefe  je         0x47cf30

// block 0047cf00
0047cf00  movzx      eax, word ptr [edi - 4]
0047cf04  push       eax
0047cf05  push       0x200
0047cf0a  lea        eax, [ebp - 0x20c]
0047cf10  push       eax
0047cf11  lea        eax, [ebp - 0x220]
0047cf17  push       eax
0047cf18  call       0x47c503

// block 0047cf1d
0047cf1d  add        esp, 0x10
0047cf20  test       eax, eax
0047cf22  je         0x47cf43

// block 0047cf24
0047cf24  mov        dword ptr [ebp - 0x258], 1
0047cf2e  jmp        0x47cf43

// block 0047cf30
0047cf30  mov        al, byte ptr [edi - 4]
0047cf33  mov        byte ptr [ebp - 0x20c], al
0047cf39  mov        dword ptr [ebp - 0x220], 1

// block 0047cf43
0047cf43  lea        eax, [ebp - 0x20c]
0047cf49  mov        dword ptr [ebp - 0x21c], eax
0047cf4f  jmp        0x47d385

// block 0047cf54
0047cf54  mov        eax, dword ptr [edi]
0047cf56  add        edi, 4
0047cf59  mov        dword ptr [ebp - 0x224], edi
0047cf5f  test       eax, eax
0047cf61  je         0x47cf9f

// block 0047cf63
0047cf63  mov        ecx, dword ptr [eax + 4]
0047cf66  test       ecx, ecx
0047cf68  je         0x47cf9f

// block 0047cf6a
0047cf6a  test       dword ptr [ebp - 0x210], 0x800
0047cf74  movsx      eax, word ptr [eax]
0047cf77  mov        dword ptr [ebp - 0x21c], ecx
0047cf7d  je         0x47cf93

// block 0047cf7f
0047cf7f  cdq        
0047cf80  sub        eax, edx
0047cf82  sar        eax, 1
0047cf84  mov        dword ptr [ebp - 0x238], 1
0047cf8e  jmp        0x47d37f

// block 0047cf93
0047cf93  and        dword ptr [ebp - 0x238], 0
0047cf9a  jmp        0x47d37f

// block 0047cf9f
0047cf9f  mov        eax, dword ptr [0x4ad3d8]
0047cfa4  mov        dword ptr [ebp - 0x21c], eax
0047cfaa  push       eax

// block 0047cfab
0047cfab  call       0x475860

// block 0047cfb0
0047cfb0  pop        ecx
0047cfb1  jmp        0x47d37f

// block 0047cfb6
0047cfb6  cmp        eax, 0x70
0047cfb9  jg         0x47d1b4

// block 0047cfbf
0047cfbf  je         0x47d1a6

// block 0047cfc5
0047cfc5  cmp        eax, 0x65
0047cfc8  jl         0x47d385

// block 0047cfce
0047cfce  cmp        eax, 0x67
0047cfd1  jle        0x47ce09

// block 0047cfd7
0047cfd7  cmp        eax, 0x69
0047cfda  je         0x47d049

// block 0047cfdc
0047cfdc  cmp        eax, 0x6e
0047cfdf  je         0x47d005

// block 0047cfe1
0047cfe1  cmp        eax, 0x6f
0047cfe4  jne        0x47d385

// block 0047cfea
0047cfea  test       byte ptr [ebp - 0x210], 0x80
0047cff1  mov        dword ptr [ebp - 0x220], esi
0047cff7  je         0x47d05a

// block 0047cff9
0047cff9  or         dword ptr [ebp - 0x210], 0x200
0047d003  jmp        0x47d05a

// block 0047d005
0047d005  mov        esi, dword ptr [edi]
0047d007  add        edi, 4
0047d00a  mov        dword ptr [ebp - 0x224], edi
0047d010  call       0x47c381

// block 0047d015
0047d015  test       eax, eax
0047d017  je         0x47ca36

// block 0047d01d
0047d01d  test       byte ptr [ebp - 0x210], 0x20
0047d024  je         0x47d032

// block 0047d026
0047d026  mov        ax, word ptr [ebp - 0x228]
0047d02d  mov        word ptr [esi], ax
0047d030  jmp        0x47d03a

// block 0047d032
0047d032  mov        eax, dword ptr [ebp - 0x228]
0047d038  mov        dword ptr [esi], eax

// block 0047d03a
0047d03a  mov        dword ptr [ebp - 0x258], 1
0047d044  jmp        0x47d4ee

// block 0047d049
0047d049  or         dword ptr [ebp - 0x210], 0x40

// block 0047d050
0047d050  mov        dword ptr [ebp - 0x220], 0xa

// block 0047d05a
0047d05a  mov        ecx, dword ptr [ebp - 0x210]
0047d060  test       ecx, 0x8000
0047d066  je         0x47d213

// block 0047d06c
0047d06c  add        edi, esi
0047d06e  mov        eax, dword ptr [edi - 8]
0047d071  mov        edx, dword ptr [edi - 4]
0047d074  jmp        0x47d24c

// block 0047d079
0047d079  jne        0x47d08c

// block 0047d07b
0047d07b  cmp        dl, 0x67
0047d07e  jne        0x47d0e6

// block 0047d080
0047d080  mov        dword ptr [ebp - 0x218], 1
0047d08a  jmp        0x47d0e6

// block 0047d08c
0047d08c  cmp        dword ptr [ebp - 0x218], eax
0047d092  jle        0x47d09a

// block 0047d094
0047d094  mov        dword ptr [ebp - 0x218], eax

// block 0047d09a
0047d09a  cmp        dword ptr [ebp - 0x218], 0xa3
0047d0a4  jle        0x47d0e6

// block 0047d0a6
0047d0a6  mov        esi, dword ptr [ebp - 0x218]
0047d0ac  add        esi, 0x15d
0047d0b2  push       esi
0047d0b3  call       0x4777ce

// block 0047d0b8
0047d0b8  mov        dl, byte ptr [ebp - 0x211]
0047d0be  pop        ecx
0047d0bf  mov        dword ptr [ebp - 0x254], eax
0047d0c5  push       8
0047d0c7  test       eax, eax
0047d0c9  je         0x47d0db

// block 0047d0cb
0047d0cb  mov        dword ptr [ebp - 0x21c], eax
0047d0d1  mov        dword ptr [ebp - 0x264], esi
0047d0d7  mov        ebx, eax
0047d0d9  jmp        0x47d0e5

// block 0047d0db
0047d0db  mov        dword ptr [ebp - 0x218], 0xa3

// block 0047d0e5
0047d0e5  pop        esi

// block 0047d0e6
0047d0e6  add        edi, esi
0047d0e8  mov        eax, dword ptr [edi - 8]
0047d0eb  mov        dword ptr [ebp - 0x278], eax
0047d0f1  mov        eax, dword ptr [edi - 4]
0047d0f4  mov        dword ptr [ebp - 0x274], eax
0047d0fa  lea        eax, [ebp - 0x250]
0047d100  push       eax
0047d101  push       dword ptr [ebp - 0x26c]
0047d107  movsx      eax, dl
0047d10a  push       dword ptr [ebp - 0x218]
0047d110  mov        dword ptr [ebp - 0x224], edi
0047d116  push       eax
0047d117  push       dword ptr [ebp - 0x264]
0047d11d  lea        eax, [ebp - 0x278]
0047d123  push       ebx
0047d124  push       eax
0047d125  push       dword ptr [0x4ade88]
0047d12b  call       0x4751de

// block 0047d130
0047d130  pop        ecx
0047d131  call       eax

// block 0047d133
0047d133  mov        edi, dword ptr [ebp - 0x210]
0047d139  add        esp, 0x1c
0047d13c  and        edi, 0x80
0047d142  je         0x47d165

// block 0047d144
0047d144  cmp        dword ptr [ebp - 0x218], 0
0047d14b  jne        0x47d165

// block 0047d14d
0047d14d  lea        eax, [ebp - 0x250]
0047d153  push       eax
0047d154  push       ebx
0047d155  push       dword ptr [0x4ade94]
0047d15b  call       0x4751de

// block 0047d160
0047d160  pop        ecx
0047d161  call       eax

// block 0047d163
0047d163  pop        ecx
0047d164  pop        ecx

// block 0047d165
0047d165  cmp        byte ptr [ebp - 0x211], 0x67
0047d16c  jne        0x47d18a

// block 0047d16e
0047d16e  test       edi, edi
0047d170  jne        0x47d18a

// block 0047d172
0047d172  lea        eax, [ebp - 0x250]
0047d178  push       eax
0047d179  push       ebx
0047d17a  push       dword ptr [0x4ade90]
0047d180  call       0x4751de

// block 0047d185
0047d185  pop        ecx
0047d186  call       eax

// block 0047d188
0047d188  pop        ecx
0047d189  pop        ecx

// block 0047d18a
0047d18a  cmp        byte ptr [ebx], 0x2d
0047d18d  jne        0x47d1a0

// block 0047d18f
0047d18f  or         dword ptr [ebp - 0x210], 0x100
0047d199  inc        ebx
0047d19a  mov        dword ptr [ebp - 0x21c], ebx

// block 0047d1a0
0047d1a0  push       ebx
0047d1a1  jmp        0x47cfab

// block 0047d1a6
0047d1a6  mov        dword ptr [ebp - 0x218], esi

// block 0047d1ac
0047d1ac  mov        dword ptr [ebp - 0x25c], ecx
0047d1b2  jmp        0x47d1d8

// block 0047d1b4
0047d1b4  sub        eax, 0x73
0047d1b7  je         0x47ce78

// block 0047d1bd
0047d1bd  dec        eax
0047d1be  dec        eax
0047d1bf  je         0x47d050

// block 0047d1c5
0047d1c5  sub        eax, 3
0047d1c8  jne        0x47d385

// block 0047d1ce
0047d1ce  mov        dword ptr [ebp - 0x25c], 0x27

// block 0047d1d8
0047d1d8  test       byte ptr [ebp - 0x210], 0x80
0047d1df  mov        dword ptr [ebp - 0x220], 0x10
0047d1e9  je         0x47d05a

// block 0047d1ef
0047d1ef  mov        al, byte ptr [ebp - 0x25c]
0047d1f5  add        al, 0x51
0047d1f7  mov        byte ptr [ebp - 0x22c], 0x30
0047d1fe  mov        byte ptr [ebp - 0x22b], al
0047d204  mov        dword ptr [ebp - 0x230], 2
0047d20e  jmp        0x47d05a

// block 0047d213
0047d213  test       ecx, 0x1000
0047d219  jne        0x47d06c

// block 0047d21f
0047d21f  add        edi, 4
0047d222  test       cl, 0x20
0047d225  je         0x47d23f

// block 0047d227
0047d227  mov        dword ptr [ebp - 0x224], edi
0047d22d  test       cl, 0x40
0047d230  je         0x47d238

// block 0047d232
0047d232  movsx      eax, word ptr [edi - 4]
0047d236  jmp        0x47d23c

// block 0047d238
0047d238  movzx      eax, word ptr [edi - 4]

// block 0047d23c
0047d23c  cdq        
0047d23d  jmp        0x47d252

// block 0047d23f
0047d23f  mov        eax, dword ptr [edi - 4]
0047d242  test       cl, 0x40
0047d245  je         0x47d24a

// block 0047d247
0047d247  cdq        
0047d248  jmp        0x47d24c

// block 0047d24a
0047d24a  xor        edx, edx

// block 0047d24c
0047d24c  mov        dword ptr [ebp - 0x224], edi

// block 0047d252
0047d252  test       cl, 0x40
0047d255  je         0x47d272

// block 0047d257
0047d257  test       edx, edx
0047d259  jg         0x47d272

// block 0047d25b
0047d25b  jl         0x47d261

// block 0047d25d
0047d25d  test       eax, eax
0047d25f  jae        0x47d272

// block 0047d261
0047d261  neg        eax
0047d263  adc        edx, 0
0047d266  neg        edx
0047d268  or         dword ptr [ebp - 0x210], 0x100

// block 0047d272
0047d272  test       dword ptr [ebp - 0x210], 0x9000
0047d27c  mov        ebx, edx
0047d27e  mov        edi, eax
0047d280  jne        0x47d284

// block 0047d282
0047d282  xor        ebx, ebx

// block 0047d284
0047d284  cmp        dword ptr [ebp - 0x218], 0
0047d28b  jge        0x47d299

// block 0047d28d
0047d28d  mov        dword ptr [ebp - 0x218], 1
0047d297  jmp        0x47d2b3

// block 0047d299
0047d299  and        dword ptr [ebp - 0x210], 0xfffffff7
0047d2a0  mov        eax, 0x200
0047d2a5  cmp        dword ptr [ebp - 0x218], eax
0047d2ab  jle        0x47d2b3

// block 0047d2ad
0047d2ad  mov        dword ptr [ebp - 0x218], eax

// block 0047d2b3
0047d2b3  mov        eax, edi
0047d2b5  or         eax, ebx
0047d2b7  jne        0x47d2bf

// block 0047d2b9
0047d2b9  and        dword ptr [ebp - 0x230], eax

// block 0047d2bf
0047d2bf  lea        esi, [ebp - 0xd]

// block 0047d2c2
0047d2c2  mov        eax, dword ptr [ebp - 0x218]
0047d2c8  dec        dword ptr [ebp - 0x218]
0047d2ce  test       eax, eax
0047d2d0  jg         0x47d2d8

// block 0047d2d2
0047d2d2  mov        eax, edi
0047d2d4  or         eax, ebx
0047d2d6  je         0x47d305

// block 0047d2d8
0047d2d8  mov        eax, dword ptr [ebp - 0x220]
0047d2de  cdq        
0047d2df  push       edx
0047d2e0  push       eax
0047d2e1  push       ebx
0047d2e2  push       edi
0047d2e3  call       0x47c890

// block 0047d2e8
0047d2e8  add        ecx, 0x30
0047d2eb  cmp        ecx, 0x39
0047d2ee  mov        dword ptr [ebp - 0x264], ebx
0047d2f4  mov        edi, eax
0047d2f6  mov        ebx, edx
0047d2f8  jle        0x47d300

// block 0047d2fa
0047d2fa  add        ecx, dword ptr [ebp - 0x25c]

// block 0047d300
0047d300  mov        byte ptr [esi], cl
0047d302  dec        esi
0047d303  jmp        0x47d2c2

// block 0047d305
0047d305  lea        eax, [ebp - 0xd]
0047d308  sub        eax, esi
0047d30a  inc        esi
0047d30b  test       dword ptr [ebp - 0x210], 0x200
0047d315  mov        dword ptr [ebp - 0x220], eax
0047d31b  mov        dword ptr [ebp - 0x21c], esi
0047d321  je         0x47d385

// block 0047d323
0047d323  test       eax, eax
0047d325  je         0x47d32e

// block 0047d327
0047d327  mov        ecx, esi
0047d329  cmp        byte ptr [ecx], 0x30
0047d32c  je         0x47d385

// block 0047d32e
0047d32e  dec        dword ptr [ebp - 0x21c]
0047d334  mov        ecx, dword ptr [ebp - 0x21c]
0047d33a  mov        byte ptr [ecx], 0x30
0047d33d  inc        eax
0047d33e  jmp        0x47d37f

// block 0047d340
0047d340  dec        ecx
0047d341  cmp        word ptr [eax], 0
0047d345  je         0x47d34d

// block 0047d347
0047d347  inc        eax
0047d348  inc        eax

// block 0047d349
0047d349  test       ecx, ecx
0047d34b  jne        0x47d340

// block 0047d34d
0047d34d  sub        eax, dword ptr [ebp - 0x21c]
0047d353  sar        eax, 1
0047d355  jmp        0x47d37f

// block 0047d357
0047d357  test       edi, edi
0047d359  jne        0x47d366

// block 0047d35b
0047d35b  mov        eax, dword ptr [0x4ad3d8]
0047d360  mov        dword ptr [ebp - 0x21c], eax

// block 0047d366
0047d366  mov        eax, dword ptr [ebp - 0x21c]
0047d36c  jmp        0x47d375

// block 0047d36e
0047d36e  dec        ecx
0047d36f  cmp        byte ptr [eax], 0
0047d372  je         0x47d379

// block 0047d374
0047d374  inc        eax

// block 0047d375
0047d375  test       ecx, ecx
0047d377  jne        0x47d36e

// block 0047d379
0047d379  sub        eax, dword ptr [ebp - 0x21c]

// block 0047d37f
0047d37f  mov        dword ptr [ebp - 0x220], eax

// block 0047d385
0047d385  cmp        dword ptr [ebp - 0x258], 0
0047d38c  jne        0x47d4ee

// block 0047d392
0047d392  mov        eax, dword ptr [ebp - 0x210]
0047d398  test       al, 0x40
0047d39a  je         0x47d3ce

// block 0047d39c
0047d39c  test       eax, 0x100
0047d3a1  je         0x47d3ac

// block 0047d3a3
0047d3a3  mov        byte ptr [ebp - 0x22c], 0x2d
0047d3aa  jmp        0x47d3c4

// block 0047d3ac
0047d3ac  test       al, 1
0047d3ae  je         0x47d3b9

// block 0047d3b0
0047d3b0  mov        byte ptr [ebp - 0x22c], 0x2b
0047d3b7  jmp        0x47d3c4

// block 0047d3b9
0047d3b9  test       al, 2
0047d3bb  je         0x47d3ce

// block 0047d3bd
0047d3bd  mov        byte ptr [ebp - 0x22c], 0x20

// block 0047d3c4
0047d3c4  mov        dword ptr [ebp - 0x230], 1

// block 0047d3ce
0047d3ce  mov        ebx, dword ptr [ebp - 0x234]
0047d3d4  sub        ebx, dword ptr [ebp - 0x220]
0047d3da  sub        ebx, dword ptr [ebp - 0x230]
0047d3e0  test       byte ptr [ebp - 0x210], 0xc
0047d3e7  jne        0x47d400

// block 0047d3e9
0047d3e9  push       dword ptr [ebp - 0x260]
0047d3ef  lea        eax, [ebp - 0x228]
0047d3f5  push       ebx
0047d3f6  push       0x20
0047d3f8  call       0x47c958

// block 0047d3fd
0047d3fd  add        esp, 0xc

// block 0047d400
0047d400  push       dword ptr [ebp - 0x230]
0047d406  mov        edi, dword ptr [ebp - 0x260]
0047d40c  lea        eax, [ebp - 0x228]
0047d412  lea        ecx, [ebp - 0x22c]
0047d418  call       0x47c97e

// block 0047d41d
0047d41d  test       byte ptr [ebp - 0x210], 8
0047d424  pop        ecx
0047d425  je         0x47d442

// block 0047d427
0047d427  test       byte ptr [ebp - 0x210], 4
0047d42e  jne        0x47d442

// block 0047d430
0047d430  push       edi
0047d431  push       ebx
0047d432  push       0x30
0047d434  lea        eax, [ebp - 0x228]
0047d43a  call       0x47c958

// block 0047d43f
0047d43f  add        esp, 0xc

// block 0047d442
0047d442  cmp        dword ptr [ebp - 0x238], 0
0047d449  mov        eax, dword ptr [ebp - 0x220]
0047d44f  je         0x47d4b7

// block 0047d451
0047d451  test       eax, eax
0047d453  jle        0x47d4b7

// block 0047d455
0047d455  mov        esi, dword ptr [ebp - 0x21c]
0047d45b  mov        dword ptr [ebp - 0x264], eax

// block 0047d461
0047d461  movzx      eax, word ptr [esi]
0047d464  dec        dword ptr [ebp - 0x264]
0047d46a  push       eax
0047d46b  push       6
0047d46d  lea        eax, [ebp - 0xc]
0047d470  push       eax
0047d471  lea        eax, [ebp - 0x270]
0047d477  inc        esi
0047d478  push       eax
0047d479  inc        esi
0047d47a  call       0x47c503

// block 0047d47f
0047d47f  add        esp, 0x10
0047d482  test       eax, eax
0047d484  jne        0x47d4ae

// block 0047d486
0047d486  cmp        dword ptr [ebp - 0x270], eax
0047d48c  je         0x47d4ae

// block 0047d48e
0047d48e  push       dword ptr [ebp - 0x270]
0047d494  lea        eax, [ebp - 0x228]
0047d49a  lea        ecx, [ebp - 0xc]
0047d49d  call       0x47c97e

// block 0047d4a2
0047d4a2  cmp        dword ptr [ebp - 0x264], 0
0047d4a9  pop        ecx
0047d4aa  jne        0x47d461

// block 0047d4ac
0047d4ac  jmp        0x47d4ca

// block 0047d4ae
0047d4ae  or         dword ptr [ebp - 0x228], 0xffffffff
0047d4b5  jmp        0x47d4ca

// block 0047d4b7
0047d4b7  mov        ecx, dword ptr [ebp - 0x21c]
0047d4bd  push       eax
0047d4be  lea        eax, [ebp - 0x228]
0047d4c4  call       0x47c97e

// block 0047d4c9
0047d4c9  pop        ecx

// block 0047d4ca
0047d4ca  cmp        dword ptr [ebp - 0x228], 0
0047d4d1  jl         0x47d4ee

// block 0047d4d3
0047d4d3  test       byte ptr [ebp - 0x210], 4
0047d4da  je         0x47d4ee

// block 0047d4dc
0047d4dc  push       edi
0047d4dd  push       ebx
0047d4de  push       0x20
0047d4e0  lea        eax, [ebp - 0x228]
0047d4e6  call       0x47c958

// block 0047d4eb
0047d4eb  add        esp, 0xc

// block 0047d4ee
0047d4ee  cmp        dword ptr [ebp - 0x254], 0
0047d4f5  je         0x47d50a

// block 0047d4f7
0047d4f7  push       dword ptr [ebp - 0x254]
0047d4fd  call       0x46c941

// block 0047d502
0047d502  and        dword ptr [ebp - 0x254], 0
0047d509  pop        ecx

// block 0047d50a
0047d50a  mov        ebx, dword ptr [ebp - 0x23c]
0047d510  mov        al, byte ptr [ebx]
0047d512  mov        byte ptr [ebp - 0x211], al
0047d518  test       al, al
0047d51a  je         0x47d529

// block 0047d51c
0047d51c  mov        edi, dword ptr [ebp - 0x224]
0047d522  mov        dl, al
0047d524  jmp        0x47cb01

// block 0047d529
0047d529  xor        esi, esi
0047d52b  cmp        dword ptr [ebp - 0x240], esi
0047d531  je         0x47d551

// block 0047d533
0047d533  cmp        dword ptr [ebp - 0x240], 7
0047d53a  je         0x47d551

// block 0047d53c
0047d53c  call       0x46e96f

// block 0047d541
0047d541  push       esi
0047d542  push       esi
0047d543  push       esi
0047d544  push       esi
0047d545  mov        dword ptr [eax], 0x16
0047d54b  push       esi
0047d54c  jmp        0x47ca48

// block 0047d551
0047d551  cmp        byte ptr [ebp - 0x244], 0
0047d558  je         0x47d564

// block 0047d55a
0047d55a  mov        eax, dword ptr [ebp - 0x248]
0047d560  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0047d564
0047d564  mov        eax, dword ptr [ebp - 0x228]

// block 0047d56a
0047d56a  mov        ecx, dword ptr [ebp - 4]
0047d56d  pop        edi
0047d56e  pop        esi
0047d56f  xor        ecx, ebp
0047d571  pop        ebx
0047d572  call       0x46c932

// block 0047d577
0047d577  leave      
0047d578  ret        
