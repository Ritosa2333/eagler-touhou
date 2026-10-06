
// block 004426b0
004426b0  push       ecx
004426b1  movsx      eax, byte ptr [0x4cead0]
004426b8  push       ebp
004426b9  push       esi
004426ba  push       edi
004426bb  xor        ebp, ebp
004426bd  push       ebp
004426be  mov        dword ptr [0x4d477c], eax
004426c3  push       8
004426c5  mov        edi, 0x4a20d0
004426ca  mov        eax, 0x4cf4e8
004426cf  call       0x454960

// block 004426d4
004426d4  movsx      eax, byte ptr [0x4cead1]
004426db  mov        dword ptr [0x4d4780], eax
004426e0  cmp        eax, ebp
004426e2  je         0x44273e

// block 004426e4
004426e4  fild       dword ptr [0x4d477c]
004426ea  fdiv       qword ptr [0x4a3ce8]
004426f0  fstp       dword ptr [esp + 0xc]
004426f4  fld        dword ptr [esp + 0xc]
004426f8  fld1       
004426fa  fld        st(0)
004426fc  fsubrp     st(2)
004426fe  fxch       st(1)
00442700  fstp       dword ptr [esp + 0xc]
00442704  fld        dword ptr [esp + 0xc]
00442708  fmul       st(0), st(0)
0044270a  fstp       dword ptr [esp + 0xc]
0044270e  fld        dword ptr [esp + 0xc]
00442712  fmul       st(0), st(0)
00442714  fstp       dword ptr [esp + 0xc]
00442718  fsub       dword ptr [esp + 0xc]
0044271c  fstp       dword ptr [esp + 0xc]
00442720  fld        dword ptr [esp + 0xc]
00442724  fmul       qword ptr [0x4a3dd8]
0044272a  call       0x4931e0

// block 0044272f
0044272f  mov        ecx, 0xffffec78
00442734  sub        ecx, eax
00442736  mov        dword ptr [0x4d4784], ecx
0044273c  jmp        0x442748

// block 0044273e
0044273e  mov        dword ptr [0x4d4784], 0xffffd8f0

// block 00442748
00442748  mov        edx, dword ptr [ebx + 0x2cc]
0044274e  mov        esi, dword ptr [0x4ce8cc]
00442754  push       edx
00442755  mov        edx, esi
00442757  call       0x461920

// block 0044275c
0044275c  cmp        eax, ebp
0044275e  jne        0x442766

// block 00442760
00442760  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442766
00442766  add        eax, 0x10
00442769  mov        ecx, 0x1d
0044276e  cmp        eax, ebp
00442770  je         0x442796

// block 00442772
00442772  jmp        0x442780

// block 00442780
00442780  mov        edx, dword ptr [eax]
00442782  cmp        word ptr [edx + 0x3ea], cx
00442789  je         0x442e2d

// block 0044278f
0044278f  mov        eax, dword ptr [eax + 4]
00442792  cmp        eax, ebp
00442794  jne        0x442780

// block 00442796
00442796  xor        eax, eax

// block 00442798
00442798  push       eax
00442799  mov        edx, esi
0044279b  call       0x461920

// block 004427a0
004427a0  mov        edi, eax
004427a2  cmp        edi, ebp
004427a4  je         0x4427cd

// block 004427a6
004427a6  movsx      ecx, byte ptr [0x4cead0]
004427ad  mov        eax, 0x51eb851f
004427b2  imul       ecx
004427b4  sar        edx, 5
004427b7  mov        ecx, edx
004427b9  shr        ecx, 0x1f
004427bc  lea        ecx, [edx + ecx + 0x2c]
004427c0  mov        edx, dword ptr [edi + 0x3f8]
004427c6  mov        eax, edi
004427c8  call       0x454b80

// block 004427cd
004427cd  mov        edx, dword ptr [ebx + 0x2cc]
004427d3  push       edx
004427d4  mov        edx, esi
004427d6  call       0x461920

// block 004427db
004427db  cmp        eax, ebp
004427dd  jne        0x4427e5

// block 004427df
004427df  mov        dword ptr [ebx + 0x2cc], ebp

// block 004427e5
004427e5  add        eax, 0x10
004427e8  mov        ecx, 0x1e
004427ed  cmp        eax, ebp
004427ef  je         0x442807

// block 004427f1
004427f1  mov        edx, dword ptr [eax]
004427f3  cmp        word ptr [edx + 0x3ea], cx
004427fa  je         0x442e36

// block 00442800
00442800  mov        eax, dword ptr [eax + 4]
00442803  cmp        eax, ebp
00442805  jne        0x4427f1

// block 00442807
00442807  xor        eax, eax

// block 00442809
00442809  push       eax
0044280a  mov        edx, esi
0044280c  call       0x461920

// block 00442811
00442811  mov        edi, eax
00442813  cmp        edi, ebp
00442815  je         0x442849

// block 00442817
00442817  movsx      ecx, byte ptr [0x4cead0]
0044281e  mov        eax, 0x66666667
00442823  imul       ecx
00442825  sar        edx, 2
00442828  mov        eax, edx
0044282a  shr        eax, 0x1f
0044282d  add        eax, edx
0044282f  cdq        
00442830  mov        ecx, 0xa
00442835  idiv       ecx
00442837  mov        eax, edi
00442839  mov        ecx, edx
0044283b  mov        edx, dword ptr [edi + 0x3f8]
00442841  add        ecx, 0x2c
00442844  call       0x454b80

// block 00442849
00442849  mov        edx, dword ptr [ebx + 0x2cc]
0044284f  push       edx
00442850  mov        edx, esi
00442852  call       0x461920

// block 00442857
00442857  cmp        eax, ebp
00442859  jne        0x442861

// block 0044285b
0044285b  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442861
00442861  add        eax, 0x10
00442864  cmp        eax, ebp
00442866  je         0x442886

// block 00442868
00442868  mov        ecx, 0x1f
0044286d  lea        ecx, [ecx]

// block 00442870
00442870  mov        edx, dword ptr [eax]
00442872  cmp        word ptr [edx + 0x3ea], cx
00442879  je         0x442e3f

// block 0044287f
0044287f  mov        eax, dword ptr [eax + 4]
00442882  cmp        eax, ebp
00442884  jne        0x442870

// block 00442886
00442886  xor        eax, eax

// block 00442888
00442888  push       eax
00442889  mov        edx, esi
0044288b  call       0x461920

// block 00442890
00442890  mov        edi, eax
00442892  cmp        edi, ebp
00442894  je         0x4428b7

// block 00442896
00442896  movsx      eax, byte ptr [0x4cead0]
0044289d  cdq        
0044289e  mov        ecx, 0xa
004428a3  idiv       ecx
004428a5  mov        eax, edi
004428a7  mov        ecx, edx
004428a9  mov        edx, dword ptr [edi + 0x3f8]
004428af  add        ecx, 0x2c
004428b2  call       0x454b80

// block 004428b7
004428b7  mov        edx, dword ptr [ebx + 0x2cc]
004428bd  push       edx
004428be  mov        edx, esi
004428c0  call       0x461920

// block 004428c5
004428c5  cmp        eax, ebp
004428c7  jne        0x4428cf

// block 004428c9
004428c9  mov        dword ptr [ebx + 0x2cc], ebp

// block 004428cf
004428cf  add        eax, 0x10
004428d2  mov        ecx, 0x21
004428d7  cmp        eax, ebp
004428d9  je         0x4428f6

// block 004428db
004428db  jmp        0x4428e0

// block 004428e0
004428e0  mov        edx, dword ptr [eax]
004428e2  cmp        word ptr [edx + 0x3ea], cx
004428e9  je         0x442e48

// block 004428ef
004428ef  mov        eax, dword ptr [eax + 4]
004428f2  cmp        eax, ebp
004428f4  jne        0x4428e0

// block 004428f6
004428f6  xor        eax, eax

// block 004428f8
004428f8  push       eax
004428f9  mov        edx, esi
004428fb  call       0x461920

// block 00442900
00442900  mov        edi, eax
00442902  cmp        edi, ebp
00442904  je         0x44292d

// block 00442906
00442906  movsx      ecx, byte ptr [0x4cead0]
0044290d  mov        eax, 0x51eb851f
00442912  imul       ecx
00442914  sar        edx, 5
00442917  mov        ecx, edx
00442919  shr        ecx, 0x1f
0044291c  lea        ecx, [edx + ecx + 0x37]
00442920  mov        edx, dword ptr [edi + 0x3f8]
00442926  mov        eax, edi
00442928  call       0x454b80

// block 0044292d
0044292d  mov        edx, dword ptr [ebx + 0x2cc]
00442933  push       edx
00442934  mov        edx, esi
00442936  call       0x461920

// block 0044293b
0044293b  cmp        eax, ebp
0044293d  jne        0x442945

// block 0044293f
0044293f  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442945
00442945  add        eax, 0x10
00442948  mov        ecx, 0x22
0044294d  cmp        eax, ebp
0044294f  je         0x442967

// block 00442951
00442951  mov        edx, dword ptr [eax]
00442953  cmp        word ptr [edx + 0x3ea], cx
0044295a  je         0x442e51

// block 00442960
00442960  mov        eax, dword ptr [eax + 4]
00442963  cmp        eax, ebp
00442965  jne        0x442951

// block 00442967
00442967  xor        eax, eax

// block 00442969
00442969  push       eax
0044296a  mov        edx, esi
0044296c  call       0x461920

// block 00442971
00442971  mov        edi, eax
00442973  cmp        edi, ebp
00442975  je         0x4429a9

// block 00442977
00442977  movsx      ecx, byte ptr [0x4cead0]
0044297e  mov        eax, 0x66666667
00442983  imul       ecx
00442985  sar        edx, 2
00442988  mov        eax, edx
0044298a  shr        eax, 0x1f
0044298d  add        eax, edx
0044298f  cdq        
00442990  mov        ecx, 0xa
00442995  idiv       ecx
00442997  mov        eax, edi
00442999  mov        ecx, edx
0044299b  mov        edx, dword ptr [edi + 0x3f8]
004429a1  add        ecx, 0x37
004429a4  call       0x454b80

// block 004429a9
004429a9  mov        edx, dword ptr [ebx + 0x2cc]
004429af  push       edx
004429b0  mov        edx, esi
004429b2  call       0x461920

// block 004429b7
004429b7  cmp        eax, ebp
004429b9  jne        0x4429c1

// block 004429bb
004429bb  mov        dword ptr [ebx + 0x2cc], ebp

// block 004429c1
004429c1  add        eax, 0x10
004429c4  cmp        eax, ebp
004429c6  je         0x4429e6

// block 004429c8
004429c8  mov        ecx, 0x23
004429cd  lea        ecx, [ecx]

// block 004429d0
004429d0  mov        edx, dword ptr [eax]
004429d2  cmp        word ptr [edx + 0x3ea], cx
004429d9  je         0x442e5a

// block 004429df
004429df  mov        eax, dword ptr [eax + 4]
004429e2  cmp        eax, ebp
004429e4  jne        0x4429d0

// block 004429e6
004429e6  xor        eax, eax

// block 004429e8
004429e8  push       eax
004429e9  mov        edx, esi
004429eb  call       0x461920

// block 004429f0
004429f0  mov        edi, eax
004429f2  cmp        edi, ebp
004429f4  je         0x442a17

// block 004429f6
004429f6  movsx      eax, byte ptr [0x4cead0]
004429fd  cdq        
004429fe  mov        ecx, 0xa
00442a03  idiv       ecx
00442a05  mov        eax, edi
00442a07  mov        ecx, edx
00442a09  mov        edx, dword ptr [edi + 0x3f8]
00442a0f  add        ecx, 0x37
00442a12  call       0x454b80

// block 00442a17
00442a17  mov        edx, dword ptr [ebx + 0x2cc]
00442a1d  push       edx
00442a1e  mov        edx, esi
00442a20  call       0x461920

// block 00442a25
00442a25  cmp        eax, ebp
00442a27  jne        0x442a2f

// block 00442a29
00442a29  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442a2f
00442a2f  add        eax, 0x10
00442a32  mov        ecx, 0x25
00442a37  cmp        eax, ebp
00442a39  je         0x442a56

// block 00442a3b
00442a3b  jmp        0x442a40

// block 00442a40
00442a40  mov        edx, dword ptr [eax]
00442a42  cmp        word ptr [edx + 0x3ea], cx
00442a49  je         0x442e63

// block 00442a4f
00442a4f  mov        eax, dword ptr [eax + 4]
00442a52  cmp        eax, ebp
00442a54  jne        0x442a40

// block 00442a56
00442a56  xor        eax, eax

// block 00442a58
00442a58  push       eax
00442a59  mov        edx, esi
00442a5b  call       0x461920

// block 00442a60
00442a60  mov        edi, eax
00442a62  cmp        edi, ebp
00442a64  je         0x442a8d

// block 00442a66
00442a66  movsx      ecx, byte ptr [0x4cead1]
00442a6d  mov        eax, 0x51eb851f
00442a72  imul       ecx
00442a74  sar        edx, 5
00442a77  mov        ecx, edx
00442a79  shr        ecx, 0x1f
00442a7c  lea        ecx, [edx + ecx + 0x2c]
00442a80  mov        edx, dword ptr [edi + 0x3f8]
00442a86  mov        eax, edi
00442a88  call       0x454b80

// block 00442a8d
00442a8d  mov        edx, dword ptr [ebx + 0x2cc]
00442a93  push       edx
00442a94  mov        edx, esi
00442a96  call       0x461920

// block 00442a9b
00442a9b  cmp        eax, ebp
00442a9d  jne        0x442aa5

// block 00442a9f
00442a9f  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442aa5
00442aa5  add        eax, 0x10
00442aa8  mov        ecx, 0x26
00442aad  cmp        eax, ebp
00442aaf  je         0x442ac7

// block 00442ab1
00442ab1  mov        edx, dword ptr [eax]
00442ab3  cmp        word ptr [edx + 0x3ea], cx
00442aba  je         0x442e6c

// block 00442ac0
00442ac0  mov        eax, dword ptr [eax + 4]
00442ac3  cmp        eax, ebp
00442ac5  jne        0x442ab1

// block 00442ac7
00442ac7  xor        eax, eax

// block 00442ac9
00442ac9  push       eax
00442aca  mov        edx, esi
00442acc  call       0x461920

// block 00442ad1
00442ad1  mov        edi, eax
00442ad3  cmp        edi, ebp
00442ad5  je         0x442b09

// block 00442ad7
00442ad7  movsx      ecx, byte ptr [0x4cead1]
00442ade  mov        eax, 0x66666667
00442ae3  imul       ecx
00442ae5  sar        edx, 2
00442ae8  mov        eax, edx
00442aea  shr        eax, 0x1f
00442aed  add        eax, edx
00442aef  cdq        
00442af0  mov        ecx, 0xa
00442af5  idiv       ecx
00442af7  mov        eax, edi
00442af9  mov        ecx, edx
00442afb  mov        edx, dword ptr [edi + 0x3f8]
00442b01  add        ecx, 0x2c
00442b04  call       0x454b80

// block 00442b09
00442b09  mov        edx, dword ptr [ebx + 0x2cc]
00442b0f  push       edx
00442b10  mov        edx, esi
00442b12  call       0x461920

// block 00442b17
00442b17  cmp        eax, ebp
00442b19  jne        0x442b21

// block 00442b1b
00442b1b  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442b21
00442b21  add        eax, 0x10
00442b24  cmp        eax, ebp
00442b26  je         0x442b46

// block 00442b28
00442b28  mov        ecx, 0x27
00442b2d  lea        ecx, [ecx]

// block 00442b30
00442b30  mov        edx, dword ptr [eax]
00442b32  cmp        word ptr [edx + 0x3ea], cx
00442b39  je         0x442e75

// block 00442b3f
00442b3f  mov        eax, dword ptr [eax + 4]
00442b42  cmp        eax, ebp
00442b44  jne        0x442b30

// block 00442b46
00442b46  xor        eax, eax

// block 00442b48
00442b48  push       eax
00442b49  mov        edx, esi
00442b4b  call       0x461920

// block 00442b50
00442b50  mov        edi, eax
00442b52  cmp        edi, ebp
00442b54  je         0x442b77

// block 00442b56
00442b56  movsx      eax, byte ptr [0x4cead1]
00442b5d  cdq        
00442b5e  mov        ecx, 0xa
00442b63  idiv       ecx
00442b65  mov        eax, edi
00442b67  mov        ecx, edx
00442b69  mov        edx, dword ptr [edi + 0x3f8]
00442b6f  add        ecx, 0x2c
00442b72  call       0x454b80

// block 00442b77
00442b77  mov        edx, dword ptr [ebx + 0x2cc]
00442b7d  push       edx
00442b7e  mov        edx, esi
00442b80  call       0x461920

// block 00442b85
00442b85  cmp        eax, ebp
00442b87  jne        0x442b8f

// block 00442b89
00442b89  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442b8f
00442b8f  add        eax, 0x10
00442b92  mov        ecx, 0x29
00442b97  cmp        eax, ebp
00442b99  je         0x442bb6

// block 00442b9b
00442b9b  jmp        0x442ba0

// block 00442ba0
00442ba0  mov        edx, dword ptr [eax]
00442ba2  cmp        word ptr [edx + 0x3ea], cx
00442ba9  je         0x442e7e

// block 00442baf
00442baf  mov        eax, dword ptr [eax + 4]
00442bb2  cmp        eax, ebp
00442bb4  jne        0x442ba0

// block 00442bb6
00442bb6  xor        eax, eax

// block 00442bb8
00442bb8  push       eax
00442bb9  mov        edx, esi
00442bbb  call       0x461920

// block 00442bc0
00442bc0  mov        edi, eax
00442bc2  cmp        edi, ebp
00442bc4  je         0x442bed

// block 00442bc6
00442bc6  movsx      ecx, byte ptr [0x4cead1]
00442bcd  mov        eax, 0x51eb851f
00442bd2  imul       ecx
00442bd4  sar        edx, 5
00442bd7  mov        ecx, edx
00442bd9  shr        ecx, 0x1f
00442bdc  lea        ecx, [edx + ecx + 0x37]
00442be0  mov        edx, dword ptr [edi + 0x3f8]
00442be6  mov        eax, edi
00442be8  call       0x454b80

// block 00442bed
00442bed  mov        edx, dword ptr [ebx + 0x2cc]
00442bf3  push       edx
00442bf4  mov        edx, esi
00442bf6  call       0x461920

// block 00442bfb
00442bfb  cmp        eax, ebp
00442bfd  jne        0x442c05

// block 00442bff
00442bff  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442c05
00442c05  add        eax, 0x10
00442c08  mov        ecx, 0x2a
00442c0d  cmp        eax, ebp
00442c0f  je         0x442c27

// block 00442c11
00442c11  mov        edx, dword ptr [eax]
00442c13  cmp        word ptr [edx + 0x3ea], cx
00442c1a  je         0x442e87

// block 00442c20
00442c20  mov        eax, dword ptr [eax + 4]
00442c23  cmp        eax, ebp
00442c25  jne        0x442c11

// block 00442c27
00442c27  xor        eax, eax

// block 00442c29
00442c29  push       eax
00442c2a  mov        edx, esi
00442c2c  call       0x461920

// block 00442c31
00442c31  mov        edi, eax
00442c33  cmp        edi, ebp
00442c35  je         0x442c69

// block 00442c37
00442c37  movsx      ecx, byte ptr [0x4cead1]
00442c3e  mov        eax, 0x66666667
00442c43  imul       ecx
00442c45  sar        edx, 2
00442c48  mov        eax, edx
00442c4a  shr        eax, 0x1f
00442c4d  add        eax, edx
00442c4f  cdq        
00442c50  mov        ecx, 0xa
00442c55  idiv       ecx
00442c57  mov        eax, edi
00442c59  mov        ecx, edx
00442c5b  mov        edx, dword ptr [edi + 0x3f8]
00442c61  add        ecx, 0x37
00442c64  call       0x454b80

// block 00442c69
00442c69  mov        edx, dword ptr [ebx + 0x2cc]
00442c6f  push       edx
00442c70  mov        edx, esi
00442c72  call       0x461920

// block 00442c77
00442c77  cmp        eax, ebp
00442c79  jne        0x442c81

// block 00442c7b
00442c7b  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442c81
00442c81  add        eax, 0x10
00442c84  cmp        eax, ebp
00442c86  je         0x442ca6

// block 00442c88
00442c88  mov        ecx, 0x2b
00442c8d  lea        ecx, [ecx]

// block 00442c90
00442c90  mov        edx, dword ptr [eax]
00442c92  cmp        word ptr [edx + 0x3ea], cx
00442c99  je         0x442e90

// block 00442c9f
00442c9f  mov        eax, dword ptr [eax + 4]
00442ca2  cmp        eax, ebp
00442ca4  jne        0x442c90

// block 00442ca6
00442ca6  xor        eax, eax

// block 00442ca8
00442ca8  push       eax
00442ca9  mov        edx, esi
00442cab  call       0x461920

// block 00442cb0
00442cb0  mov        edi, eax
00442cb2  cmp        edi, ebp
00442cb4  je         0x442cd7

// block 00442cb6
00442cb6  movsx      eax, byte ptr [0x4cead1]
00442cbd  cdq        
00442cbe  mov        ecx, 0xa
00442cc3  idiv       ecx
00442cc5  mov        eax, edi
00442cc7  mov        ecx, edx
00442cc9  mov        edx, dword ptr [edi + 0x3f8]
00442ccf  add        ecx, 0x37
00442cd2  call       0x454b80

// block 00442cd7
00442cd7  mov        al, byte ptr [0x4cead0]
00442cdc  cmp        al, 0xa
00442cde  jge        0x442ebd

// block 00442ce4
00442ce4  mov        edx, dword ptr [ebx + 0x2cc]
00442cea  push       edx
00442ceb  mov        edx, esi
00442ced  call       0x461920

// block 00442cf2
00442cf2  cmp        eax, ebp
00442cf4  jne        0x442cfc

// block 00442cf6
00442cf6  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442cfc
00442cfc  add        eax, 0x10
00442cff  cmp        eax, ebp
00442d01  je         0x442d26

// block 00442d03
00442d03  mov        ecx, 0x1d
00442d08  jmp        0x442d10

// block 00442d10
00442d10  mov        edx, dword ptr [eax]
00442d12  cmp        word ptr [edx + 0x3ea], cx
00442d19  je         0x442e99

// block 00442d1f
00442d1f  mov        eax, dword ptr [eax + 4]
00442d22  cmp        eax, ebp
00442d24  jne        0x442d10

// block 00442d26
00442d26  xor        eax, eax

// block 00442d28
00442d28  push       eax
00442d29  mov        edx, esi
00442d2b  call       0x461920

// block 00442d30
00442d30  and        dword ptr [eax + 0x47c], 0xfffffffd
00442d37  mov        ecx, dword ptr [ebx + 0x2cc]
00442d3d  add        eax, 0x47c
00442d42  push       ecx
00442d43  mov        edx, esi
00442d45  call       0x461920

// block 00442d4a
00442d4a  cmp        eax, ebp
00442d4c  jne        0x442d54

// block 00442d4e
00442d4e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442d54
00442d54  add        eax, 0x10
00442d57  cmp        eax, ebp
00442d59  je         0x442d76

// block 00442d5b
00442d5b  mov        ecx, 0x1e

// block 00442d60
00442d60  mov        edx, dword ptr [eax]
00442d62  cmp        word ptr [edx + 0x3ea], cx
00442d69  je         0x442ea2

// block 00442d6f
00442d6f  mov        eax, dword ptr [eax + 4]
00442d72  cmp        eax, ebp
00442d74  jne        0x442d60

// block 00442d76
00442d76  xor        eax, eax

// block 00442d78
00442d78  push       eax
00442d79  mov        edx, esi
00442d7b  call       0x461920

// block 00442d80
00442d80  and        dword ptr [eax + 0x47c], 0xfffffffd
00442d87  mov        ecx, dword ptr [ebx + 0x2cc]
00442d8d  add        eax, 0x47c
00442d92  push       ecx
00442d93  mov        edx, esi
00442d95  call       0x461920

// block 00442d9a
00442d9a  cmp        eax, ebp
00442d9c  jne        0x442da4

// block 00442d9e
00442d9e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442da4
00442da4  add        eax, 0x10
00442da7  cmp        eax, ebp
00442da9  je         0x442dc6

// block 00442dab
00442dab  mov        ecx, 0x21

// block 00442db0
00442db0  mov        edx, dword ptr [eax]
00442db2  cmp        word ptr [edx + 0x3ea], cx
00442db9  je         0x442eab

// block 00442dbf
00442dbf  mov        eax, dword ptr [eax + 4]
00442dc2  cmp        eax, ebp
00442dc4  jne        0x442db0

// block 00442dc6
00442dc6  xor        eax, eax

// block 00442dc8
00442dc8  push       eax
00442dc9  mov        edx, esi
00442dcb  call       0x461920

// block 00442dd0
00442dd0  and        dword ptr [eax + 0x47c], 0xfffffffd
00442dd7  mov        ecx, dword ptr [ebx + 0x2cc]
00442ddd  add        eax, 0x47c
00442de2  push       ecx
00442de3  mov        edx, esi
00442de5  call       0x461920

// block 00442dea
00442dea  cmp        eax, ebp
00442dec  jne        0x442df4

// block 00442dee
00442dee  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442df4
00442df4  add        eax, 0x10
00442df7  cmp        eax, ebp
00442df9  je         0x442e16

// block 00442dfb
00442dfb  mov        ecx, 0x22

// block 00442e00
00442e00  mov        edx, dword ptr [eax]
00442e02  cmp        word ptr [edx + 0x3ea], cx
00442e09  je         0x442eb4

// block 00442e0f
00442e0f  mov        eax, dword ptr [eax + 4]
00442e12  cmp        eax, ebp
00442e14  jne        0x442e00

// block 00442e16
00442e16  xor        eax, eax

// block 00442e18
00442e18  push       eax
00442e19  mov        edx, esi
00442e1b  call       0x461920

// block 00442e20
00442e20  add        eax, 0x47c
00442e25  and        dword ptr [eax], 0xfffffffd
00442e28  jmp        0x443008

// block 00442e2d
00442e2d  mov        eax, edx
00442e2f  mov        eax, dword ptr [eax]
00442e31  jmp        0x442798

// block 00442e36
00442e36  mov        eax, edx
00442e38  mov        eax, dword ptr [eax]
00442e3a  jmp        0x442809

// block 00442e3f
00442e3f  mov        eax, edx
00442e41  mov        eax, dword ptr [eax]
00442e43  jmp        0x442888

// block 00442e48
00442e48  mov        eax, edx
00442e4a  mov        eax, dword ptr [eax]
00442e4c  jmp        0x4428f8

// block 00442e51
00442e51  mov        eax, edx
00442e53  mov        eax, dword ptr [eax]
00442e55  jmp        0x442969

// block 00442e5a
00442e5a  mov        eax, edx
00442e5c  mov        eax, dword ptr [eax]
00442e5e  jmp        0x4429e8

// block 00442e63
00442e63  mov        eax, edx
00442e65  mov        eax, dword ptr [eax]
00442e67  jmp        0x442a58

// block 00442e6c
00442e6c  mov        eax, edx
00442e6e  mov        eax, dword ptr [eax]
00442e70  jmp        0x442ac9

// block 00442e75
00442e75  mov        eax, edx
00442e77  mov        eax, dword ptr [eax]
00442e79  jmp        0x442b48

// block 00442e7e
00442e7e  mov        eax, edx
00442e80  mov        eax, dword ptr [eax]
00442e82  jmp        0x442bb8

// block 00442e87
00442e87  mov        eax, edx
00442e89  mov        eax, dword ptr [eax]
00442e8b  jmp        0x442c29

// block 00442e90
00442e90  mov        eax, edx
00442e92  mov        eax, dword ptr [eax]
00442e94  jmp        0x442ca8

// block 00442e99
00442e99  mov        eax, edx
00442e9b  mov        eax, dword ptr [eax]
00442e9d  jmp        0x442d28

// block 00442ea2
00442ea2  mov        eax, edx
00442ea4  mov        eax, dword ptr [eax]
00442ea6  jmp        0x442d78

// block 00442eab
00442eab  mov        eax, edx
00442ead  mov        eax, dword ptr [eax]
00442eaf  jmp        0x442dc8

// block 00442eb4
00442eb4  mov        eax, edx
00442eb6  mov        eax, dword ptr [eax]
00442eb8  jmp        0x442e18

// block 00442ebd
00442ebd  cmp        al, 0x64
00442ebf  mov        ecx, dword ptr [ebx + 0x2cc]
00442ec5  mov        edx, esi
00442ec7  push       ecx
00442ec8  jge        0x443178

// block 00442ece
00442ece  call       0x461920

// block 00442ed3
00442ed3  cmp        eax, ebp
00442ed5  jne        0x442edd

// block 00442ed7
00442ed7  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442edd
00442edd  add        eax, 0x10
00442ee0  cmp        eax, ebp
00442ee2  je         0x442f06

// block 00442ee4
00442ee4  mov        ecx, 0x1d
00442ee9  lea        esp, [esp]

// block 00442ef0
00442ef0  mov        edx, dword ptr [eax]
00442ef2  cmp        word ptr [edx + 0x3ea], cx
00442ef9  je         0x44315d

// block 00442eff
00442eff  mov        eax, dword ptr [eax + 4]
00442f02  cmp        eax, ebp
00442f04  jne        0x442ef0

// block 00442f06
00442f06  xor        eax, eax

// block 00442f08
00442f08  push       eax
00442f09  mov        edx, esi
00442f0b  call       0x461920

// block 00442f10
00442f10  and        dword ptr [eax + 0x47c], 0xfffffffd
00442f17  mov        ecx, dword ptr [ebx + 0x2cc]
00442f1d  add        eax, 0x47c
00442f22  push       ecx
00442f23  mov        edx, esi
00442f25  call       0x461920

// block 00442f2a
00442f2a  cmp        eax, ebp
00442f2c  jne        0x442f34

// block 00442f2e
00442f2e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442f34
00442f34  add        eax, 0x10
00442f37  cmp        eax, ebp
00442f39  je         0x442f56

// block 00442f3b
00442f3b  mov        ecx, 0x1e

// block 00442f40
00442f40  mov        edx, dword ptr [eax]
00442f42  cmp        word ptr [edx + 0x3ea], cx
00442f49  je         0x443166

// block 00442f4f
00442f4f  mov        eax, dword ptr [eax + 4]
00442f52  cmp        eax, ebp
00442f54  jne        0x442f40

// block 00442f56
00442f56  xor        eax, eax

// block 00442f58
00442f58  push       eax
00442f59  mov        edx, esi
00442f5b  call       0x461920

// block 00442f60
00442f60  or         dword ptr [eax + 0x47c], 2
00442f67  mov        ecx, dword ptr [ebx + 0x2cc]
00442f6d  add        eax, 0x47c
00442f72  push       ecx
00442f73  mov        edx, esi
00442f75  call       0x461920

// block 00442f7a
00442f7a  cmp        eax, ebp
00442f7c  jne        0x442f84

// block 00442f7e
00442f7e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442f84
00442f84  add        eax, 0x10
00442f87  cmp        eax, ebp
00442f89  je         0x442fa6

// block 00442f8b
00442f8b  mov        ecx, 0x21

// block 00442f90
00442f90  mov        edx, dword ptr [eax]
00442f92  cmp        word ptr [edx + 0x3ea], cx
00442f99  je         0x44316f

// block 00442f9f
00442f9f  mov        eax, dword ptr [eax + 4]
00442fa2  cmp        eax, ebp
00442fa4  jne        0x442f90

// block 00442fa6
00442fa6  xor        eax, eax

// block 00442fa8
00442fa8  push       eax
00442fa9  mov        edx, esi
00442fab  call       0x461920

// block 00442fb0
00442fb0  and        dword ptr [eax + 0x47c], 0xfffffffd
00442fb7  mov        ecx, dword ptr [ebx + 0x2cc]
00442fbd  add        eax, 0x47c
00442fc2  push       ecx
00442fc3  mov        edx, esi
00442fc5  call       0x461920

// block 00442fca
00442fca  cmp        eax, ebp
00442fcc  jne        0x442fd4

// block 00442fce
00442fce  mov        dword ptr [ebx + 0x2cc], ebp

// block 00442fd4
00442fd4  add        eax, 0x10
00442fd7  cmp        eax, ebp
00442fd9  je         0x442ff6

// block 00442fdb
00442fdb  mov        ecx, 0x22

// block 00442fe0
00442fe0  mov        edx, dword ptr [eax]
00442fe2  cmp        word ptr [edx + 0x3ea], cx
00442fe9  je         0x4432c3

// block 00442fef
00442fef  mov        eax, dword ptr [eax + 4]
00442ff2  cmp        eax, ebp
00442ff4  jne        0x442fe0

// block 00442ff6
00442ff6  xor        eax, eax

// block 00442ff8
00442ff8  push       eax
00442ff9  mov        edx, esi
00442ffb  call       0x461920

// block 00443000
00443000  add        eax, 0x47c
00443005  or         dword ptr [eax], 2

// block 00443008
00443008  mov        al, byte ptr [0x4cead1]
0044300d  cmp        al, 0xa
0044300f  mov        ecx, dword ptr [ebx + 0x2cc]
00443015  mov        edx, esi
00443017  push       ecx
00443018  jge        0x4432f0

// block 0044301e
0044301e  call       0x461920

// block 00443023
00443023  cmp        eax, ebp
00443025  jne        0x44302d

// block 00443027
00443027  mov        dword ptr [ebx + 0x2cc], ebp

// block 0044302d
0044302d  add        eax, 0x10
00443030  cmp        eax, ebp
00443032  je         0x443056

// block 00443034
00443034  mov        ecx, 0x25
00443039  lea        esp, [esp]

// block 00443040
00443040  mov        edx, dword ptr [eax]
00443042  cmp        word ptr [edx + 0x3ea], cx
00443049  je         0x4432cc

// block 0044304f
0044304f  mov        eax, dword ptr [eax + 4]
00443052  cmp        eax, ebp
00443054  jne        0x443040

// block 00443056
00443056  xor        eax, eax

// block 00443058
00443058  push       eax
00443059  mov        edx, esi
0044305b  call       0x461920

// block 00443060
00443060  and        dword ptr [eax + 0x47c], 0xfffffffd
00443067  mov        ecx, dword ptr [ebx + 0x2cc]
0044306d  add        eax, 0x47c
00443072  push       ecx
00443073  mov        edx, esi
00443075  call       0x461920

// block 0044307a
0044307a  cmp        eax, ebp
0044307c  jne        0x443084

// block 0044307e
0044307e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443084
00443084  add        eax, 0x10
00443087  cmp        eax, ebp
00443089  je         0x4430a6

// block 0044308b
0044308b  mov        ecx, 0x26

// block 00443090
00443090  mov        edx, dword ptr [eax]
00443092  cmp        word ptr [edx + 0x3ea], cx
00443099  je         0x4432d5

// block 0044309f
0044309f  mov        eax, dword ptr [eax + 4]
004430a2  cmp        eax, ebp
004430a4  jne        0x443090

// block 004430a6
004430a6  xor        eax, eax

// block 004430a8
004430a8  push       eax
004430a9  mov        edx, esi
004430ab  call       0x461920

// block 004430b0
004430b0  and        dword ptr [eax + 0x47c], 0xfffffffd
004430b7  mov        ecx, dword ptr [ebx + 0x2cc]
004430bd  add        eax, 0x47c
004430c2  push       ecx
004430c3  mov        edx, esi
004430c5  call       0x461920

// block 004430ca
004430ca  cmp        eax, ebp
004430cc  jne        0x4430d4

// block 004430ce
004430ce  mov        dword ptr [ebx + 0x2cc], ebp

// block 004430d4
004430d4  add        eax, 0x10
004430d7  cmp        eax, ebp
004430d9  je         0x4430f6

// block 004430db
004430db  mov        ecx, 0x29

// block 004430e0
004430e0  mov        edx, dword ptr [eax]
004430e2  cmp        word ptr [edx + 0x3ea], cx
004430e9  je         0x4432de

// block 004430ef
004430ef  mov        eax, dword ptr [eax + 4]
004430f2  cmp        eax, ebp
004430f4  jne        0x4430e0

// block 004430f6
004430f6  xor        eax, eax

// block 004430f8
004430f8  push       eax
004430f9  mov        edx, esi
004430fb  call       0x461920

// block 00443100
00443100  and        dword ptr [eax + 0x47c], 0xfffffffd
00443107  mov        ecx, dword ptr [ebx + 0x2cc]
0044310d  add        eax, 0x47c
00443112  push       ecx
00443113  mov        edx, esi
00443115  call       0x461920

// block 0044311a
0044311a  cmp        eax, ebp
0044311c  jne        0x443124

// block 0044311e
0044311e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443124
00443124  add        eax, 0x10
00443127  cmp        eax, ebp
00443129  je         0x443146

// block 0044312b
0044312b  mov        ecx, 0x2a

// block 00443130
00443130  mov        edx, dword ptr [eax]
00443132  cmp        word ptr [edx + 0x3ea], cx
00443139  je         0x4432e7

// block 0044313f
0044313f  mov        eax, dword ptr [eax + 4]
00443142  cmp        eax, ebp
00443144  jne        0x443130

// block 00443146
00443146  xor        eax, eax

// block 00443148
00443148  push       eax
00443149  mov        edx, esi
0044314b  call       0x461920

// block 00443150
00443150  pop        edi
00443151  add        eax, 0x47c
00443156  and        dword ptr [eax], 0xfffffffd
00443159  pop        esi
0044315a  pop        ebp
0044315b  pop        ecx
0044315c  ret        

// block 0044315d
0044315d  mov        eax, edx
0044315f  mov        eax, dword ptr [eax]
00443161  jmp        0x442f08

// block 00443166
00443166  mov        eax, edx
00443168  mov        eax, dword ptr [eax]
0044316a  jmp        0x442f58

// block 0044316f
0044316f  mov        eax, edx
00443171  mov        eax, dword ptr [eax]
00443173  jmp        0x442fa8

// block 00443178
00443178  call       0x461920

// block 0044317d
0044317d  cmp        eax, ebp
0044317f  jne        0x443187

// block 00443181
00443181  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443187
00443187  add        eax, 0x10
0044318a  cmp        eax, ebp
0044318c  je         0x4431b6

// block 0044318e
0044318e  mov        ecx, 0x1d
00443193  jmp        0x4431a0

// block 004431a0
004431a0  mov        edx, dword ptr [eax]
004431a2  cmp        word ptr [edx + 0x3ea], cx
004431a9  je         0x4432ab

// block 004431af
004431af  mov        eax, dword ptr [eax + 4]
004431b2  cmp        eax, ebp
004431b4  jne        0x4431a0

// block 004431b6
004431b6  xor        eax, eax

// block 004431b8
004431b8  push       eax
004431b9  mov        edx, esi
004431bb  call       0x461920

// block 004431c0
004431c0  or         dword ptr [eax + 0x47c], 2
004431c7  mov        ecx, dword ptr [ebx + 0x2cc]
004431cd  add        eax, 0x47c
004431d2  push       ecx
004431d3  mov        edx, esi
004431d5  call       0x461920

// block 004431da
004431da  cmp        eax, ebp
004431dc  jne        0x4431e4

// block 004431de
004431de  mov        dword ptr [ebx + 0x2cc], ebp

// block 004431e4
004431e4  add        eax, 0x10
004431e7  cmp        eax, ebp
004431e9  je         0x443206

// block 004431eb
004431eb  mov        ecx, 0x1e

// block 004431f0
004431f0  mov        edx, dword ptr [eax]
004431f2  cmp        word ptr [edx + 0x3ea], cx
004431f9  je         0x4432b4

// block 004431ff
004431ff  mov        eax, dword ptr [eax + 4]
00443202  cmp        eax, ebp
00443204  jne        0x4431f0

// block 00443206
00443206  xor        eax, eax

// block 00443208
00443208  push       eax
00443209  mov        edx, esi
0044320b  call       0x461920

// block 00443210
00443210  or         dword ptr [eax + 0x47c], 2
00443217  mov        ecx, dword ptr [ebx + 0x2cc]
0044321d  add        eax, 0x47c
00443222  push       ecx
00443223  mov        edx, esi
00443225  call       0x461920

// block 0044322a
0044322a  cmp        eax, ebp
0044322c  jne        0x443234

// block 0044322e
0044322e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443234
00443234  add        eax, 0x10
00443237  cmp        eax, ebp
00443239  je         0x443256

// block 0044323b
0044323b  mov        ecx, 0x21

// block 00443240
00443240  mov        edx, dword ptr [eax]
00443242  cmp        word ptr [edx + 0x3ea], cx
00443249  je         0x4432bd

// block 0044324f
0044324f  mov        eax, dword ptr [eax + 4]
00443252  cmp        eax, ebp
00443254  jne        0x443240

// block 00443256
00443256  xor        eax, eax

// block 00443258
00443258  push       eax
00443259  mov        edx, esi
0044325b  call       0x461920

// block 00443260
00443260  or         dword ptr [eax + 0x47c], 2
00443267  mov        ecx, dword ptr [ebx + 0x2cc]
0044326d  add        eax, 0x47c
00443272  push       ecx
00443273  mov        edx, esi
00443275  call       0x461920

// block 0044327a
0044327a  cmp        eax, ebp
0044327c  jne        0x443284

// block 0044327e
0044327e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443284
00443284  add        eax, 0x10
00443287  cmp        eax, ebp
00443289  je         0x442ff6

// block 0044328f
0044328f  mov        ecx, 0x22

// block 00443294
00443294  mov        edx, dword ptr [eax]
00443296  cmp        word ptr [edx + 0x3ea], cx
0044329d  je         0x4432c3

// block 0044329f
0044329f  mov        eax, dword ptr [eax + 4]
004432a2  cmp        eax, ebp
004432a4  jne        0x443294

// block 004432a6
004432a6  jmp        0x442ff6

// block 004432ab
004432ab  mov        eax, edx
004432ad  mov        eax, dword ptr [eax]
004432af  jmp        0x4431b8

// block 004432b4
004432b4  mov        eax, edx
004432b6  mov        eax, dword ptr [eax]
004432b8  jmp        0x443208

// block 004432bd
004432bd  mov        eax, edx
004432bf  mov        eax, dword ptr [eax]
004432c1  jmp        0x443258

// block 004432c3
004432c3  mov        eax, edx
004432c5  mov        eax, dword ptr [eax]
004432c7  jmp        0x442ff8

// block 004432cc
004432cc  mov        eax, edx
004432ce  mov        eax, dword ptr [eax]
004432d0  jmp        0x443058

// block 004432d5
004432d5  mov        eax, edx
004432d7  mov        eax, dword ptr [eax]
004432d9  jmp        0x4430a8

// block 004432de
004432de  mov        eax, edx
004432e0  mov        eax, dword ptr [eax]
004432e2  jmp        0x4430f8

// block 004432e7
004432e7  mov        eax, edx
004432e9  mov        eax, dword ptr [eax]
004432eb  jmp        0x443148

// block 004432f0
004432f0  cmp        al, 0x64
004432f2  jge        0x443455

// block 004432f8
004432f8  call       0x461920

// block 004432fd
004432fd  cmp        eax, ebp
004432ff  jne        0x443307

// block 00443301
00443301  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443307
00443307  add        eax, 0x10
0044330a  cmp        eax, ebp
0044330c  je         0x443336

// block 0044330e
0044330e  mov        ecx, 0x25
00443313  jmp        0x443320

// block 00443320
00443320  mov        edx, dword ptr [eax]
00443322  cmp        word ptr [edx + 0x3ea], cx
00443329  je         0x44343d

// block 0044332f
0044332f  mov        eax, dword ptr [eax + 4]
00443332  cmp        eax, ebp
00443334  jne        0x443320

// block 00443336
00443336  xor        eax, eax

// block 00443338
00443338  push       eax
00443339  mov        edx, esi
0044333b  call       0x461920

// block 00443340
00443340  and        dword ptr [eax + 0x47c], 0xfffffffd
00443347  mov        ecx, dword ptr [ebx + 0x2cc]
0044334d  add        eax, 0x47c
00443352  push       ecx
00443353  mov        edx, esi
00443355  call       0x461920

// block 0044335a
0044335a  cmp        eax, ebp
0044335c  jne        0x443364

// block 0044335e
0044335e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443364
00443364  add        eax, 0x10
00443367  cmp        eax, ebp
00443369  je         0x443386

// block 0044336b
0044336b  mov        ecx, 0x26

// block 00443370
00443370  mov        edx, dword ptr [eax]
00443372  cmp        word ptr [edx + 0x3ea], cx
00443379  je         0x443446

// block 0044337f
0044337f  mov        eax, dword ptr [eax + 4]
00443382  cmp        eax, ebp
00443384  jne        0x443370

// block 00443386
00443386  xor        eax, eax

// block 00443388
00443388  push       eax
00443389  mov        edx, esi
0044338b  call       0x461920

// block 00443390
00443390  or         dword ptr [eax + 0x47c], 2
00443397  mov        ecx, dword ptr [ebx + 0x2cc]
0044339d  add        eax, 0x47c
004433a2  push       ecx
004433a3  mov        edx, esi
004433a5  call       0x461920

// block 004433aa
004433aa  cmp        eax, ebp
004433ac  jne        0x4433b4

// block 004433ae
004433ae  mov        dword ptr [ebx + 0x2cc], ebp

// block 004433b4
004433b4  add        eax, 0x10
004433b7  cmp        eax, ebp
004433b9  je         0x4433d6

// block 004433bb
004433bb  mov        ecx, 0x29

// block 004433c0
004433c0  mov        edx, dword ptr [eax]
004433c2  cmp        word ptr [edx + 0x3ea], cx
004433c9  je         0x44344f

// block 004433cf
004433cf  mov        eax, dword ptr [eax + 4]
004433d2  cmp        eax, ebp
004433d4  jne        0x4433c0

// block 004433d6
004433d6  xor        eax, eax

// block 004433d8
004433d8  push       eax
004433d9  mov        edx, esi
004433db  call       0x461920

// block 004433e0
004433e0  and        dword ptr [eax + 0x47c], 0xfffffffd
004433e7  mov        ecx, dword ptr [ebx + 0x2cc]
004433ed  add        eax, 0x47c
004433f2  push       ecx
004433f3  mov        edx, esi
004433f5  call       0x461920

// block 004433fa
004433fa  cmp        eax, ebp
004433fc  jne        0x443404

// block 004433fe
004433fe  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443404
00443404  add        eax, 0x10
00443407  cmp        eax, ebp
00443409  je         0x443426

// block 0044340b
0044340b  mov        ecx, 0x2a

// block 00443410
00443410  mov        edx, dword ptr [eax]
00443412  cmp        word ptr [edx + 0x3ea], cx
00443419  je         0x4435a3

// block 0044341f
0044341f  mov        eax, dword ptr [eax + 4]
00443422  cmp        eax, ebp
00443424  jne        0x443410

// block 00443426
00443426  xor        eax, eax

// block 00443428
00443428  push       eax
00443429  mov        edx, esi
0044342b  call       0x461920

// block 00443430
00443430  pop        edi
00443431  add        eax, 0x47c
00443436  or         dword ptr [eax], 2
00443439  pop        esi
0044343a  pop        ebp
0044343b  pop        ecx
0044343c  ret        

// block 0044343d
0044343d  mov        eax, edx
0044343f  mov        eax, dword ptr [eax]
00443441  jmp        0x443338

// block 00443446
00443446  mov        eax, edx
00443448  mov        eax, dword ptr [eax]
0044344a  jmp        0x443388

// block 0044344f
0044344f  mov        eax, edx
00443451  mov        eax, dword ptr [eax]
00443453  jmp        0x4433d8

// block 00443455
00443455  call       0x461920

// block 0044345a
0044345a  cmp        eax, ebp
0044345c  jne        0x443464

// block 0044345e
0044345e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443464
00443464  add        eax, 0x10
00443467  cmp        eax, ebp
00443469  je         0x443486

// block 0044346b
0044346b  mov        ecx, 0x25

// block 00443470
00443470  mov        edx, dword ptr [eax]
00443472  cmp        word ptr [edx + 0x3ea], cx
00443479  je         0x44358b

// block 0044347f
0044347f  mov        eax, dword ptr [eax + 4]
00443482  cmp        eax, ebp
00443484  jne        0x443470

// block 00443486
00443486  xor        eax, eax

// block 00443488
00443488  push       eax
00443489  mov        edx, esi
0044348b  call       0x461920

// block 00443490
00443490  or         dword ptr [eax + 0x47c], 2
00443497  mov        ecx, dword ptr [ebx + 0x2cc]
0044349d  add        eax, 0x47c
004434a2  push       ecx
004434a3  mov        edx, esi
004434a5  call       0x461920

// block 004434aa
004434aa  cmp        eax, ebp
004434ac  jne        0x4434b4

// block 004434ae
004434ae  mov        dword ptr [ebx + 0x2cc], ebp

// block 004434b4
004434b4  add        eax, 0x10
004434b7  cmp        eax, ebp
004434b9  je         0x4434d6

// block 004434bb
004434bb  mov        ecx, 0x26

// block 004434c0
004434c0  mov        edx, dword ptr [eax]
004434c2  cmp        word ptr [edx + 0x3ea], cx
004434c9  je         0x443594

// block 004434cf
004434cf  mov        eax, dword ptr [eax + 4]
004434d2  cmp        eax, ebp
004434d4  jne        0x4434c0

// block 004434d6
004434d6  xor        eax, eax

// block 004434d8
004434d8  push       eax
004434d9  mov        edx, esi
004434db  call       0x461920

// block 004434e0
004434e0  or         dword ptr [eax + 0x47c], 2
004434e7  mov        ecx, dword ptr [ebx + 0x2cc]
004434ed  add        eax, 0x47c
004434f2  push       ecx
004434f3  mov        edx, esi
004434f5  call       0x461920

// block 004434fa
004434fa  cmp        eax, ebp
004434fc  jne        0x443504

// block 004434fe
004434fe  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443504
00443504  add        eax, 0x10
00443507  cmp        eax, ebp
00443509  je         0x443526

// block 0044350b
0044350b  mov        ecx, 0x29

// block 00443510
00443510  mov        edx, dword ptr [eax]
00443512  cmp        word ptr [edx + 0x3ea], cx
00443519  je         0x44359d

// block 0044351f
0044351f  mov        eax, dword ptr [eax + 4]
00443522  cmp        eax, ebp
00443524  jne        0x443510

// block 00443526
00443526  xor        eax, eax

// block 00443528
00443528  push       eax
00443529  mov        edx, esi
0044352b  call       0x461920

// block 00443530
00443530  or         dword ptr [eax + 0x47c], 2
00443537  mov        ecx, dword ptr [ebx + 0x2cc]
0044353d  add        eax, 0x47c
00443542  push       ecx
00443543  mov        edx, esi
00443545  call       0x461920

// block 0044354a
0044354a  cmp        eax, ebp
0044354c  jne        0x443554

// block 0044354e
0044354e  mov        dword ptr [ebx + 0x2cc], ebp

// block 00443554
00443554  add        eax, 0x10
00443557  cmp        eax, ebp
00443559  je         0x443426

// block 0044355f
0044355f  mov        ecx, 0x2a
00443564  jmp        0x443570

// block 00443570
00443570  mov        edx, dword ptr [eax]
00443572  cmp        word ptr [edx + 0x3ea], cx
00443579  je         0x4435a3

// block 0044357f
0044357f  mov        eax, dword ptr [eax + 4]
00443582  cmp        eax, ebp
00443584  jne        0x443570

// block 00443586
00443586  jmp        0x443426

// block 0044358b
0044358b  mov        eax, edx
0044358d  mov        eax, dword ptr [eax]
0044358f  jmp        0x443488

// block 00443594
00443594  mov        eax, edx
00443596  mov        eax, dword ptr [eax]
00443598  jmp        0x4434d8

// block 0044359d
0044359d  mov        eax, edx
0044359f  mov        eax, dword ptr [eax]
004435a1  jmp        0x443528

// block 004435a3
004435a3  mov        eax, edx
004435a5  mov        eax, dword ptr [eax]
004435a7  jmp        0x443428
