
// block 004154c0
004154c0  add        ecx, 0x1040
004154c6  jmp        0x4154d0

// block 004154d0
004154d0  push       ebp
004154d1  mov        ebp, esp
004154d3  and        esp, 0xfffffff8
004154d6  push       -1
004154d8  push       0x4976ce
004154dd  mov        eax, dword ptr fs:[0]
004154e3  push       eax
004154e4  sub        esp, 0x308
004154ea  mov        eax, dword ptr [0x4ad138]
004154ef  xor        eax, esp
004154f1  mov        dword ptr [esp + 0x300], eax
004154f8  push       ebx
004154f9  push       esi
004154fa  push       edi
004154fb  mov        eax, dword ptr [0x4ad138]
00415500  xor        eax, esp
00415502  push       eax
00415503  lea        eax, [esp + 0x318]
0041550a  mov        dword ptr fs:[0], eax
00415510  mov        ebx, ecx
00415512  mov        esi, dword ptr [ebx + 0x174c]
00415518  mov        eax, dword ptr [esi + 4]
0041551b  mov        edi, dword ptr [eax + 4]
0041551e  movzx      eax, word ptr [edi + 4]
00415522  movsx      ecx, ax
00415525  add        ecx, 0xffffff00
0041552b  mov        dword ptr [esp + 0x34], ebx
0041552f  mov        dword ptr [esp + 0x18], edi
00415533  cmp        ecx, 0x163
00415539  ja         0x41962b

// block 0041553f
0041553f  movzx      ecx, byte ptr [ecx + 0x419890]
00415546  jmp        dword ptr [ecx*4 + 0x419654]

// block 0041554d
0041554d  mov        edx, dword ptr [0x4b43dc]
00415553  cmp        dword ptr [edx + 0x1c], 0
00415557  jne        0x41962b

// block 0041555d
0041555d  mov        eax, dword ptr [edi + 0x10]
00415560  add        eax, 4
00415563  cdq        
00415564  and        edx, 3
00415567  add        eax, edx
00415569  mov        edi, eax
0041556b  push       0x50
0041556d  lea        eax, [esp + 0x290]
00415574  push       0
00415576  push       eax
00415577  sar        edi, 2
0041557a  call       0x477420

// block 0041557f
0041557f  fld        dword ptr [ebx + 0x34]
00415582  mov        esi, dword ptr [esp + 0x24]
00415586  add        esp, 0xc
00415589  fstp       dword ptr [esp + 0x10]
0041558d  test       dword ptr [ebx + 0x16b8], 0x2000000
00415597  fld        dword ptr [esi + edi*4 + 0x10]
0041559b  push       ecx
0041559c  fstp       dword ptr [esp]
0041559f  mov        eax, ebx
004155a1  push       1
004155a3  jne        0x4155dd

// block 004155a5
004155a5  call       0x41a9b0

// block 004155aa
004155aa  fadd       dword ptr [esp + 0x10]
004155ae  push       ecx
004155af  mov        eax, ebx
004155b1  fstp       dword ptr [esp + 0x290]
004155b8  fld        dword ptr [ebx + 0x38]
004155bb  fstp       dword ptr [esp + 0x14]
004155bf  fld        dword ptr [esi + edi*4 + 0x14]
004155c3  fstp       dword ptr [esp]
004155c6  push       2
004155c8  call       0x41a9b0

// block 004155cd
004155cd  fadd       dword ptr [esp + 0x10]
004155d1  fstp       dword ptr [esp + 0x290]
004155d8  jmp        0x415678

// block 004155dd
004155dd  call       0x41a9b0

// block 004155e2
004155e2  fadd       dword ptr [esp + 0x10]
004155e6  push       ecx
004155e7  mov        eax, ebx
004155e9  fstp       dword ptr [esp + 0x28]
004155ed  fld        dword ptr [ebx + 0x38]
004155f0  fstp       dword ptr [esp + 0x14]
004155f4  fld        dword ptr [esi + edi*4 + 0x14]
004155f8  fstp       dword ptr [esp]
004155fb  push       2
004155fd  call       0x41a9b0

// block 00415602
00415602  fadd       dword ptr [esp + 0x10]
00415606  mov        esi, 0x4ce8e8
0041560b  fstp       dword ptr [esp + 0x28]
0041560f  fld        dword ptr [ebx + 0x3c]
00415612  xor        ebx, ebx
00415614  fstp       dword ptr [esp + 0x2c]
00415618  call       0x402410

// block 0041561d
0041561d  push       0x4b0e88
00415622  push       0x4ceb38
00415627  push       0x4ceb78
0041562c  push       0x4cebb8
00415631  lea        ecx, [esp + 0x34]
00415635  push       ecx
00415636  lea        edx, [esp + 0x2a0]
0041563d  push       edx
0041563e  call       0x46c6d4

// block 00415643
00415643  fld        dword ptr [esp + 0x28c]
0041564a  fsub       qword ptr [0x4a40b0]
00415650  mov        ebx, dword ptr [esp + 0x34]
00415654  fstp       dword ptr [esp + 0x28c]
0041565b  fld        dword ptr [esp + 0x290]
00415662  fsub       qword ptr [0x4a3d68]
00415668  fstp       dword ptr [esp + 0x290]
0041566f  fldz       
00415671  fstp       dword ptr [esp + 0x294]

// block 00415678
00415678  mov        esi, dword ptr [esp + 0x18]
0041567c  mov        eax, dword ptr [esi + edi*4 + 0x18]
00415680  push       3
00415682  mov        ecx, ebx
00415684  call       0x41a970

// block 00415689
00415689  mov        dword ptr [esp + 0x2a0], eax
00415690  mov        eax, dword ptr [esi + edi*4 + 0x1c]
00415694  push       4
00415696  mov        ecx, ebx
00415698  call       0x41a970

// block 0041569d
0041569d  mov        dword ptr [esp + 0x298], eax
004156a4  mov        eax, dword ptr [esi + edi*4 + 0x20]
004156a8  push       5
004156aa  mov        ecx, ebx
004156ac  call       0x41a970

// block 004156b1
004156b1  lea        esi, [ebx + 0x23c]
004156b7  mov        ecx, 0xc
004156bc  lea        edi, [esp + 0x2ac]
004156c3  mov        dword ptr [esp + 0x29c], eax

// block 004156ca
004156ca  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 004156cc
004156cc  mov        ecx, dword ptr [esp + 0x18]
004156d0  lea        eax, [esp + 0x28c]
004156d7  push       eax
004156d8  add        ecx, 0x14
004156db  push       ecx
004156dc  call       0x412990

// block 004156e1
004156e1  jmp        0x41962b

// block 004156e6
004156e6  mov        edx, dword ptr [0x4b43dc]
004156ec  cmp        dword ptr [edx + 0x1c], 0
004156f0  jne        0x41962b

// block 004156f6
004156f6  mov        eax, dword ptr [edi + 0x10]
004156f9  add        eax, 4
004156fc  cdq        
004156fd  and        edx, 3
00415700  add        eax, edx
00415702  mov        esi, eax
00415704  push       0x50
00415706  lea        eax, [esp + 0x290]
0041570d  push       0
0041570f  push       eax
00415710  sar        esi, 2
00415713  call       0x477420

// block 00415718
00415718  fld        dword ptr [edi + esi*4 + 0x10]
0041571c  add        esp, 8
0041571f  fstp       dword ptr [esp]
00415722  push       1
00415724  mov        eax, ebx
00415726  call       0x41a9b0

// block 0041572b
0041572b  fstp       dword ptr [esp + 0x28c]
00415732  fld        dword ptr [edi + esi*4 + 0x14]
00415736  push       ecx
00415737  fstp       dword ptr [esp]
0041573a  push       2
0041573c  mov        eax, ebx
0041573e  call       0x41a9b0

// block 00415743
00415743  fstp       dword ptr [esp + 0x290]
0041574a  mov        eax, dword ptr [edi + esi*4 + 0x18]
0041574e  push       3
00415750  mov        ecx, ebx
00415752  call       0x41a970

// block 00415757
00415757  mov        dword ptr [esp + 0x2a0], eax
0041575e  mov        eax, dword ptr [edi + esi*4 + 0x1c]
00415762  push       4
00415764  mov        ecx, ebx
00415766  call       0x41a970

// block 0041576b
0041576b  mov        dword ptr [esp + 0x298], eax
00415772  mov        eax, dword ptr [edi + esi*4 + 0x20]
00415776  push       5
00415778  mov        ecx, ebx
0041577a  call       0x41a970

// block 0041577f
0041577f  mov        edx, dword ptr [esp + 0x18]
00415783  mov        dword ptr [esp + 0x29c], eax
0041578a  lea        esi, [ebx + 0x23c]
00415790  mov        ecx, 0xc
00415795  lea        edi, [esp + 0x2ac]

// block 0041579c
0041579c  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0041579e
0041579e  lea        ecx, [esp + 0x28c]
004157a5  push       ecx
004157a6  add        edx, 0x14
004157a9  push       edx
004157aa  call       0x412990

// block 004157af
004157af  jmp        0x41962b

// block 004157b4
004157b4  mov        eax, dword ptr [0x4b43dc]
004157b9  cmp        dword ptr [eax + 0x1c], 0
004157bd  jne        0x41962b

// block 004157c3
004157c3  mov        eax, dword ptr [edi + 0x10]
004157c6  add        eax, 4
004157c9  cdq        
004157ca  and        edx, 3
004157cd  push       0x50
004157cf  add        eax, edx
004157d1  lea        ecx, [esp + 0x290]
004157d8  mov        esi, eax
004157da  push       0
004157dc  push       ecx
004157dd  sar        esi, 2
004157e0  call       0x477420

// block 004157e5
004157e5  fld        dword ptr [edi + esi*4 + 0x10]
004157e9  add        esp, 8
004157ec  fstp       dword ptr [esp]
004157ef  push       1
004157f1  mov        eax, ebx
004157f3  call       0x41a9b0

// block 004157f8
004157f8  fadd       dword ptr [0x4ceaec]
004157fe  push       ecx
004157ff  mov        eax, ebx
00415801  fstp       dword ptr [esp + 0x290]
00415808  fld        dword ptr [edi + esi*4 + 0x14]
0041580c  fstp       dword ptr [esp]
0041580f  push       2
00415811  call       0x41a9b0

// block 00415816
00415816  fadd       dword ptr [0x4ceaf0]
0041581c  push       ecx
0041581d  mov        eax, ebx
0041581f  fstp       dword ptr [esp + 0x294]
00415826  fld        dword ptr [edi + esi*4 + 0x18]
0041582a  fstp       dword ptr [esp]
0041582d  push       3
0041582f  call       0x41a9b0

// block 00415834
00415834  fstp       dword ptr [esp + 0x294]
0041583b  mov        eax, dword ptr [edi + esi*4 + 0x1c]
0041583f  push       4
00415841  mov        ecx, ebx
00415843  call       0x41a970

// block 00415848
00415848  mov        dword ptr [esp + 0x2a0], eax
0041584f  mov        eax, dword ptr [edi + esi*4 + 0x20]
00415853  push       5
00415855  mov        ecx, ebx
00415857  call       0x41a970

// block 0041585c
0041585c  mov        dword ptr [esp + 0x298], eax
00415863  mov        eax, dword ptr [edi + esi*4 + 0x24]
00415867  push       6
00415869  mov        ecx, ebx
0041586b  call       0x41a970

// block 00415870
00415870  mov        dword ptr [esp + 0x29c], eax
00415877  mov        eax, dword ptr [esp + 0x18]
0041587b  lea        edx, [esp + 0x28c]
00415882  push       edx
00415883  add        eax, 0x14
00415886  lea        esi, [ebx + 0x23c]
0041588c  mov        ecx, 0xc
00415891  lea        edi, [esp + 0x2b0]

// block 00415898
00415898  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0041589a
0041589a  or         dword ptr [esp + 0x2ac], 1
004158a2  push       eax
004158a3  call       0x412990

// block 004158a8
004158a8  jmp        0x41962b

// block 004158ad
004158ad  mov        ecx, dword ptr [0x4b43dc]
004158b3  cmp        dword ptr [ecx + 0x1c], 0
004158b7  jne        0x41962b

// block 004158bd
004158bd  mov        eax, dword ptr [edi + 0x10]
004158c0  add        eax, 4
004158c3  cdq        
004158c4  and        edx, 3
004158c7  add        eax, edx
004158c9  push       0x50
004158cb  lea        edx, [esp + 0x290]
004158d2  mov        esi, eax
004158d4  push       0
004158d6  push       edx
004158d7  sar        esi, 2
004158da  call       0x477420

// block 004158df
004158df  fld        dword ptr [ebx + 0x34]
004158e2  add        esp, 8
004158e5  fstp       dword ptr [esp + 0x14]
004158e9  mov        eax, ebx
004158eb  fld        dword ptr [edi + esi*4 + 0x10]
004158ef  fstp       dword ptr [esp]
004158f2  push       1
004158f4  call       0x41a9b0

// block 004158f9
004158f9  fadd       dword ptr [esp + 0x10]
004158fd  push       ecx
004158fe  mov        eax, ebx
00415900  fstp       dword ptr [esp + 0x290]
00415907  fld        dword ptr [ebx + 0x38]
0041590a  fstp       dword ptr [esp + 0x14]
0041590e  fld        dword ptr [edi + esi*4 + 0x14]
00415912  fstp       dword ptr [esp]
00415915  push       2
00415917  call       0x41a9b0

// block 0041591c
0041591c  fadd       dword ptr [esp + 0x10]
00415920  mov        eax, dword ptr [edi + esi*4 + 0x18]
00415924  push       3
00415926  mov        ecx, ebx
00415928  fstp       dword ptr [esp + 0x294]
0041592f  call       0x41a970

// block 00415934
00415934  mov        dword ptr [esp + 0x2a0], eax
0041593b  mov        eax, dword ptr [edi + esi*4 + 0x1c]
0041593f  push       4
00415941  mov        ecx, ebx
00415943  call       0x41a970

// block 00415948
00415948  mov        dword ptr [esp + 0x298], eax
0041594f  mov        eax, dword ptr [edi + esi*4 + 0x20]
00415953  push       5
00415955  mov        ecx, ebx
00415957  call       0x41a970

// block 0041595c
0041595c  lea        esi, [ebx + 0x23c]
00415962  mov        ecx, 0xc
00415967  lea        edi, [esp + 0x2ac]
0041596e  mov        dword ptr [esp + 0x29c], eax

// block 00415975
00415975  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00415977
00415977  mov        dword ptr [esp + 0x2a4], 1
00415982  jmp        0x4156cc

// block 00415987
00415987  mov        edx, dword ptr [0x4b43dc]
0041598d  cmp        dword ptr [edx + 0x1c], 0
00415991  jne        0x41962b

// block 00415997
00415997  mov        eax, dword ptr [edi + 0x10]
0041599a  add        eax, 4
0041599d  cdq        
0041599e  and        edx, 3
004159a1  add        eax, edx
004159a3  mov        esi, eax
004159a5  push       0x50
004159a7  lea        eax, [esp + 0x290]
004159ae  push       0
004159b0  push       eax
004159b1  sar        esi, 2
004159b4  call       0x477420

// block 004159b9
004159b9  fld        dword ptr [edi + esi*4 + 0x10]
004159bd  add        esp, 8
004159c0  fstp       dword ptr [esp]
004159c3  push       1
004159c5  mov        eax, ebx
004159c7  call       0x41a9b0

// block 004159cc
004159cc  fstp       dword ptr [esp + 0x28c]
004159d3  fld        dword ptr [edi + esi*4 + 0x14]
004159d7  push       ecx
004159d8  fstp       dword ptr [esp]
004159db  push       2
004159dd  mov        eax, ebx
004159df  call       0x41a9b0

// block 004159e4
004159e4  fstp       dword ptr [esp + 0x290]
004159eb  mov        eax, dword ptr [edi + esi*4 + 0x18]
004159ef  push       3
004159f1  mov        ecx, ebx
004159f3  call       0x41a970

// block 004159f8
004159f8  mov        dword ptr [esp + 0x2a0], eax
004159ff  mov        eax, dword ptr [edi + esi*4 + 0x1c]
00415a03  push       4
00415a05  mov        ecx, ebx
00415a07  call       0x41a970

// block 00415a0c
00415a0c  mov        dword ptr [esp + 0x298], eax
00415a13  mov        eax, dword ptr [edi + esi*4 + 0x20]
00415a17  push       5
00415a19  mov        ecx, ebx
00415a1b  call       0x41a970

// block 00415a20
00415a20  mov        edx, dword ptr [esp + 0x18]
00415a24  mov        dword ptr [esp + 0x29c], eax
00415a2b  lea        esi, [ebx + 0x23c]
00415a31  mov        ecx, 0xc
00415a36  lea        edi, [esp + 0x2ac]

// block 00415a3d
00415a3d  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00415a3f
00415a3f  lea        ecx, [esp + 0x28c]
00415a46  push       ecx
00415a47  add        edx, 0x14
00415a4a  push       edx
00415a4b  mov        dword ptr [esp + 0x2ac], 1
00415a56  call       0x412990

// block 00415a5b
00415a5b  jmp        0x41962b

// block 00415a60
00415a60  mov        edx, dword ptr [esi + 4]
00415a63  push       0
00415a65  call       0x468e20

// block 00415a6a
00415a6a  mov        dword ptr [ebx + 0x220], eax
00415a70  jmp        0x41962b

// block 00415a75
00415a75  mov        edx, dword ptr [esi + 4]
00415a78  push       0
00415a7a  call       0x468e20

// block 00415a7f
00415a7f  shl        eax, 0x12
00415a82  xor        eax, dword ptr [ebx + 0x16b8]
00415a88  and        eax, 0x40000
00415a8d  xor        dword ptr [ebx + 0x16b8], eax
00415a93  jmp        0x41962b

// block 00415a98
00415a98  mov        edx, dword ptr [esi + 4]
00415a9b  push       0
00415a9d  call       0x468e20

// block 00415aa2
00415aa2  mov        ecx, dword ptr [0x4b43dc]
00415aa8  mov        edx, dword ptr [ecx + 0x3c]
00415aab  xor        edx, eax
00415aad  and        edx, 1
00415ab0  xor        dword ptr [ecx + 0x3c], edx
00415ab3  jmp        0x41962b

// block 00415ab8
00415ab8  mov        edx, dword ptr [esi + 4]
00415abb  push       0
00415abd  call       0x468e20

// block 00415ac2
00415ac2  mov        dword ptr [ebx + 0x238], eax
00415ac8  jmp        0x41962b

// block 00415acd
00415acd  mov        edx, dword ptr [esi + 4]
00415ad0  push       0
00415ad2  call       0x468e20

// block 00415ad7
00415ad7  mov        dword ptr [ebx + 0x168c], eax
00415add  jmp        0x41962b

// block 00415ae2
00415ae2  mov        edx, dword ptr [esi + 4]
00415ae5  push       0
00415ae7  call       0x468e20

// block 00415aec
00415aec  mov        esi, eax
00415aee  push       esi
00415aef  mov        ecx, 0x4b0c40
00415af4  call       0x40e730

// block 00415af9
00415af9  push       -1
00415afb  add        ebx, 0x34
00415afe  push       ebx
00415aff  mov        eax, esi
00415b01  call       0x43e250

// block 00415b06
00415b06  jmp        0x41962b

// block 00415b0b
00415b0b  mov        edx, dword ptr [esi + 4]
00415b0e  push       0
00415b10  call       0x468e20

// block 00415b15
00415b15  mov        esi, eax
00415b17  mov        eax, dword ptr [ebx + 0x174c]
00415b1d  mov        edx, dword ptr [eax + 4]
00415b20  push       1
00415b22  mov        dword ptr [esp + 0x18], esi
00415b26  call       0x468e20

// block 00415b2b
00415b2b  mov        edi, eax
00415b2d  mov        eax, dword ptr [ebx + esi*4 + 0xe0]
00415b34  lea        esi, [ebx + esi*4 + 0xe0]
00415b3b  push       eax
00415b3c  call       0x461a70

// block 00415b41
00415b41  mov        dword ptr [esi], 0
00415b47  test       edi, edi
00415b49  jl         0x41962b

// block 00415b4f
00415b4f  mov        eax, dword ptr [ebx + 0x174c]
00415b55  mov        edx, dword ptr [eax + 4]
00415b58  push       1
00415b5a  call       0x468e20

// block 00415b5f
00415b5f  mov        edx, dword ptr [ebx + 0x220]
00415b65  mov        ecx, eax
00415b67  mov        eax, dword ptr [0x4b43dc]
00415b6c  mov        edi, dword ptr [eax + edx*4 + 0x40]
00415b70  mov        eax, dword ptr [ebx + 0x238]
00415b76  push       ecx
00415b77  lea        ecx, [esp + 0x84]
00415b7e  add        eax, 7
00415b81  push       ecx
00415b82  call       0x41c6a0

// block 00415b87
00415b87  cmp        dword ptr [esp + 0x14], 0
00415b8c  mov        edx, dword ptr [eax]
00415b8e  mov        dword ptr [esi], edx
00415b90  jne        0x415bb4

// block 00415b92
00415b92  mov        eax, dword ptr [ebx + 0x174c]
00415b98  mov        edx, dword ptr [eax + 4]
00415b9b  push       1
00415b9d  call       0x468e20

// block 00415ba2
00415ba2  mov        dword ptr [ebx + 0x228], eax
00415ba8  mov        eax, dword ptr [ebx + 0x220]
00415bae  mov        dword ptr [ebx + 0x224], eax

// block 00415bb4
00415bb4  call       0x461c50

// block 00415bb9
00415bb9  cmp        dword ptr [esp + 0x14], 0

// block 00415bbe
00415bbe  jne        0x415bd8

// block 00415bc0
00415bc0  fld        dword ptr [eax + 0x5c]
00415bc3  fmul       dword ptr [eax + 0x44]
00415bc6  fstp       dword ptr [ebx + 0x15ec]
00415bcc  fld        dword ptr [eax + 0x58]
00415bcf  fmul       dword ptr [eax + 0x40]
00415bd2  fstp       dword ptr [ebx + 0x15f0]

// block 00415bd8
00415bd8  test       byte ptr [ebx + 0x16b8], 0x20
00415bdf  je         0x41962b

// block 00415be5
00415be5  mov        eax, esi
00415be7  call       0x461d80

// block 00415bec
00415bec  jmp        0x41962b

// block 00415bf1
00415bf1  mov        edx, dword ptr [esi + 4]
00415bf4  push       0
00415bf6  call       0x468e20

// block 00415bfb
00415bfb  lea        esi, [ebx + eax*4 + 0xe0]
00415c02  call       0x461c50

// block 00415c07
00415c07  mov        esi, eax
00415c09  test       esi, esi
00415c0b  je         0x41962b

// block 00415c11
00415c11  mov        ebx, dword ptr [ebx + 0x174c]
00415c17  mov        eax, dword ptr [ebx + 4]
00415c1a  mov        ecx, 1
00415c1f  call       0x468ec0

// block 00415c24
00415c24  fstp       dword ptr [esi + 0x2c]
00415c27  or         dword ptr [esi + 0x47c], 4
00415c2e  jmp        0x41962b

// block 00415c33
00415c33  mov        edx, dword ptr [esi + 4]
00415c36  push       0
00415c38  call       0x468e20

// block 00415c3d
00415c3d  lea        esi, [ebx + eax*4 + 0xe0]
00415c44  call       0x461c50

// block 00415c49
00415c49  mov        esi, eax
00415c4b  test       esi, esi
00415c4d  je         0x41962b

// block 00415c53
00415c53  mov        eax, dword ptr [ebx + 0x174c]
00415c59  mov        eax, dword ptr [eax + 4]
00415c5c  mov        ecx, 1
00415c61  call       0x468ec0

// block 00415c66
00415c66  fstp       dword ptr [esi + 0x40]
00415c69  mov        edi, 8
00415c6e  or         dword ptr [esi + 0x47c], edi
00415c74  mov        ebx, dword ptr [ebx + 0x174c]
00415c7a  mov        eax, dword ptr [ebx + 4]
00415c7d  lea        ecx, [edi - 7]
00415c80  call       0x468ec0

// block 00415c85
00415c85  fstp       dword ptr [esi + 0x44]
00415c88  or         dword ptr [esi + 0x47c], edi
00415c8e  jmp        0x41962b

// block 00415c93
00415c93  mov        edx, dword ptr [esi + 4]
00415c96  push       0
00415c98  call       0x468e20

// block 00415c9d
00415c9d  mov        esi, eax
00415c9f  mov        eax, dword ptr [ebx + 0x174c]
00415ca5  mov        eax, dword ptr [eax + 4]
00415ca8  mov        ecx, 1
00415cad  call       0x468ec0

// block 00415cb2
00415cb2  lea        ecx, [esi + esi*2 + 0x48]
00415cb6  fstp       dword ptr [ebx + ecx*4]
00415cb9  mov        eax, dword ptr [ebx + 0x174c]
00415cbf  mov        eax, dword ptr [eax + 4]
00415cc2  lea        edx, [esi + esi*2]
00415cc5  mov        ecx, 2
00415cca  lea        esi, [ebx + edx*4]
00415ccd  call       0x468ec0

// block 00415cd2
00415cd2  fstp       dword ptr [esi + 0x124]
00415cd8  fldz       
00415cda  fstp       dword ptr [esi + 0x128]
00415ce0  jmp        0x41962b

// block 00415ce5
00415ce5  mov        edx, dword ptr [esi + 4]
00415ce8  push       1
00415cea  call       0x468e20

// block 00415cef
00415cef  mov        esi, eax
00415cf1  mov        eax, dword ptr [ebx + 0x174c]
00415cf7  mov        edx, dword ptr [eax + 4]
00415cfa  push       0
00415cfc  call       0x468e20

// block 00415d01
00415d01  mov        dword ptr [ebx + eax*4 + 0x1e0], esi
00415d08  jmp        0x41962b

// block 00415d0d
00415d0d  call       0x421740

// block 00415d12
00415d12  jmp        0x41962b

// block 00415d17
00415d17  test       dword ptr [ebx + 0x16b8], 0x2000000
00415d21  mov        edx, dword ptr [esi + 4]
00415d24  push       1
00415d26  jne        0x415e24

// block 00415d2c
00415d2c  call       0x468e20

// block 00415d31
00415d31  mov        dword ptr [esp + 0x10], eax
00415d35  mov        eax, dword ptr [ebx + 0x174c]
00415d3b  mov        edx, dword ptr [eax + 4]
00415d3e  push       0
00415d40  call       0x468e20

// block 00415d45
00415d45  test       dword ptr [0x4cee78], 0x8000
00415d4f  mov        ecx, dword ptr [0x4b43dc]
00415d55  mov        esi, dword ptr [ecx + eax*4 + 0x40]
00415d59  lea        edi, [ebx + 0x34]
00415d5c  je         0x415d6f

// block 00415d5e
00415d5e  push       0x4cf1d0
00415d63  call       dword ptr [0x498088]

// block 00415d69
00415d69  inc        byte ptr [0x4cf221]

// block 00415d6f
00415d6f  inc        dword ptr [esi + 0x130]
00415d75  call       0x4621c0

// block 00415d7a
00415d7a  mov        ebx, eax
00415d7c  or         dword ptr [ebx + 0x480], 1
00415d83  mov        dword ptr [ebx + 0x20], 9
00415d8a  test       edi, edi
00415d8c  jne        0x415dbc

// block 00415d8e
00415d8e  fldz       
00415d90  fst        dword ptr [esp + 0x24]
00415d94  mov        edx, dword ptr [esp + 0x24]
00415d98  fst        dword ptr [esp + 0x28]
00415d9c  mov        eax, dword ptr [esp + 0x28]
00415da0  fstp       dword ptr [esp + 0x2c]
00415da4  mov        ecx, dword ptr [esp + 0x2c]
00415da8  mov        dword ptr [ebx + 0x430], edx
00415dae  mov        dword ptr [ebx + 0x434], eax
00415db4  mov        dword ptr [ebx + 0x438], ecx
00415dba  jmp        0x415de8

// block 00415dbc
00415dbc  fld        dword ptr [edi]
00415dbe  fadd       qword ptr [0x4a3d70]
00415dc4  fadd       qword ptr [0x4a3d28]
00415dca  fstp       dword ptr [ebx + 0x430]
00415dd0  fld        dword ptr [edi + 4]
00415dd3  fadd       qword ptr [0x4a3d68]
00415dd9  fstp       dword ptr [ebx + 0x434]
00415ddf  fld        dword ptr [edi + 8]
00415de2  fstp       dword ptr [ebx + 0x438]

// block 00415de8
00415de8  mov        edx, dword ptr [esp + 0x10]
00415dec  push       edx
00415ded  push       ebx
00415dee  mov        ecx, esi
00415df0  call       0x454d10

// block 00415df5
00415df5  lea        eax, [esp + 0x1c]
00415df9  call       0x4612d0

// block 00415dfe
00415dfe  test       dword ptr [0x4cee78], 0x8000
00415e08  je         0x41962b

// block 00415e0e
00415e0e  push       0x4cf1d0
00415e13  call       dword ptr [0x49808c]

// block 00415e19
00415e19  dec        byte ptr [0x4cf221]
00415e1f  jmp        0x41962b

// block 00415e24
00415e24  call       0x468e20

// block 00415e29
00415e29  mov        dword ptr [esp + 0x10], eax
00415e2d  mov        eax, dword ptr [ebx + 0x174c]
00415e33  mov        edx, dword ptr [eax + 4]
00415e36  push       0
00415e38  call       0x468e20

// block 00415e3d
00415e3d  test       dword ptr [0x4cee78], 0x8000
00415e47  mov        ecx, dword ptr [0x4b43dc]
00415e4d  mov        esi, dword ptr [ecx + eax*4 + 0x40]
00415e51  lea        edi, [ebx + 0x34]
00415e54  je         0x415e67

// block 00415e56
00415e56  push       0x4cf1d0
00415e5b  call       dword ptr [0x498088]

// block 00415e61
00415e61  inc        byte ptr [0x4cf221]

// block 00415e67
00415e67  inc        dword ptr [esi + 0x130]
00415e6d  call       0x4621c0

// block 00415e72
00415e72  mov        ebx, eax
00415e74  or         dword ptr [ebx + 0x480], 1
00415e7b  mov        dword ptr [ebx + 0x20], 9
00415e82  test       edi, edi
00415e84  jne        0x415eae

// block 00415e86
00415e86  fldz       
00415e88  fst        dword ptr [esp + 0x24]
00415e8c  mov        edx, dword ptr [esp + 0x24]
00415e90  fst        dword ptr [esp + 0x28]
00415e94  mov        eax, dword ptr [esp + 0x28]
00415e98  fstp       dword ptr [esp + 0x2c]
00415e9c  mov        ecx, dword ptr [esp + 0x2c]
00415ea0  mov        dword ptr [ebx + 0x430], edx
00415ea6  mov        dword ptr [ebx + 0x434], eax
00415eac  jmp        0x415ec2

// block 00415eae
00415eae  mov        edx, dword ptr [edi]
00415eb0  mov        dword ptr [ebx + 0x430], edx
00415eb6  mov        eax, dword ptr [edi + 4]
00415eb9  mov        dword ptr [ebx + 0x434], eax
00415ebf  mov        ecx, dword ptr [edi + 8]

// block 00415ec2
00415ec2  mov        edx, dword ptr [esp + 0x10]
00415ec6  push       edx
00415ec7  mov        dword ptr [ebx + 0x438], ecx
00415ecd  push       ebx
00415ece  mov        ecx, esi
00415ed0  call       0x454d10

// block 00415ed5
00415ed5  lea        eax, [esp + 0x1c]
00415ed9  call       0x4612d0

// block 00415ede
00415ede  test       dword ptr [0x4cee78], 0x8000
00415ee8  je         0x41962b

// block 00415eee
00415eee  push       0x4cf1d0
00415ef3  call       dword ptr [0x49808c]

// block 00415ef9
00415ef9  dec        byte ptr [0x4cf221]
00415eff  jmp        0x41962b

// block 00415f04
00415f04  test       dword ptr [ebx + 0x16b8], 0x2000000
00415f0e  mov        edx, dword ptr [esi + 4]
00415f11  push       1
00415f13  jne        0x415ff0

// block 00415f19
00415f19  call       0x468e20

// block 00415f1e
00415f1e  mov        dword ptr [esp + 0x10], eax
00415f22  mov        eax, dword ptr [ebx + 0x174c]
00415f28  mov        edx, dword ptr [eax + 4]
00415f2b  push       0
00415f2d  call       0x468e20

// block 00415f32
00415f32  test       dword ptr [0x4cee78], 0x8000
00415f3c  mov        ecx, dword ptr [0x4b43dc]
00415f42  mov        esi, dword ptr [ecx + eax*4 + 0x40]
00415f46  lea        edi, [ebx + 0x34]
00415f49  je         0x415f5c

// block 00415f4b
00415f4b  push       0x4cf1d0
00415f50  call       dword ptr [0x498088]

// block 00415f56
00415f56  inc        byte ptr [0x4cf221]

// block 00415f5c
00415f5c  inc        dword ptr [esi + 0x130]
00415f62  call       0x4621c0

// block 00415f67
00415f67  mov        ebx, eax
00415f69  or         dword ptr [ebx + 0x480], 1
00415f70  mov        dword ptr [ebx + 0x20], 9
00415f77  test       edi, edi
00415f79  jne        0x415fa9

// block 00415f7b
00415f7b  fldz       
00415f7d  fst        dword ptr [esp + 0x24]
00415f81  mov        edx, dword ptr [esp + 0x24]
00415f85  fst        dword ptr [esp + 0x28]
00415f89  mov        eax, dword ptr [esp + 0x28]
00415f8d  fstp       dword ptr [esp + 0x2c]
00415f91  mov        ecx, dword ptr [esp + 0x2c]
00415f95  mov        dword ptr [ebx + 0x430], edx
00415f9b  mov        dword ptr [ebx + 0x434], eax
00415fa1  mov        dword ptr [ebx + 0x438], ecx
00415fa7  jmp        0x415fd5

// block 00415fa9
00415fa9  fld        dword ptr [edi]
00415fab  fadd       qword ptr [0x4a3d70]
00415fb1  fadd       qword ptr [0x4a3d28]
00415fb7  fstp       dword ptr [ebx + 0x430]
00415fbd  fld        dword ptr [edi + 4]
00415fc0  fadd       qword ptr [0x4a3d68]
00415fc6  fstp       dword ptr [ebx + 0x434]
00415fcc  fld        dword ptr [edi + 8]
00415fcf  fstp       dword ptr [ebx + 0x438]

// block 00415fd5
00415fd5  mov        edx, dword ptr [esp + 0x10]
00415fd9  push       edx
00415fda  push       ebx
00415fdb  mov        ecx, esi
00415fdd  call       0x454d10

// block 00415fe2
00415fe2  lea        eax, [esp + 0x1c]
00415fe6  call       0x461250

// block 00415feb
00415feb  jmp        0x415ede

// block 00415ff0
00415ff0  call       0x468e20

// block 00415ff5
00415ff5  mov        esi, eax
00415ff7  mov        eax, dword ptr [ebx + 0x174c]
00415ffd  mov        edx, dword ptr [eax + 4]
00416000  push       0
00416002  call       0x468e20

// block 00416007
00416007  push       0
00416009  push       esi
0041600a  lea        edx, [esp + 0x24]
0041600e  push       edx
0041600f  mov        edx, dword ptr [0x4b43dc]
00416015  mov        eax, dword ptr [edx + eax*4 + 0x40]
00416019  push       eax
0041601a  lea        ecx, [ebx + 0x34]
0041601d  mov        eax, 9
00416022  call       0x4615a0

// block 00416027
00416027  jmp        0x41962b

// block 0041602c
0041602c  mov        edx, dword ptr [esi + 4]
0041602f  push       0
00416031  call       0x468e20

// block 00416036
00416036  mov        ebx, dword ptr [ebx + 0x174c]
0041603c  mov        edx, dword ptr [ebx + 4]
0041603f  push       1
00416041  mov        esi, eax
00416043  call       0x468e20

// block 00416048
00416048  mov        edx, dword ptr [0x4b43dc]
0041604e  mov        edi, dword ptr [edx + esi*4 + 0x40]
00416052  push       eax
00416053  lea        ecx, [esp + 0x20]
00416057  push       ecx
00416058  mov        eax, 9
0041605d  call       0x41c6a0

// block 00416062
00416062  jmp        0x41962b

// block 00416067
00416067  test       dword ptr [ebx + 0x16b8], 0x2000000
00416071  mov        edx, dword ptr [esi + 4]
00416074  push       1
00416076  jne        0x416113

// block 0041607c
0041607c  call       0x468e20

// block 00416081
00416081  mov        dword ptr [esp + 0x10], eax
00416085  mov        eax, dword ptr [ebx + 0x174c]
0041608b  mov        edx, dword ptr [eax + 4]
0041608e  push       0
00416090  call       0x468e20

// block 00416095
00416095  test       dword ptr [0x4cee78], 0x8000
0041609f  mov        ecx, dword ptr [0x4b43dc]
004160a5  mov        esi, dword ptr [ecx + eax*4 + 0x40]
004160a9  lea        edi, [ebx + 0x34]
004160ac  je         0x4160bf

// block 004160ae
004160ae  push       0x4cf1d0
004160b3  call       dword ptr [0x498088]

// block 004160b9
004160b9  inc        byte ptr [0x4cf221]

// block 004160bf
004160bf  inc        dword ptr [esi + 0x130]
004160c5  call       0x4621c0

// block 004160ca
004160ca  mov        ebx, eax
004160cc  or         dword ptr [ebx + 0x480], 1
004160d3  mov        dword ptr [ebx + 0x20], 9
004160da  test       edi, edi
004160dc  je         0x416175

// block 004160e2
004160e2  fld        dword ptr [edi]
004160e4  fadd       qword ptr [0x4a3d70]
004160ea  fadd       qword ptr [0x4a3d28]
004160f0  fstp       dword ptr [ebx + 0x430]
004160f6  fld        dword ptr [edi + 4]
004160f9  fadd       qword ptr [0x4a3d68]
004160ff  fstp       dword ptr [ebx + 0x434]
00416105  fld        dword ptr [edi + 8]
00416108  fstp       dword ptr [ebx + 0x438]
0041610e  jmp        0x4161b7

// block 00416113
00416113  call       0x468e20

// block 00416118
00416118  mov        dword ptr [esp + 0x10], eax
0041611c  mov        eax, dword ptr [ebx + 0x174c]
00416122  mov        edx, dword ptr [eax + 4]
00416125  push       0
00416127  call       0x468e20

// block 0041612c
0041612c  test       dword ptr [0x4cee78], 0x8000
00416136  mov        ecx, dword ptr [0x4b43dc]
0041613c  mov        esi, dword ptr [ecx + eax*4 + 0x40]
00416140  lea        edi, [ebx + 0x34]
00416143  je         0x416156

// block 00416145
00416145  push       0x4cf1d0
0041614a  call       dword ptr [0x498088]

// block 00416150
00416150  inc        byte ptr [0x4cf221]

// block 00416156
00416156  inc        dword ptr [esi + 0x130]
0041615c  call       0x4621c0

// block 00416161
00416161  mov        ebx, eax
00416163  or         dword ptr [ebx + 0x480], 1
0041616a  mov        dword ptr [ebx + 0x20], 9
00416171  test       edi, edi
00416173  jne        0x41619d

// block 00416175
00416175  fldz       
00416177  fst        dword ptr [esp + 0x24]
0041617b  mov        edx, dword ptr [esp + 0x24]
0041617f  fst        dword ptr [esp + 0x28]
00416183  mov        eax, dword ptr [esp + 0x28]
00416187  fstp       dword ptr [esp + 0x2c]
0041618b  mov        ecx, dword ptr [esp + 0x2c]
0041618f  mov        dword ptr [ebx + 0x430], edx
00416195  mov        dword ptr [ebx + 0x434], eax
0041619b  jmp        0x4161b1

// block 0041619d
0041619d  mov        edx, dword ptr [edi]
0041619f  mov        dword ptr [ebx + 0x430], edx
004161a5  mov        eax, dword ptr [edi + 4]
004161a8  mov        dword ptr [ebx + 0x434], eax
004161ae  mov        ecx, dword ptr [edi + 8]

// block 004161b1
004161b1  mov        dword ptr [ebx + 0x438], ecx

// block 004161b7
004161b7  mov        edx, dword ptr [esp + 0x10]
004161bb  push       edx
004161bc  push       ebx
004161bd  mov        ecx, esi
004161bf  call       0x454d10

// block 004161c4
004161c4  lea        eax, [esp + 0x1c]
004161c8  call       0x4612d0

// block 004161cd
004161cd  test       dword ptr [0x4cee78], 0x8000
004161d7  mov        esi, dword ptr [eax]
004161d9  je         0x4161ec

// block 004161db
004161db  push       0x4cf1d0
004161e0  call       dword ptr [0x49808c]

// block 004161e6
004161e6  dec        byte ptr [0x4cf221]

// block 004161ec
004161ec  mov        dword ptr [esp + 0x14], esi
004161f0  lea        esi, [esp + 0x14]
004161f4  call       0x461c50

// block 004161f9
004161f9  mov        edx, dword ptr [esp + 0x34]
004161fd  test       dword ptr [edx + 0x16b8], 0x2000000
00416207  mov        esi, eax
00416209  jne        0x41622e

// block 0041620b
0041620b  mov        eax, edi
0041620d  call       0x422c40

// block 00416212
00416212  mov        ecx, dword ptr [eax]
00416214  mov        dword ptr [esi + 0x430], ecx
0041621a  mov        ecx, dword ptr [eax + 4]
0041621d  mov        dword ptr [esi + 0x434], ecx
00416223  mov        eax, dword ptr [eax + 8]
00416226  mov        dword ptr [esi + 0x438], eax
0041622c  jmp        0x416248

// block 0041622e
0041622e  mov        ecx, dword ptr [edi]
00416230  mov        dword ptr [esi + 0x430], ecx
00416236  mov        eax, dword ptr [edi + 4]
00416239  mov        dword ptr [esi + 0x434], eax
0041623f  mov        ecx, dword ptr [edi + 8]
00416242  mov        dword ptr [esi + 0x438], ecx

// block 00416248
00416248  mov        eax, dword ptr [edx + 0x174c]
0041624e  mov        eax, dword ptr [eax + 4]
00416251  mov        ecx, 2
00416256  call       0x468ec0

// block 0041625b
0041625b  fstp       dword ptr [esi + 0x2c]
0041625e  or         dword ptr [esi + 0x47c], 4
00416265  jmp        0x41962b

// block 0041626a
0041626a  mov        edx, dword ptr [esi + 4]
0041626d  push       0
0041626f  call       0x468e20

// block 00416274
00416274  mov        esi, eax
00416276  mov        eax, dword ptr [ebx + 0x174c]
0041627c  mov        edx, dword ptr [eax + 4]
0041627f  push       1
00416281  mov        dword ptr [esp + 0x18], esi
00416285  call       0x468e20

// block 0041628a
0041628a  mov        edx, dword ptr [ebx + esi*4 + 0xe0]
00416291  lea        esi, [ebx + esi*4 + 0xe0]
00416298  mov        edi, eax
0041629a  push       edx
0041629b  mov        dword ptr [esp + 0x14], edi
0041629f  call       0x461a70

// block 004162a4
004162a4  mov        ecx, dword ptr [0x4b43dc]
004162aa  mov        dword ptr [esi], 0
004162b0  mov        eax, dword ptr [ebx + 0x220]
004162b6  mov        ecx, dword ptr [ecx + eax*4 + 0x40]
004162ba  push       edi
004162bb  lea        edx, [esp + 0x78]
004162bf  push       edx
004162c0  mov        eax, 8
004162c5  mov        edi, ecx
004162c7  call       0x41c6a0

// block 004162cc
004162cc  mov        eax, dword ptr [eax]
004162ce  mov        dword ptr [esi], eax
004162d0  call       0x461c50

// block 004162d5
004162d5  cmp        dword ptr [esp + 0x14], 0
004162da  jne        0x4162f4

// block 004162dc
004162dc  fld        dword ptr [eax + 0x5c]
004162df  fmul       dword ptr [eax + 0x44]
004162e2  fstp       dword ptr [ebx + 0x15ec]
004162e8  fld        dword ptr [eax + 0x58]
004162eb  fmul       dword ptr [eax + 0x40]
004162ee  fstp       dword ptr [ebx + 0x15f0]

// block 004162f4
004162f4  test       byte ptr [ebx + 0x16b8], 0x20
004162fb  je         0x416304

// block 004162fd
004162fd  mov        eax, esi
004162ff  call       0x461d80

// block 00416304
00416304  cmp        dword ptr [esp + 0x14], 0
00416309  jne        0x41962b

// block 0041630f
0041630f  mov        eax, dword ptr [esp + 0x10]
00416313  mov        ecx, dword ptr [ebx + 0x220]
00416319  or         dword ptr [ebx + 0x16b8], 0x80000
00416323  mov        dword ptr [ebx + 0x22c], eax
00416329  mov        dword ptr [ebx + 0x230], 0
00416333  mov        dword ptr [ebx + 0x228], eax
00416339  mov        dword ptr [ebx + 0x224], ecx
0041633f  jmp        0x41962b

// block 00416344
00416344  mov        edx, dword ptr [ebx + 0xe0]
0041634a  push       edx
0041634b  call       0x461a70

// block 00416350
00416350  mov        ecx, dword ptr [0x4b43dc]
00416356  xor        esi, esi
00416358  mov        dword ptr [ebx + 0xe0], esi
0041635e  mov        eax, dword ptr [ebx + 0x220]
00416364  mov        edi, dword ptr [ecx + eax*4 + 0x40]
00416368  mov        edx, dword ptr [ebx + 0x22c]
0041636e  push       edx
0041636f  lea        eax, [esp + 0x7c]
00416373  push       eax
00416374  lea        eax, [esi + 8]
00416377  call       0x41c6a0

// block 0041637c
0041637c  mov        ecx, dword ptr [eax]
0041637e  mov        edx, dword ptr [ebx + 0x22c]
00416384  mov        eax, dword ptr [ebx + 0x220]
0041638a  and        dword ptr [ebx + 0x16b8], 0xfff7ffff
00416394  mov        dword ptr [ebx + 0xe0], ecx
0041639a  mov        dword ptr [ebx + 0x230], esi
004163a0  mov        dword ptr [ebx + 0x228], edx
004163a6  mov        dword ptr [ebx + 0x224], eax
004163ac  jmp        0x41962b

// block 004163b1
004163b1  mov        edx, dword ptr [esi + 4]
004163b4  push       0
004163b6  call       0x468e20

// block 004163bb
004163bb  mov        ecx, dword ptr [ebx + eax*4 + 0xe0]
004163c2  lea        esi, [ebx + eax*4 + 0xe0]
004163c9  push       ecx
004163ca  mov        dword ptr [esp + 0x14], eax
004163ce  call       0x461a70

// block 004163d3
004163d3  mov        eax, dword ptr [0x4b43dc]
004163d8  mov        dword ptr [esi], 0
004163de  mov        edx, dword ptr [ebx + 0x220]
004163e4  mov        ecx, dword ptr [ebx + 0x22c]
004163ea  mov        edi, dword ptr [eax + edx*4 + 0x40]
004163ee  add        ecx, 5
004163f1  push       ecx
004163f2  lea        edx, [esp + 0x74]
004163f6  push       edx
004163f7  mov        eax, 8
004163fc  call       0x41c6a0

// block 00416401
00416401  mov        eax, dword ptr [eax]
00416403  mov        dword ptr [esi], eax
00416405  call       0x461c50

// block 0041640a
0041640a  cmp        dword ptr [esp + 0x10], 0
0041640f  jmp        0x415bbe

// block 00416414
00416414  mov        edx, dword ptr [esi + 4]
00416417  push       0
00416419  call       0x468e20

// block 0041641e
0041641e  mov        ecx, dword ptr [ebx + eax*4 + 0xe0]
00416425  lea        esi, [ebx + eax*4 + 0xe0]
0041642c  push       ecx
0041642d  mov        dword ptr [esp + 0x14], eax
00416431  call       0x461a70

// block 00416436
00416436  mov        dword ptr [esi], 0
0041643c  mov        eax, dword ptr [ebx + 0x174c]
00416442  mov        edx, dword ptr [eax + 4]
00416445  push       1
00416447  call       0x468e20

// block 0041644c
0041644c  mov        edx, dword ptr [ebx + 0x220]
00416452  mov        ecx, dword ptr [0x4b43dc]
00416458  mov        edi, dword ptr [ecx + edx*4 + 0x40]
0041645c  mov        edx, dword ptr [ebx + 0x22c]
00416462  lea        eax, [edx + eax + 5]
00416466  push       eax
00416467  lea        ecx, [esp + 0x80]
0041646e  push       ecx
0041646f  mov        eax, 8
00416474  call       0x41c6a0

// block 00416479
00416479  mov        edx, dword ptr [eax]
0041647b  mov        dword ptr [esi], edx
0041647d  call       0x461c50

// block 00416482
00416482  cmp        dword ptr [esp + 0x10], 0
00416487  jmp        0x415bbe

// block 0041648c
0041648c  mov        edx, dword ptr [esi + 4]
0041648f  push       0
00416491  call       0x468e20

// block 00416496
00416496  mov        esi, eax
00416498  mov        eax, dword ptr [ebx + 0x174c]
0041649e  mov        edx, dword ptr [eax + 4]
004164a1  push       1
004164a3  call       0x468e20

// block 004164a8
004164a8  mov        ecx, dword ptr [ebx + esi*4 + 0xe0]
004164af  movzx      eax, ax
004164b2  push       ecx
004164b3  mov        ebx, eax
004164b5  call       0x461970

// block 004164ba
004164ba  jmp        0x41962b

// block 004164bf
004164bf  fld        dword ptr [ebx + 0x9c]
004164c5  fadd       dword ptr [ebx + 0x68]
004164c8  fstp       dword ptr [ebx + 0x68]
004164cb  fld        dword ptr [ebx + 0xa0]
004164d1  fadd       dword ptr [ebx + 0x6c]
004164d4  fstp       dword ptr [ebx + 0x6c]
004164d7  fld        dword ptr [ebx + 0xa4]
004164dd  fadd       dword ptr [ebx + 0x70]
004164e0  fstp       dword ptr [ebx + 0x70]
004164e3  mov        edx, dword ptr [0x4ce8d0]
004164e9  fldz       
004164eb  mov        dword ptr [ebx + 0x9c], edx
004164f1  mov        eax, dword ptr [0x4ce8d4]
004164f6  mov        dword ptr [ebx + 0xa0], eax
004164fc  mov        ecx, dword ptr [0x4ce8d8]
00416502  fst        dword ptr [ebx + 0xb4]
00416508  fstp       dword ptr [ebx + 0x80]
0041650e  mov        dword ptr [ebx + 0xa4], ecx
00416514  mov        edx, dword ptr [0x4ce8d0]
0041651a  mov        dword ptr [ebx + 0xa8], edx
00416520  mov        eax, dword ptr [0x4ce8d4]
00416525  mov        dword ptr [ebx + 0xac], eax
0041652b  mov        ecx, dword ptr [0x4ce8d8]
00416531  mov        dword ptr [ebx + 0xb0], ecx
00416537  mov        edx, dword ptr [0x4ce8d0]
0041653d  mov        dword ptr [ebx + 0x74], edx
00416540  mov        eax, dword ptr [0x4ce8d4]
00416545  mov        dword ptr [ebx + 0x78], eax
00416548  mov        ecx, dword ptr [0x4ce8d8]
0041654e  mov        dword ptr [ebx + 0x7c], ecx
00416551  mov        eax, 0xfffffffc
00416556  and        dword ptr [ebx + 0xcc], eax
0041655c  and        dword ptr [ebx + 0x98], eax
00416562  mov        eax, 0xfffffffd
00416567  and        dword ptr [ebx + 0xcc], eax
0041656d  and        dword ptr [ebx + 0x98], eax
00416573  xor        eax, eax
00416575  mov        dword ptr [ebx + 0x2d0], eax
0041657b  mov        dword ptr [ebx + 0x31c], eax
00416581  mov        dword ptr [ebx + 0x358], eax
00416587  mov        dword ptr [ebx + 0x394], eax
0041658d  mov        dword ptr [ebx + 0x3d0], eax
00416593  mov        dword ptr [ebx + 0x40c], eax
00416599  mov        dword ptr [ebx + 0x448], eax
0041659f  mov        dword ptr [ebx + 0x484], eax
004165a5  jmp        0x41962b

// block 004165aa
004165aa  mov        edx, 0x12c
004165af  lea        edi, [ebx + 0x68]
004165b2  cmp        ax, dx
004165b5  je         0x4165bd

// block 004165b7
004165b7  lea        edi, [ebx + 0x9c]

// block 004165bd
004165bd  mov        eax, dword ptr [esi + 4]
004165c0  xor        ecx, ecx
004165c2  call       0x468ec0

// block 004165c7
004165c7  fstp       dword ptr [esp + 0x10]
004165cb  mov        eax, dword ptr [ebx + 0x174c]
004165d1  mov        eax, dword ptr [eax + 4]
004165d4  mov        ecx, 1
004165d9  call       0x468ec0

// block 004165de
004165de  fstp       dword ptr [esp + 0x14]
004165e2  fld        dword ptr [esp + 0x10]
004165e6  fld        qword ptr [0x4a4440]
004165ec  fcom       st(1)
004165ee  fnstsw     ax
004165f0  test       ah, 5
004165f3  jp         0x4165fb

// block 004165f5
004165f5  fxch       st(1)
004165f7  fstp       dword ptr [edi]
004165f9  jmp        0x4165fd

// block 004165fb
004165fb  fstp       st(1)

// block 004165fd
004165fd  fld        dword ptr [esp + 0x14]
00416601  fcom       st(1)
00416603  fnstsw     ax
00416605  fstp       st(1)
00416607  test       ah, 0x41
0041660a  jne        0x416611

// block 0041660c
0041660c  fstp       dword ptr [edi + 4]
0041660f  jmp        0x416613

// block 00416611
00416611  fstp       st(0)

// block 00416613
00416613  and        dword ptr [edi + 0x30], 0xfffffffc
00416617  fld        dword ptr [ebx + 0x9c]
0041661d  fadd       dword ptr [ebx + 0x68]
00416620  mov        esi, ebx
00416622  fstp       dword ptr [esp + 0x24]
00416626  mov        eax, dword ptr [esp + 0x24]
0041662a  fld        dword ptr [ebx + 0xa0]
00416630  fadd       dword ptr [ebx + 0x6c]
00416633  fstp       dword ptr [esp + 0x28]
00416637  mov        ecx, dword ptr [esp + 0x28]
0041663b  fld        dword ptr [ebx + 0xa4]
00416641  fadd       dword ptr [ebx + 0x70]
00416644  mov        dword ptr [ebx + 0x34], eax
00416647  mov        dword ptr [ebx + 0x38], ecx
0041664a  fstp       dword ptr [esp + 0x2c]
0041664e  mov        edx, dword ptr [esp + 0x2c]
00416652  mov        dword ptr [ebx + 0x3c], edx
00416655  call       0x413700

// block 0041665a
0041665a  jmp        0x41962b

// block 0041665f
0041665f  mov        ecx, 0x13c
00416664  lea        edi, [ebx + 0x68]
00416667  cmp        ax, cx
0041666a  je         0x416672

// block 0041666c
0041666c  lea        edi, [ebx + 0x9c]

// block 00416672
00416672  mov        eax, dword ptr [esi + 4]
00416675  xor        ecx, ecx
00416677  call       0x468ec0

// block 0041667c
0041667c  fstp       dword ptr [esp + 0x10]
00416680  mov        eax, dword ptr [ebx + 0x174c]
00416686  mov        eax, dword ptr [eax + 4]
00416689  mov        ecx, 1
0041668e  call       0x468ec0

// block 00416693
00416693  fstp       dword ptr [esp + 0x14]
00416697  mov        eax, dword ptr [ebx + 0x174c]
0041669d  mov        eax, dword ptr [eax + 4]
004166a0  mov        ecx, 2
004166a5  call       0x468ec0

// block 004166aa
004166aa  fstp       dword ptr [esp + 0x38]
004166ae  fld        dword ptr [esp + 0x10]
004166b2  mov        esi, ebx
004166b4  fadd       dword ptr [edi]
004166b6  fstp       dword ptr [edi]
004166b8  fld        dword ptr [edi + 4]
004166bb  fadd       dword ptr [esp + 0x14]
004166bf  fstp       dword ptr [edi + 4]
004166c2  fld        dword ptr [esp + 0x38]
004166c6  fadd       dword ptr [edi + 8]
004166c9  fstp       dword ptr [edi + 8]
004166cc  call       0x413700

// block 004166d1
004166d1  jmp        0x41962b

// block 004166d6
004166d6  mov        edx, 0x12d
004166db  cmp        ax, dx
004166de  jne        0x4166e9

// block 004166e0
004166e0  lea        ecx, [ebx + 0x68]
004166e3  mov        dword ptr [esp + 0x14], ecx
004166e7  jmp        0x4166f3

// block 004166e9
004166e9  lea        edx, [ebx + 0x9c]
004166ef  mov        dword ptr [esp + 0x14], edx

// block 004166f3
004166f3  mov        ecx, 0x12d
004166f8  lea        edi, [ebx + 0x28c]
004166fe  cmp        ax, cx
00416701  je         0x416709

// block 00416703
00416703  lea        edi, [ebx + 0x2d8]

// block 00416709
00416709  mov        eax, dword ptr [esi + 4]
0041670c  mov        ecx, 2
00416711  call       0x468ec0

// block 00416716
00416716  fstp       dword ptr [esp + 0x3c]
0041671a  mov        eax, dword ptr [ebx + 0x174c]
00416720  mov        eax, dword ptr [eax + 4]
00416723  mov        ecx, 3
00416728  call       0x468ec0

// block 0041672d
0041672d  fstp       dword ptr [esp + 0x10]
00416731  mov        eax, dword ptr [ebx + 0x174c]
00416737  mov        edx, dword ptr [eax + 4]
0041673a  push       0
0041673c  call       0x468e20

// block 00416741
00416741  test       eax, eax
00416743  jg         0x416751

// block 00416745
00416745  mov        dword ptr [edi + 0x44], 0
0041674c  jmp        0x41962b

// block 00416751
00416751  mov        eax, dword ptr [ebx + 0x174c]
00416757  mov        edx, dword ptr [eax + 4]
0041675a  push       0
0041675c  call       0x468e20

// block 00416761
00416761  mov        dword ptr [edi + 0x44], eax
00416764  mov        edx, dword ptr [0x4ce8d0]
0041676a  mov        dword ptr [edi + 0x18], edx
0041676d  mov        eax, dword ptr [0x4ce8d4]
00416772  mov        dword ptr [edi + 0x1c], eax
00416775  mov        ecx, dword ptr [0x4ce8d8]
0041677b  mov        dword ptr [edi + 0x20], ecx
0041677e  mov        edx, dword ptr [0x4ce8d0]
00416784  mov        dword ptr [edi + 0x24], edx
00416787  mov        eax, dword ptr [0x4ce8d4]
0041678c  mov        dword ptr [edi + 0x28], eax
0041678f  mov        ecx, dword ptr [0x4ce8d8]
00416795  mov        dword ptr [edi + 0x2c], ecx
00416798  mov        ebx, dword ptr [ebx + 0x174c]
0041679e  mov        edx, dword ptr [ebx + 4]
004167a1  push       1
004167a3  call       0x468e20

// block 004167a8
004167a8  fld        dword ptr [esp + 0x10]
004167ac  mov        esi, dword ptr [esp + 0x14]
004167b0  fld        qword ptr [0x4a4440]
004167b6  mov        dword ptr [edi + 0x48], eax
004167b9  fcom       st(1)
004167bb  mov        edx, dword ptr [esi]
004167bd  mov        dword ptr [edi], edx
004167bf  mov        eax, dword ptr [esi + 4]
004167c2  mov        dword ptr [edi + 4], eax
004167c5  fnstsw     ax
004167c7  mov        ecx, dword ptr [esi + 8]
004167ca  mov        dword ptr [edi + 8], ecx
004167cd  test       ah, 5
004167d0  jp         0x4167d6

// block 004167d2
004167d2  fxch       st(1)
004167d4  jmp        0x4167db

// block 004167d6
004167d6  fstp       st(1)
004167d8  fld        dword ptr [esi + 4]

// block 004167db
004167db  fstp       dword ptr [esp + 0x38]
004167df  fld        dword ptr [esp + 0x3c]
004167e3  fcom       st(1)
004167e5  fnstsw     ax
004167e7  fstp       st(1)
004167e9  test       ah, 0x41
004167ec  je         0x4167f2

// block 004167ee
004167ee  fstp       st(0)
004167f0  fld        dword ptr [esi]

// block 004167f2
004167f2  fstp       dword ptr [esp + 0x14]
004167f6  fld        dword ptr [esp + 0x14]
004167fa  fstp       dword ptr [esp + 0x24]
004167fe  mov        edx, dword ptr [esp + 0x24]
00416802  fld        dword ptr [esp + 0x38]
00416806  mov        dword ptr [edi + 0xc], edx
00416809  fstp       dword ptr [esp + 0x28]
0041680d  mov        eax, dword ptr [esp + 0x28]
00416811  fldz       
00416813  mov        dword ptr [edi + 0x10], eax
00416816  fstp       dword ptr [esp + 0x2c]
0041681a  mov        eax, edi
0041681c  mov        ecx, dword ptr [esp + 0x2c]
00416820  mov        dword ptr [edi + 0x14], ecx
00416823  call       0x406340

// block 00416828
00416828  and        dword ptr [esi + 0x30], 0xfffffffc
0041682c  jmp        0x41962b

// block 00416831
00416831  mov        edx, 0x145
00416836  cmp        ax, dx
00416839  jne        0x416844

// block 0041683b
0041683b  lea        ecx, [ebx + 0x68]
0041683e  mov        dword ptr [esp + 0x14], ecx
00416842  jmp        0x41684e

// block 00416844
00416844  lea        edx, [ebx + 0x9c]
0041684a  mov        dword ptr [esp + 0x14], edx

// block 0041684e
0041684e  mov        ecx, 0x145
00416853  lea        edi, [ebx + 0x28c]
00416859  cmp        ax, cx
0041685c  je         0x416864

// block 0041685e
0041685e  lea        edi, [ebx + 0x2d8]

// block 00416864
00416864  mov        eax, dword ptr [esi + 4]
00416867  mov        ecx, 3
0041686c  call       0x468ec0

// block 00416871
00416871  fstp       dword ptr [esp + 0x3c]
00416875  mov        eax, dword ptr [ebx + 0x174c]
0041687b  mov        eax, dword ptr [eax + 4]
0041687e  mov        ecx, 4
00416883  call       0x468ec0

// block 00416888
00416888  fstp       dword ptr [esp + 0x10]
0041688c  mov        eax, dword ptr [ebx + 0x174c]
00416892  mov        eax, dword ptr [eax + 4]
00416895  mov        ecx, 1
0041689a  call       0x468ec0

// block 0041689f
0041689f  fstp       dword ptr [esp + 0x24]
004168a3  mov        eax, dword ptr [ebx + 0x174c]
004168a9  mov        eax, dword ptr [eax + 4]
004168ac  mov        ecx, 2
004168b1  call       0x468ec0

// block 004168b6
004168b6  fstp       dword ptr [esp + 0x28]
004168ba  mov        eax, dword ptr [ebx + 0x174c]
004168c0  fldz       
004168c2  mov        eax, dword ptr [eax + 4]
004168c5  fstp       dword ptr [esp + 0x2c]
004168c9  mov        ecx, 5
004168ce  call       0x468ec0

// block 004168d3
004168d3  fstp       dword ptr [esp + 0x5c]
004168d7  mov        eax, dword ptr [ebx + 0x174c]
004168dd  mov        eax, dword ptr [eax + 4]
004168e0  mov        ecx, 6
004168e5  call       0x468ec0

// block 004168ea
004168ea  fstp       dword ptr [esp + 0x60]
004168ee  mov        ebx, dword ptr [ebx + 0x174c]
004168f4  fldz       
004168f6  mov        edx, dword ptr [ebx + 4]
004168f9  fstp       dword ptr [esp + 0x64]
004168fd  push       0
004168ff  call       0x468e20

// block 00416904
00416904  fld        dword ptr [esp + 0x10]
00416908  mov        edx, dword ptr [esp + 0x24]
0041690c  fld        qword ptr [0x4a4440]
00416912  mov        ecx, dword ptr [esp + 0x2c]
00416916  fcom       st(1)
00416918  mov        esi, dword ptr [esp + 0x14]
0041691c  mov        dword ptr [edi + 0x44], eax
0041691f  mov        eax, dword ptr [esp + 0x28]
00416923  mov        dword ptr [edi + 0x18], edx
00416926  mov        edx, dword ptr [esp + 0x5c]
0041692a  mov        dword ptr [edi + 0x1c], eax
0041692d  mov        eax, dword ptr [esp + 0x60]
00416931  mov        dword ptr [edi + 0x20], ecx
00416934  mov        ecx, dword ptr [esp + 0x64]
00416938  mov        dword ptr [edi + 0x24], edx
0041693b  mov        dword ptr [edi + 0x28], eax
0041693e  mov        dword ptr [edi + 0x2c], ecx
00416941  mov        dword ptr [edi + 0x48], 8
00416948  mov        edx, dword ptr [esi]
0041694a  mov        dword ptr [edi], edx
0041694c  mov        eax, dword ptr [esi + 4]
0041694f  mov        dword ptr [edi + 4], eax
00416952  fnstsw     ax
00416954  mov        ecx, dword ptr [esi + 8]
00416957  mov        dword ptr [edi + 8], ecx
0041695a  test       ah, 5
0041695d  jp         0x416963

// block 0041695f
0041695f  fxch       st(1)
00416961  jmp        0x416968

// block 00416963
00416963  fstp       st(1)
00416965  fld        dword ptr [esi + 4]

// block 00416968
00416968  fstp       dword ptr [esp + 0x38]
0041696c  fld        dword ptr [esp + 0x3c]
00416970  fcom       st(1)
00416972  fnstsw     ax
00416974  fstp       st(1)
00416976  test       ah, 0x41
00416979  je         0x41697f

// block 0041697b
0041697b  fstp       st(0)
0041697d  fld        dword ptr [esi]

// block 0041697f
0041697f  fstp       dword ptr [esp + 0x14]
00416983  fld        dword ptr [esp + 0x14]
00416987  fstp       dword ptr [esp + 0x24]
0041698b  mov        edx, dword ptr [esp + 0x24]
0041698f  fld        dword ptr [esp + 0x38]
00416993  mov        dword ptr [edi + 0xc], edx
00416996  fstp       dword ptr [esp + 0x28]
0041699a  mov        eax, dword ptr [esp + 0x28]
0041699e  fldz       
004169a0  mov        dword ptr [edi + 0x10], eax
004169a3  fstp       dword ptr [esp + 0x2c]
004169a7  mov        eax, edi
004169a9  mov        ecx, dword ptr [esp + 0x2c]
004169ad  mov        dword ptr [edi + 0x14], ecx
004169b0  call       0x406340

// block 004169b5
004169b5  and        dword ptr [esi + 0x30], 0xfffffffc
004169b9  jmp        0x41962b

// block 004169be
004169be  mov        edx, 0x130
004169c3  cmp        ax, dx
004169c6  je         0x4169de

// block 004169c8
004169c8  mov        ecx, 0x148
004169cd  cmp        ax, cx
004169d0  je         0x4169de

// block 004169d2
004169d2  lea        edx, [ebx + 0x9c]
004169d8  mov        dword ptr [esp + 0x18], edx
004169dc  jmp        0x4169e5

// block 004169de
004169de  lea        eax, [ebx + 0x68]
004169e1  mov        dword ptr [esp + 0x18], eax

// block 004169e5
004169e5  mov        eax, dword ptr [esi + 4]
004169e8  xor        ecx, ecx
004169ea  call       0x468ec0

// block 004169ef
004169ef  fstp       dword ptr [esp + 0x34]
004169f3  mov        eax, dword ptr [ebx + 0x174c]
004169f9  mov        eax, dword ptr [eax + 4]
004169fc  mov        ecx, 1
00416a01  call       0x468ec0

// block 00416a06
00416a06  fstp       dword ptr [esp + 0x14]
00416a0a  fld        dword ptr [esp + 0x34]
00416a0e  fcom       qword ptr [0x4a4440]
00416a14  fnstsw     ax
00416a16  test       ah, 0x41
00416a19  jne        0x416a85

// block 00416a1b
00416a1b  test       dword ptr [ebx + 0x16b8], 0x40000
00416a25  je         0x416a75

// block 00416a27
00416a27  movzx      eax, word ptr [edi + 4]
00416a2b  mov        ecx, 0x130
00416a30  cmp        ax, cx
00416a33  je         0x416a3f

// block 00416a35
00416a35  mov        edx, 0x132
00416a3a  cmp        ax, dx
00416a3d  jne        0x416a75

// block 00416a3f
00416a3f  fsub       qword ptr [0x4a4058]
00416a45  push       ecx
00416a46  fstp       dword ptr [esp + 0x14]
00416a4a  fld        dword ptr [esp + 0x14]
00416a4e  fstp       dword ptr [esp]
00416a51  call       0x4646e0

// block 00416a56
00416a56  fsubr      qword ptr [0x4a4058]
00416a5c  push       ecx
00416a5d  fstp       dword ptr [esp + 0x14]
00416a61  fld        dword ptr [esp + 0x14]
00416a65  fstp       dword ptr [esp]
00416a68  call       0x4646e0

// block 00416a6d
00416a6d  fstp       dword ptr [esp + 0x34]
00416a71  fld        dword ptr [esp + 0x34]

// block 00416a75
00416a75  mov        esi, dword ptr [esp + 0x18]
00416a79  push       ecx
00416a7a  fstp       dword ptr [esp]
00416a7d  push       esi
00416a7e  call       0x408910

// block 00416a83
00416a83  jmp        0x416a8b

// block 00416a85
00416a85  mov        esi, dword ptr [esp + 0x18]
00416a89  fstp       st(0)

// block 00416a8b
00416a8b  fld        dword ptr [esp + 0x14]
00416a8f  fcom       qword ptr [0x4a4440]
00416a95  fnstsw     ax
00416a97  test       ah, 0x41
00416a9a  jne        0x416aa8

// block 00416a9c
00416a9c  and        dword ptr [esi + 0x30], 0xfffffffc
00416aa0  fstp       dword ptr [esi + 0x18]
00416aa3  jmp        0x41962b

// block 00416aa8
00416aa8  and        dword ptr [esi + 0x30], 0xfffffffc
00416aac  fstp       st(0)
00416aae  jmp        0x41962b

// block 00416ab3
00416ab3  mov        ecx, 0x131
00416ab8  cmp        ax, cx
00416abb  je         0x416ad3

// block 00416abd
00416abd  mov        edx, 0x149
00416ac2  cmp        ax, dx
00416ac5  je         0x416ad3

// block 00416ac7
00416ac7  lea        ecx, [ebx + 0x9c]
00416acd  mov        dword ptr [esp + 0x34], ecx
00416ad1  jmp        0x416ada

// block 00416ad3
00416ad3  lea        edx, [ebx + 0x68]
00416ad6  mov        dword ptr [esp + 0x34], edx

// block 00416ada
00416ada  mov        ecx, 0x131
00416adf  cmp        ax, cx
00416ae2  je         0x416af4

// block 00416ae4
00416ae4  mov        edx, 0x149
00416ae9  lea        edi, [ebx + 0x360]
00416aef  cmp        ax, dx
00416af2  jne        0x416afa

// block 00416af4
00416af4  lea        edi, [ebx + 0x324]

// block 00416afa
00416afa  mov        eax, dword ptr [esi + 4]
00416afd  mov        ecx, 2
00416b02  call       0x468ec0

// block 00416b07
00416b07  fstp       dword ptr [esp + 0x14]
00416b0b  mov        eax, dword ptr [ebx + 0x174c]
00416b11  mov        eax, dword ptr [eax + 4]
00416b14  mov        ecx, 3
00416b19  call       0x468ec0

// block 00416b1e
00416b1e  fstp       dword ptr [esp + 0x38]
00416b22  mov        eax, dword ptr [ebx + 0x174c]
00416b28  mov        edx, dword ptr [eax + 4]
00416b2b  push       0
00416b2d  call       0x468e20

// block 00416b32
00416b32  test       eax, eax
00416b34  jg         0x416b42

// block 00416b36
00416b36  mov        dword ptr [edi + 0x34], 0
00416b3d  jmp        0x41962b

// block 00416b42
00416b42  mov        eax, dword ptr [ebx + 0x174c]
00416b48  mov        edx, dword ptr [eax + 4]
00416b4b  push       1
00416b4d  call       0x468e20

// block 00416b52
00416b52  fld        dword ptr [esp + 0x14]
00416b56  mov        dword ptr [edi + 0x38], eax
00416b59  cmp        eax, 7
00416b5c  je         0x416c00

// block 00416b62
00416b62  fcom       qword ptr [0x4a4440]
00416b68  fnstsw     ax
00416b6a  test       ah, 0x41
00416b6d  jne        0x416bcb

// block 00416b6f
00416b6f  test       dword ptr [ebx + 0x16b8], 0x40000
00416b79  je         0x416bc5

// block 00416b7b
00416b7b  mov        eax, dword ptr [esp + 0x18]
00416b7f  movzx      eax, word ptr [eax + 4]
00416b83  mov        ecx, 0x131
00416b88  cmp        ax, cx
00416b8b  je         0x416b97

// block 00416b8d
00416b8d  mov        edx, 0x133
00416b92  cmp        ax, dx
00416b95  jne        0x416bc5

// block 00416b97
00416b97  fsub       qword ptr [0x4a4058]
00416b9d  push       ecx
00416b9e  fstp       dword ptr [esp + 0x14]
00416ba2  fld        dword ptr [esp + 0x14]
00416ba6  fstp       dword ptr [esp]
00416ba9  call       0x4646e0

// block 00416bae
00416bae  fsubr      qword ptr [0x4a4058]
00416bb4  push       ecx
00416bb5  fstp       dword ptr [esp + 0x14]
00416bb9  fld        dword ptr [esp + 0x14]
00416bbd  fstp       dword ptr [esp]
00416bc0  call       0x4646e0

// block 00416bc5
00416bc5  mov        esi, dword ptr [esp + 0x34]
00416bc9  jmp        0x416bd4

// block 00416bcb
00416bcb  mov        esi, dword ptr [esp + 0x34]
00416bcf  fstp       st(0)
00416bd1  fld        dword ptr [esi + 0x1c]

// block 00416bd4
00416bd4  fstp       dword ptr [esp + 0x18]
00416bd8  fld        dword ptr [esp + 0x18]
00416bdc  fstp       dword ptr [esp + 0x1c]
00416be0  fld        dword ptr [esp + 0x38]
00416be4  fcom       qword ptr [0x4a4440]
00416bea  fnstsw     ax
00416bec  test       ah, 0x41
00416bef  je         0x416bf6

// block 00416bf1
00416bf1  fstp       st(0)
00416bf3  fld        dword ptr [esi + 0x18]

// block 00416bf6
00416bf6  fstp       dword ptr [esp + 0x14]
00416bfa  fld        dword ptr [esp + 0x14]
00416bfe  jmp        0x416c3c

// block 00416c00
00416c00  fld        qword ptr [0x4a4440]
00416c06  fcom       st(1)
00416c08  fnstsw     ax
00416c0a  fldz       
00416c0c  test       ah, 5
00416c0f  jp         0x416c19

// block 00416c11
00416c11  fxch       st(2)
00416c13  fstp       dword ptr [esp + 0x1c]
00416c17  jmp        0x416c23

// block 00416c19
00416c19  fstp       st(2)
00416c1b  fxch       st(1)
00416c1d  fst        dword ptr [esp + 0x1c]
00416c21  fxch       st(1)

// block 00416c23
00416c23  fld        dword ptr [esp + 0x38]
00416c27  mov        esi, dword ptr [esp + 0x34]
00416c2b  fcom       st(1)
00416c2d  fnstsw     ax
00416c2f  fstp       st(1)
00416c31  test       ah, 0x41
00416c34  jne        0x416c3a

// block 00416c36
00416c36  fstp       st(1)
00416c38  jmp        0x416c3c

// block 00416c3a
00416c3a  fstp       st(0)

// block 00416c3c
00416c3c  fstp       dword ptr [esp + 0x20]
00416c40  fld        dword ptr [esi + 0x1c]
00416c43  fstp       dword ptr [esp + 0x40]
00416c47  fld        dword ptr [esi + 0x18]
00416c4a  fstp       dword ptr [esp + 0x44]
00416c4e  fld        dword ptr [esp + 0x40]
00416c52  fld        st(0)
00416c54  fld        dword ptr [esp + 0x1c]
00416c58  fld        st(0)
00416c5a  fsubp      st(2)
00416c5c  fxch       st(1)
00416c5e  fstp       dword ptr [esp + 0x10]
00416c62  fld        dword ptr [esp + 0x10]
00416c66  fabs       
00416c68  fstp       dword ptr [esp + 0x10]
00416c6c  fld        dword ptr [esp + 0x10]
00416c70  fcomp      qword ptr [0x4a3cd8]
00416c76  fnstsw     ax
00416c78  test       ah, 1
00416c7b  jne        0x416ca2

// block 00416c7d
00416c7d  fcom       st(1)
00416c7f  fnstsw     ax
00416c81  test       ah, 0x41
00416c84  jne        0x416c94

// block 00416c86
00416c86  fstp       st(0)
00416c88  fadd       qword ptr [0x4a3cd0]
00416c8e  fstp       dword ptr [esp + 0x40]
00416c92  jmp        0x416ca6

// block 00416c94
00416c94  fstp       st(1)
00416c96  fadd       qword ptr [0x4a3cd0]
00416c9c  fstp       dword ptr [esp + 0x1c]
00416ca0  jmp        0x416ca6

// block 00416ca2
00416ca2  fstp       st(1)
00416ca4  fstp       st(0)

// block 00416ca6
00416ca6  mov        ebx, dword ptr [ebx + 0x174c]
00416cac  mov        edx, dword ptr [ebx + 4]
00416caf  push       0
00416cb1  call       0x468e20

// block 00416cb6
00416cb6  mov        dword ptr [edi + 0x34], eax
00416cb9  mov        eax, dword ptr [0x4ce8dc]
00416cbe  mov        dword ptr [edi + 0x10], eax
00416cc1  mov        ecx, dword ptr [0x4ce8e0]
00416cc7  mov        dword ptr [edi + 0x14], ecx
00416cca  mov        edx, dword ptr [0x4ce8dc]
00416cd0  mov        ecx, dword ptr [esp + 0x40]
00416cd4  mov        dword ptr [edi + 0x18], edx
00416cd7  mov        eax, dword ptr [0x4ce8e0]
00416cdc  mov        edx, dword ptr [esp + 0x44]
00416ce0  mov        dword ptr [edi + 0x1c], eax
00416ce3  mov        eax, dword ptr [esp + 0x1c]
00416ce7  mov        dword ptr [edi], ecx
00416ce9  mov        ecx, dword ptr [esp + 0x20]
00416ced  mov        dword ptr [edi + 8], eax
00416cf0  mov        eax, edi
00416cf2  mov        dword ptr [edi + 4], edx
00416cf5  mov        dword ptr [edi + 0xc], ecx
00416cf8  call       0x41c350

// block 00416cfd
00416cfd  and        dword ptr [esi + 0x30], 0xfffffffc
00416d01  jmp        0x41962b

// block 00416d06
00416d06  mov        edx, 0x134
00416d0b  lea        edi, [ebx + 0x68]
00416d0e  cmp        ax, dx
00416d11  je         0x416d19

// block 00416d13
00416d13  lea        edi, [ebx + 0x9c]

// block 00416d19
00416d19  mov        eax, dword ptr [esi + 4]
00416d1c  xor        ecx, ecx
00416d1e  call       0x468ec0

// block 00416d23
00416d23  fstp       dword ptr [esp + 0x10]
00416d27  mov        eax, dword ptr [ebx + 0x174c]
00416d2d  mov        eax, dword ptr [eax + 4]
00416d30  mov        ecx, 1
00416d35  call       0x468ec0

// block 00416d3a
00416d3a  fstp       dword ptr [esp + 0x14]
00416d3e  mov        eax, dword ptr [ebx + 0x174c]
00416d44  mov        eax, dword ptr [eax + 4]
00416d47  mov        ecx, 2
00416d4c  call       0x468ec0

// block 00416d51
00416d51  fstp       dword ptr [esp + 0x38]
00416d55  mov        ebx, dword ptr [ebx + 0x174c]
00416d5b  mov        eax, dword ptr [ebx + 4]
00416d5e  mov        ecx, 3
00416d63  call       0x468ec0

// block 00416d68
00416d68  fstp       dword ptr [esp + 0x3c]
00416d6c  test       byte ptr [edi + 0x30], 1
00416d70  jne        0x416d83

// block 00416d72
00416d72  mov        eax, dword ptr [edi]
00416d74  mov        ecx, dword ptr [edi + 4]
00416d77  mov        edx, dword ptr [edi + 8]
00416d7a  mov        dword ptr [edi + 0xc], eax
00416d7d  mov        dword ptr [edi + 0x10], ecx
00416d80  mov        dword ptr [edi + 0x14], edx

// block 00416d83
00416d83  fld        dword ptr [esp + 0x10]
00416d87  fld        qword ptr [0x4a4440]
00416d8d  fcom       st(1)
00416d8f  fnstsw     ax
00416d91  test       ah, 5
00416d94  jp         0x416daa

// block 00416d96
00416d96  push       ecx
00416d97  fstp       st(0)
00416d99  fstp       dword ptr [esp]
00416d9c  push       edi
00416d9d  call       0x408910

// block 00416da2
00416da2  fld        qword ptr [0x4a4440]
00416da8  jmp        0x416dac

// block 00416daa
00416daa  fstp       st(1)

// block 00416dac
00416dac  fld        dword ptr [esp + 0x14]
00416db0  fcom       st(1)
00416db2  fnstsw     ax
00416db4  test       ah, 0x41
00416db7  jne        0x416dbe

// block 00416db9
00416db9  fstp       dword ptr [edi + 0x18]
00416dbc  jmp        0x416dc0

// block 00416dbe
00416dbe  fstp       st(0)

// block 00416dc0
00416dc0  fld        dword ptr [esp + 0x38]
00416dc4  fcom       st(1)
00416dc6  fnstsw     ax
00416dc8  test       ah, 0x41
00416dcb  jne        0x416dd2

// block 00416dcd
00416dcd  fstp       dword ptr [edi + 0x20]
00416dd0  jmp        0x416dd4

// block 00416dd2
00416dd2  fstp       st(0)

// block 00416dd4
00416dd4  fld        dword ptr [esp + 0x3c]
00416dd8  fcom       st(1)
00416dda  fnstsw     ax
00416ddc  fstp       st(1)
00416dde  test       ah, 0x41
00416de1  jne        0x416de8

// block 00416de3
00416de3  fstp       dword ptr [edi + 0x24]
00416de6  jmp        0x416dea

// block 00416de8
00416de8  fstp       st(0)

// block 00416dea
00416dea  or         dword ptr [edi + 0x30], 1
00416dee  mov        ebx, edi
00416df0  call       0x464eb0

// block 00416df5
00416df5  mov        esi, dword ptr [esp + 0x34]
00416df9  call       0x413700

// block 00416dfe
00416dfe  jmp        0x41962b

// block 00416e03
00416e03  mov        ecx, 0x135
00416e08  cmp        ax, cx
00416e0b  jne        0x416e16

// block 00416e0d
00416e0d  lea        edx, [ebx + 0x68]
00416e10  mov        dword ptr [esp + 0x18], edx
00416e14  jmp        0x416e20

// block 00416e16
00416e16  lea        ecx, [ebx + 0x9c]
00416e1c  mov        dword ptr [esp + 0x18], ecx

// block 00416e20
00416e20  mov        edx, 0x135
00416e25  cmp        ax, dx
00416e28  jne        0x416e36

// block 00416e2a
00416e2a  lea        ecx, [ebx + 0x324]
00416e30  mov        dword ptr [esp + 0x3c], ecx
00416e34  jmp        0x416e40

// block 00416e36
00416e36  lea        edx, [ebx + 0x360]
00416e3c  mov        dword ptr [esp + 0x3c], edx

// block 00416e40
00416e40  mov        ecx, 0x135
00416e45  lea        edi, [ebx + 0x39c]
00416e4b  cmp        ax, cx
00416e4e  je         0x416e56

// block 00416e50
00416e50  lea        edi, [ebx + 0x3d8]

// block 00416e56
00416e56  mov        eax, dword ptr [esi + 4]
00416e59  mov        ecx, 2
00416e5e  call       0x468ec0

// block 00416e63
00416e63  fstp       dword ptr [esp + 0x10]
00416e67  mov        eax, dword ptr [ebx + 0x174c]
00416e6d  mov        eax, dword ptr [eax + 4]
00416e70  mov        ecx, 3
00416e75  call       0x468ec0

// block 00416e7a
00416e7a  fstp       dword ptr [esp + 0x4c]
00416e7e  mov        eax, dword ptr [ebx + 0x174c]
00416e84  mov        eax, dword ptr [eax + 4]
00416e87  mov        ecx, 4
00416e8c  call       0x468ec0

// block 00416e91
00416e91  fstp       dword ptr [esp + 0x38]
00416e95  fld        dword ptr [esp + 0x10]
00416e99  mov        esi, dword ptr [esp + 0x18]
00416e9d  fld        qword ptr [0x4a4440]
00416ea3  fcom       st(1)
00416ea5  fnstsw     ax
00416ea7  test       ah, 5
00416eaa  jp         0x416eb0

// block 00416eac
00416eac  fxch       st(1)
00416eae  jmp        0x416eb5

// block 00416eb0
00416eb0  fstp       st(1)
00416eb2  fld        dword ptr [esi + 0x18]

// block 00416eb5
00416eb5  fstp       dword ptr [esp + 0x14]
00416eb9  fldz       
00416ebb  fst        dword ptr [esp + 0x40]
00416ebf  fld        dword ptr [esp + 0x14]
00416ec3  fstp       dword ptr [esp + 0x44]
00416ec7  fstp       dword ptr [esp + 0x1c]
00416ecb  fld        dword ptr [esi + 0x18]
00416ece  fstp       dword ptr [esp + 0x20]
00416ed2  fld        dword ptr [esp + 0x38]
00416ed6  fcom       st(1)
00416ed8  fnstsw     ax
00416eda  test       ah, 0x41
00416edd  je         0x416ee4

// block 00416edf
00416edf  fstp       st(0)
00416ee1  fld        dword ptr [esi + 0x24]

// block 00416ee4
00416ee4  fstp       dword ptr [esp + 0x38]
00416ee8  fld        dword ptr [esp + 0x4c]
00416eec  fcom       st(1)
00416eee  fnstsw     ax
00416ef0  fstp       st(1)
00416ef2  test       ah, 0x41
00416ef5  je         0x416efc

// block 00416ef7
00416ef7  fstp       st(0)
00416ef9  fld        dword ptr [esi + 0x20]

// block 00416efc
00416efc  mov        eax, dword ptr [ebx + 0x174c]
00416f02  fstp       dword ptr [esp + 0x14]
00416f06  fld        dword ptr [esp + 0x14]
00416f0a  mov        edx, dword ptr [eax + 4]
00416f0d  fstp       dword ptr [esp + 0x4c]
00416f11  push       0
00416f13  fld        dword ptr [esp + 0x3c]
00416f17  fstp       dword ptr [esp + 0x54]
00416f1b  fld        dword ptr [esi + 0x20]
00416f1e  fstp       dword ptr [esp + 0x58]
00416f22  fld        dword ptr [esi + 0x24]
00416f25  fstp       dword ptr [esp + 0x5c]
00416f29  call       0x468e20

// block 00416f2e
00416f2e  mov        ebx, dword ptr [ebx + 0x174c]
00416f34  mov        edx, dword ptr [ebx + 4]
00416f37  push       1
00416f39  mov        dword ptr [esp + 0x18], eax
00416f3d  call       0x468e20

// block 00416f42
00416f42  mov        ecx, dword ptr [esp + 0x14]
00416f46  mov        ebx, eax
00416f48  xor        eax, eax
00416f4a  cmp        ecx, eax
00416f4c  jg         0x416f5d

// block 00416f4e
00416f4e  mov        edx, dword ptr [esp + 0x3c]
00416f52  mov        dword ptr [edx + 0x34], eax
00416f55  mov        dword ptr [edi + 0x34], eax
00416f58  jmp        0x41962b

// block 00416f5d
00416f5d  mov        eax, dword ptr [esp + 0x3c]
00416f61  mov        dword ptr [eax + 0x34], ecx
00416f64  mov        ecx, dword ptr [0x4ce8dc]
00416f6a  mov        dword ptr [eax + 0x10], ecx
00416f6d  mov        edx, dword ptr [0x4ce8e0]
00416f73  mov        dword ptr [eax + 0x14], edx
00416f76  mov        ecx, dword ptr [0x4ce8dc]
00416f7c  mov        dword ptr [eax + 0x18], ecx
00416f7f  mov        edx, dword ptr [0x4ce8e0]
00416f85  mov        ecx, dword ptr [esp + 0x1c]
00416f89  mov        dword ptr [eax + 0x1c], edx
00416f8c  mov        edx, dword ptr [esp + 0x20]
00416f90  mov        dword ptr [eax], ecx
00416f92  mov        ecx, dword ptr [esp + 0x40]
00416f96  mov        dword ptr [eax + 4], edx
00416f99  mov        edx, dword ptr [esp + 0x44]
00416f9d  mov        dword ptr [eax + 0x38], ebx
00416fa0  mov        dword ptr [eax + 8], ecx
00416fa3  mov        dword ptr [eax + 0xc], edx
00416fa6  call       0x41c350

// block 00416fab
00416fab  mov        eax, dword ptr [esp + 0x14]
00416faf  mov        dword ptr [edi + 0x34], eax
00416fb2  mov        ecx, dword ptr [0x4ce8dc]
00416fb8  mov        dword ptr [edi + 0x10], ecx
00416fbb  mov        edx, dword ptr [0x4ce8e0]
00416fc1  mov        dword ptr [edi + 0x14], edx
00416fc4  mov        eax, dword ptr [0x4ce8dc]
00416fc9  mov        edx, dword ptr [esp + 0x54]
00416fcd  mov        dword ptr [edi + 0x18], eax
00416fd0  mov        ecx, dword ptr [0x4ce8e0]
00416fd6  mov        eax, dword ptr [esp + 0x58]
00416fda  mov        dword ptr [edi + 0x1c], ecx
00416fdd  mov        ecx, dword ptr [esp + 0x4c]
00416fe1  mov        dword ptr [edi], edx
00416fe3  mov        edx, dword ptr [esp + 0x50]
00416fe7  mov        dword ptr [edi + 4], eax
00416fea  mov        eax, edi
00416fec  mov        dword ptr [edi + 0x38], ebx
00416fef  mov        dword ptr [edi + 8], ecx
00416ff2  mov        dword ptr [edi + 0xc], edx
00416ff5  call       0x41c350

// block 00416ffa
00416ffa  or         dword ptr [esi + 0x30], 1
00416ffe  mov        ebx, esi
00417000  call       0x464eb0

// block 00417005
00417005  mov        esi, dword ptr [esp + 0x34]
00417009  call       0x413700

// block 0041700e
0041700e  jmp        0x41962b

// block 00417013
00417013  mov        ecx, 0x13e
00417018  lea        edi, [ebx + 0x68]
0041701b  cmp        ax, cx
0041701e  je         0x417026

// block 00417020
00417020  lea        edi, [ebx + 0x9c]

// block 00417026
00417026  mov        eax, dword ptr [esi + 4]
00417029  xor        ecx, ecx
0041702b  call       0x468ec0

// block 00417030
00417030  fstp       dword ptr [esp + 0x4c]
00417034  mov        ebx, dword ptr [ebx + 0x174c]
0041703a  mov        eax, dword ptr [ebx + 4]
0041703d  mov        ecx, 1
00417042  call       0x468ec0

// block 00417047
00417047  fstp       dword ptr [esp + 0x10]
0041704b  fld        dword ptr [esp + 0x4c]
0041704f  fld        qword ptr [0x4a4440]
00417055  fcom       st(1)
00417057  fnstsw     ax
00417059  test       ah, 5
0041705c  jp         0x417065

// block 0041705e
0041705e  fxch       st(1)
00417060  fstp       dword ptr [edi + 0xc]
00417063  jmp        0x417067

// block 00417065
00417065  fstp       st(1)

// block 00417067
00417067  fld        dword ptr [esp + 0x10]
0041706b  fcom       st(1)
0041706d  fnstsw     ax
0041706f  fstp       st(1)
00417071  test       ah, 0x41
00417074  jne        0x41707b

// block 00417076
00417076  fstp       dword ptr [edi + 0x10]
00417079  jmp        0x41707d

// block 0041707b
0041707b  fstp       st(0)

// block 0041707d
0041707d  mov        ebx, edi
0041707f  call       0x464eb0

// block 00417084
00417084  mov        esi, dword ptr [esp + 0x34]
00417088  call       0x413700

// block 0041708d
0041708d  mov        edi, dword ptr [esp + 0x18]
00417091  mov        ebx, esi

// block 00417093
00417093  mov        edx, 0x140
00417098  lea        esi, [ebx + 0x68]
0041709b  cmp        word ptr [edi + 4], dx
0041709f  je         0x4170a7

// block 004170a1
004170a1  lea        esi, [ebx + 0x9c]

// block 004170a7
004170a7  mov        eax, dword ptr [ebx + 0x174c]
004170ad  mov        eax, dword ptr [eax + 4]
004170b0  xor        ecx, ecx
004170b2  call       0x468ec0

// block 004170b7
004170b7  fstp       dword ptr [esp + 0x4c]
004170bb  mov        eax, dword ptr [ebx + 0x174c]
004170c1  mov        eax, dword ptr [eax + 4]
004170c4  mov        ecx, 1
004170c9  call       0x468ec0

// block 004170ce
004170ce  fstp       dword ptr [esp + 0x10]
004170d2  mov        eax, dword ptr [ebx + 0x174c]
004170d8  mov        eax, dword ptr [eax + 4]
004170db  mov        ecx, 2
004170e0  call       0x468ec0

// block 004170e5
004170e5  fstp       dword ptr [esp + 0x14]
004170e9  mov        eax, dword ptr [ebx + 0x174c]
004170ef  mov        eax, dword ptr [eax + 4]
004170f2  mov        ecx, 3
004170f7  call       0x468ec0

// block 004170fc
004170fc  fstp       dword ptr [esp + 0x38]
00417100  mov        eax, dword ptr [ebx + 0x174c]
00417106  mov        eax, dword ptr [eax + 4]
00417109  mov        ecx, 4
0041710e  call       0x468ec0

// block 00417113
00417113  fstp       dword ptr [esp + 0x3c]
00417117  mov        ebx, dword ptr [ebx + 0x174c]
0041711d  mov        eax, dword ptr [ebx + 4]
00417120  mov        ecx, 5
00417125  call       0x468ec0

// block 0041712a
0041712a  fstp       dword ptr [esp + 0x18]
0041712e  test       byte ptr [esi + 0x30], 1
00417132  jne        0x417145

// block 00417134
00417134  mov        eax, dword ptr [esi]
00417136  mov        ecx, dword ptr [esi + 4]
00417139  mov        edx, dword ptr [esi + 8]
0041713c  mov        dword ptr [esi + 0xc], eax
0041713f  mov        dword ptr [esi + 0x10], ecx
00417142  mov        dword ptr [esi + 0x14], edx

// block 00417145
00417145  fld        dword ptr [esp + 0x4c]
00417149  fld        qword ptr [0x4a4440]
0041714f  fcom       st(1)
00417151  fnstsw     ax
00417153  test       ah, 5
00417156  jp         0x41716c

// block 00417158
00417158  push       ecx
00417159  fstp       st(0)
0041715b  fstp       dword ptr [esp]
0041715e  push       esi
0041715f  call       0x408910

// block 00417164
00417164  fld        qword ptr [0x4a4440]
0041716a  jmp        0x41716e

// block 0041716c
0041716c  fstp       st(1)

// block 0041716e
0041716e  fld        dword ptr [esp + 0x10]
00417172  fcom       st(1)
00417174  fnstsw     ax
00417176  test       ah, 0x41
00417179  jne        0x417180

// block 0041717b
0041717b  fstp       dword ptr [esi + 0x18]
0041717e  jmp        0x417182

// block 00417180
00417180  fstp       st(0)

// block 00417182
00417182  fld        dword ptr [esp + 0x14]
00417186  fcom       st(1)
00417188  fnstsw     ax
0041718a  test       ah, 0x41
0041718d  jne        0x417194

// block 0041718f
0041718f  fstp       dword ptr [esi + 0x20]
00417192  jmp        0x417196

// block 00417194
00417194  fstp       st(0)

// block 00417196
00417196  fld        dword ptr [esp + 0x38]
0041719a  fcom       st(1)
0041719c  fnstsw     ax
0041719e  test       ah, 0x41
004171a1  jne        0x4171a8

// block 004171a3
004171a3  fstp       dword ptr [esi + 0x24]
004171a6  jmp        0x4171aa

// block 004171a8
004171a8  fstp       st(0)

// block 004171aa
004171aa  fld        dword ptr [esp + 0x3c]
004171ae  fcom       st(1)
004171b0  fnstsw     ax
004171b2  test       ah, 0x41
004171b5  jne        0x4171cb

// block 004171b7
004171b7  push       ecx
004171b8  fstp       st(1)
004171ba  fstp       dword ptr [esp]
004171bd  push       esi
004171be  call       0x41c5e0

// block 004171c3
004171c3  fld        qword ptr [0x4a4440]
004171c9  jmp        0x4171cd

// block 004171cb
004171cb  fstp       st(0)

// block 004171cd
004171cd  fld        dword ptr [esp + 0x18]
004171d1  fcom       st(1)
004171d3  fnstsw     ax
004171d5  fstp       st(1)
004171d7  test       ah, 0x41
004171da  jne        0x4171e1

// block 004171dc
004171dc  fstp       dword ptr [esi + 0x2c]
004171df  jmp        0x4171e3

// block 004171e1
004171e1  fstp       st(0)

// block 004171e3
004171e3  or         dword ptr [esi + 0x30], 3
004171e7  mov        ebx, esi
004171e9  call       0x464eb0

// block 004171ee
004171ee  mov        esi, dword ptr [esp + 0x34]
004171f2  call       0x413700

// block 004171f7
004171f7  jmp        0x41962b

// block 004171fc
004171fc  mov        ecx, 0x141
00417201  lea        edi, [ebx + 0x68]
00417204  cmp        ax, cx
00417207  je         0x41720f

// block 00417209
00417209  lea        edi, [ebx + 0x9c]

// block 0041720f
0041720f  mov        edx, ecx
00417211  cmp        ax, dx
00417214  jne        0x417222

// block 00417216
00417216  lea        ecx, [ebx + 0x324]
0041721c  mov        dword ptr [esp + 0x3c], ecx
00417220  jmp        0x41722c

// block 00417222
00417222  lea        edx, [ebx + 0x360]
00417228  mov        dword ptr [esp + 0x3c], edx

// block 0041722c
0041722c  mov        ecx, 0x141
00417231  cmp        ax, cx
00417234  jne        0x417242

// block 00417236
00417236  lea        edx, [ebx + 0x39c]
0041723c  mov        dword ptr [esp + 0x18], edx
00417240  jmp        0x41724c

// block 00417242
00417242  lea        ecx, [ebx + 0x3d8]
00417248  mov        dword ptr [esp + 0x18], ecx

// block 0041724c
0041724c  mov        edx, 0x141
00417251  cmp        ax, dx
00417254  jne        0x417262

// block 00417256
00417256  lea        eax, [ebx + 0x414]
0041725c  mov        dword ptr [esp + 0x38], eax
00417260  jmp        0x41726c

// block 00417262
00417262  lea        ecx, [ebx + 0x450]
00417268  mov        dword ptr [esp + 0x38], ecx

// block 0041726c
0041726c  mov        eax, dword ptr [esi + 4]
0041726f  mov        ecx, 2
00417274  call       0x468ec0

// block 00417279
00417279  fstp       dword ptr [esp + 0x4c]
0041727d  mov        eax, dword ptr [ebx + 0x174c]
00417283  mov        eax, dword ptr [eax + 4]
00417286  mov        ecx, 3
0041728b  call       0x468ec0

// block 00417290
00417290  fstp       dword ptr [esp + 0x54]
00417294  mov        eax, dword ptr [ebx + 0x174c]
0041729a  mov        eax, dword ptr [eax + 4]
0041729d  mov        ecx, 4
004172a2  call       0x468ec0

// block 004172a7
004172a7  fstp       dword ptr [esp + 0x10]
004172ab  mov        eax, dword ptr [ebx + 0x174c]
004172b1  mov        eax, dword ptr [eax + 4]
004172b4  mov        ecx, 5
004172b9  call       0x468ec0

// block 004172be
004172be  fstp       dword ptr [esp + 0x1c]
004172c2  mov        eax, dword ptr [ebx + 0x174c]
004172c8  mov        eax, dword ptr [eax + 4]
004172cb  mov        ecx, 6
004172d0  call       0x468ec0

// block 004172d5
004172d5  fstp       dword ptr [esp + 0x40]
004172d9  fld        dword ptr [esp + 0x4c]
004172dd  fld        qword ptr [0x4a4440]
004172e3  fcom       st(1)
004172e5  fnstsw     ax
004172e7  test       ah, 5
004172ea  jp         0x4172f0

// block 004172ec
004172ec  fxch       st(1)
004172ee  jmp        0x4172f5

// block 004172f0
004172f0  fstp       st(1)
004172f2  fld        dword ptr [edi + 0x18]

// block 004172f5
004172f5  fstp       dword ptr [esp + 0x14]
004172f9  fldz       
004172fb  fst        dword ptr [esp + 0x5c]
004172ff  fld        dword ptr [esp + 0x14]
00417303  fstp       dword ptr [esp + 0x60]
00417307  fstp       dword ptr [esp + 0x4c]
0041730b  fld        dword ptr [edi + 0x18]
0041730e  fstp       dword ptr [esp + 0x50]
00417312  fld        dword ptr [esp + 0x10]
00417316  fcom       st(1)
00417318  fnstsw     ax
0041731a  test       ah, 0x41
0041731d  je         0x417324

// block 0041731f
0041731f  fstp       st(0)
00417321  fld        dword ptr [edi + 0x24]

// block 00417324
00417324  fstp       dword ptr [esp + 0x10]
00417328  fld        dword ptr [esp + 0x54]
0041732c  fcom       st(1)
0041732e  fnstsw     ax
00417330  test       ah, 0x41
00417333  je         0x41733a

// block 00417335
00417335  fstp       st(0)
00417337  fld        dword ptr [edi + 0x20]

// block 0041733a
0041733a  fstp       dword ptr [esp + 0x14]
0041733e  fld        dword ptr [esp + 0x14]
00417342  fstp       dword ptr [esp + 0x24]
00417346  fld        dword ptr [esp + 0x10]
0041734a  fstp       dword ptr [esp + 0x28]
0041734e  fld        dword ptr [edi + 0x20]
00417351  fstp       dword ptr [esp + 0x54]
00417355  fld        dword ptr [edi + 0x24]
00417358  fstp       dword ptr [esp + 0x58]
0041735c  fld        dword ptr [esp + 0x40]
00417360  fcom       st(1)
00417362  fnstsw     ax
00417364  test       ah, 0x41
00417367  je         0x41736e

// block 00417369
00417369  fstp       st(0)
0041736b  fld        dword ptr [edi + 0x2c]

// block 0041736e
0041736e  fstp       dword ptr [esp + 0x14]
00417372  fld        dword ptr [esp + 0x1c]
00417376  fcom       st(1)
00417378  fnstsw     ax
0041737a  fstp       st(1)
0041737c  test       ah, 0x41
0041737f  je         0x417386

// block 00417381
00417381  fstp       st(0)
00417383  fld        dword ptr [edi + 0x28]

// block 00417386
00417386  fstp       dword ptr [esp + 0x10]
0041738a  xor        ecx, ecx
0041738c  fld        dword ptr [esp + 0x10]
00417390  mov        eax, ebx
00417392  fstp       dword ptr [esp + 0x40]
00417396  fld        dword ptr [esp + 0x14]
0041739a  fstp       dword ptr [esp + 0x44]
0041739e  fld        dword ptr [edi + 0x28]
004173a1  fstp       dword ptr [esp + 0x1c]
004173a5  fld        dword ptr [edi + 0x2c]
004173a8  fstp       dword ptr [esp + 0x20]
004173ac  call       0x41a900

// block 004173b1
004173b1  mov        esi, eax
004173b3  mov        ecx, 1
004173b8  mov        eax, ebx
004173ba  call       0x41a900

// block 004173bf
004173bf  mov        dword ptr [esp + 0x14], eax
004173c3  xor        eax, eax
004173c5  cmp        esi, eax
004173c7  jg         0x4173e3

// block 004173c9
004173c9  mov        edx, dword ptr [esp + 0x3c]
004173cd  mov        ecx, dword ptr [esp + 0x18]
004173d1  mov        dword ptr [edx + 0x34], eax
004173d4  mov        edx, dword ptr [esp + 0x38]
004173d8  mov        dword ptr [ecx + 0x34], eax
004173db  mov        dword ptr [edx + 0x34], eax
004173de  jmp        0x41962b

// block 004173e3
004173e3  mov        ebx, dword ptr [esp + 0x3c]
004173e7  mov        eax, ebx
004173e9  mov        dword ptr [ebx + 0x34], esi
004173ec  call       0x41c300

// block 004173f1
004173f1  mov        eax, ebx
004173f3  call       0x41c320

// block 004173f8
004173f8  mov        eax, dword ptr [esp + 0x14]
004173fc  mov        ecx, dword ptr [esp + 0x4c]
00417400  mov        edx, dword ptr [esp + 0x50]
00417404  mov        dword ptr [ebx + 0x38], eax
00417407  mov        eax, dword ptr [esp + 0x5c]
0041740b  mov        dword ptr [ebx], ecx
0041740d  mov        ecx, dword ptr [esp + 0x60]
00417411  mov        dword ptr [ebx + 8], eax
00417414  mov        eax, ebx
00417416  mov        dword ptr [ebx + 4], edx
00417419  mov        dword ptr [ebx + 0xc], ecx
0041741c  call       0x41c350

// block 00417421
00417421  mov        ebx, dword ptr [esp + 0x18]
00417425  mov        eax, ebx
00417427  mov        dword ptr [ebx + 0x34], esi
0041742a  call       0x41c300

// block 0041742f
0041742f  mov        eax, ebx
00417431  call       0x41c320

// block 00417436
00417436  mov        eax, dword ptr [esp + 0x54]
0041743a  mov        edx, dword ptr [esp + 0x14]
0041743e  mov        ecx, dword ptr [esp + 0x58]
00417442  mov        dword ptr [ebx], eax
00417444  mov        eax, dword ptr [esp + 0x28]
00417448  mov        dword ptr [ebx + 0x38], edx
0041744b  mov        edx, dword ptr [esp + 0x24]
0041744f  mov        dword ptr [ebx + 0xc], eax
00417452  mov        eax, ebx
00417454  mov        dword ptr [ebx + 4], ecx
00417457  mov        dword ptr [ebx + 8], edx
0041745a  call       0x41c350

// block 0041745f
0041745f  mov        ebx, dword ptr [esp + 0x38]
00417463  mov        eax, ebx
00417465  mov        dword ptr [ebx + 0x34], esi
00417468  call       0x41c300

// block 0041746d
0041746d  mov        eax, ebx
0041746f  call       0x41c320

// block 00417474
00417474  mov        ecx, dword ptr [esp + 0x14]
00417478  mov        edx, dword ptr [esp + 0x1c]
0041747c  mov        eax, dword ptr [esp + 0x20]
00417480  mov        dword ptr [ebx + 0x38], ecx
00417483  mov        ecx, dword ptr [esp + 0x40]
00417487  mov        dword ptr [ebx], edx
00417489  mov        edx, dword ptr [esp + 0x44]
0041748d  mov        dword ptr [ebx + 4], eax
00417490  mov        eax, ebx
00417492  mov        dword ptr [ebx + 8], ecx
00417495  mov        dword ptr [ebx + 0xc], edx
00417498  call       0x41c350

// block 0041749d
0041749d  mov        eax, dword ptr [edi]
0041749f  mov        ecx, dword ptr [edi + 4]
004174a2  mov        edx, dword ptr [edi + 8]
004174a5  or         dword ptr [edi + 0x30], 3
004174a9  mov        dword ptr [edi + 0xc], eax
004174ac  mov        dword ptr [edi + 0x10], ecx
004174af  mov        ebx, edi
004174b1  mov        dword ptr [edi + 0x14], edx
004174b4  call       0x464eb0

// block 004174b9
004174b9  mov        esi, dword ptr [esp + 0x34]
004174bd  call       0x413700

// block 004174c2
004174c2  jmp        0x41962b

// block 004174c7
004174c7  mov        eax, dword ptr [0x4b43dc]
004174cc  mov        eax, dword ptr [eax + 0x1c]
004174cf  lea        ecx, [ebx + 0x68]

// block 004174d2
004174d2  add        eax, 0x1074

// block 004174d7
004174d7  mov        edx, dword ptr [eax]
004174d9  mov        dword ptr [ecx], edx
004174db  mov        edx, dword ptr [eax + 4]
004174de  mov        dword ptr [ecx + 4], edx
004174e1  mov        eax, dword ptr [eax + 8]
004174e4  mov        dword ptr [ecx + 8], eax
004174e7  jmp        0x41962b

// block 004174ec
004174ec  mov        ecx, dword ptr [0x4b43dc]
004174f2  mov        eax, dword ptr [ecx + 0x1c]
004174f5  lea        ecx, [ebx + 0x9c]
004174fb  jmp        0x4174d2

// block 004174fd
004174fd  or         dword ptr [ebx + 0x16b8], 0x10000
00417507  mov        eax, dword ptr [esi + 4]
0041750a  xor        ecx, ecx
0041750c  call       0x468ec0

// block 00417511
00417511  fstp       dword ptr [ebx + 0x15f4]
00417517  mov        eax, dword ptr [ebx + 0x174c]
0041751d  mov        eax, dword ptr [eax + 4]
00417520  mov        ecx, 1
00417525  call       0x468ec0

// block 0041752a
0041752a  fstp       dword ptr [ebx + 0x15f8]
00417530  mov        eax, dword ptr [ebx + 0x174c]
00417536  mov        eax, dword ptr [eax + 4]
00417539  mov        ecx, 2
0041753e  call       0x468ec0

// block 00417543
00417543  fstp       dword ptr [ebx + 0x15fc]
00417549  mov        eax, dword ptr [ebx + 0x174c]
0041754f  mov        eax, dword ptr [eax + 4]
00417552  mov        ecx, 3
00417557  call       0x468ec0

// block 0041755c
0041755c  fstp       dword ptr [ebx + 0x1600]
00417562  jmp        0x41962b

// block 00417567
00417567  and        dword ptr [ebx + 0x16b8], 0xfffeffff
00417571  jmp        0x41962b

// block 00417576
00417576  mov        eax, dword ptr [esi + 4]
00417579  xor        ecx, ecx
0041757b  call       0x468ec0

// block 00417580
00417580  fmul       st(0), st(0)
00417582  fstp       dword ptr [ebx + 0x16c8]
00417588  jmp        0x41962b

// block 0041758d
0041758d  mov        ecx, 0x138
00417592  cmp        ax, cx
00417595  jne        0x4175a0

// block 00417597
00417597  lea        edx, [ebx + 0x68]
0041759a  mov        dword ptr [esp + 0x10], edx
0041759e  jmp        0x4175aa

// block 004175a0
004175a0  lea        ecx, [ebx + 0x9c]
004175a6  mov        dword ptr [esp + 0x10], ecx

// block 004175aa
004175aa  mov        edx, 0x138
004175af  lea        edi, [ebx + 0x324]
004175b5  cmp        ax, dx
004175b8  je         0x4175c0

// block 004175ba
004175ba  lea        edi, [ebx + 0x360]

// block 004175c0
004175c0  fld        dword ptr [ebx + 0x15fc]
004175c6  fmul       qword ptr [0x4a42b8]
004175cc  fld        dword ptr [ebx + 0x34]
004175cf  fld        dword ptr [ebx + 0x15f4]
004175d5  fsub       st(2)
004175d7  fcompp     
004175d9  fnstsw     ax
004175db  test       ah, 0x41
004175de  jne        0x4175f2

// block 004175e0
004175e0  fstp       st(0)
004175e2  call       0x409310

// block 004175e7
004175e7  fdiv       qword ptr [0x4a3fd8]
004175ed  jmp        0x41769d

// block 004175f2
004175f2  fld        dword ptr [ebx + 0x34]
004175f5  fld        dword ptr [ebx + 0x15f4]
004175fb  faddp      st(2)
004175fd  fcompp     
004175ff  fnstsw     ax
00417601  test       ah, 0x41
00417604  jne        0x41762a

// block 00417606
00417606  call       0x409310

// block 0041760b
0041760b  fdiv       qword ptr [0x4a3fd8]
00417611  push       ecx
00417612  fadd       qword ptr [0x4a3cd8]
00417618  fstp       dword ptr [esp + 0x20]
0041761c  fld        dword ptr [esp + 0x20]
00417620  fstp       dword ptr [esp]
00417623  call       0x4646e0

// block 00417628
00417628  jmp        0x41769d

// block 0041762a
0041762a  mov        eax, dword ptr [0x4b4514]
0041762f  fld        dword ptr [ebx + 0x34]
00417632  fld        dword ptr [eax + 0x97c]
00417638  fcompp     
0041763a  fnstsw     ax
0041763c  test       ah, 0x41
0041763f  jne        0x41766b

// block 00417641
00417641  call       0x409310

// block 00417646
00417646  fmul       qword ptr [0x4a42b8]
0041764c  mov        esi, 0x4ce568
00417651  fstp       dword ptr [esp + 0x18]
00417655  call       0x4643d0

// block 0041765a
0041765a  movzx      eax, ax
0041765d  cdq        
0041765e  mov        ecx, 3
00417663  idiv       ecx
00417665  test       edx, edx
00417667  jne        0x4176a1

// block 00417669
00417669  jmp        0x417693

// block 0041766b
0041766b  call       0x409310

// block 00417670
00417670  fmul       qword ptr [0x4a42b8]
00417676  mov        esi, 0x4ce568
0041767b  fstp       dword ptr [esp + 0x18]
0041767f  call       0x4643d0

// block 00417684
00417684  movzx      eax, ax
00417687  cdq        
00417688  mov        ecx, 3
0041768d  idiv       ecx
0041768f  test       edx, edx
00417691  je         0x4176a1

// block 00417693
00417693  fld        dword ptr [esp + 0x18]
00417697  fadd       qword ptr [0x4a3cd8]

// block 0041769d
0041769d  fstp       dword ptr [esp + 0x18]

// block 004176a1
004176a1  fld        dword ptr [ebx + 0x1600]
004176a7  fmul       qword ptr [0x4a42b8]
004176ad  fld        dword ptr [ebx + 0x38]
004176b0  fld        dword ptr [ebx + 0x15f8]
004176b6  fsub       st(2)
004176b8  fcompp     
004176ba  fnstsw     ax
004176bc  test       ah, 0x41
004176bf  jne        0x4176d3

// block 004176c1
004176c1  fstp       st(0)
004176c3  fld        dword ptr [esp + 0x18]
004176c7  fabs       
004176c9  fstp       dword ptr [esp + 0x1c]
004176cd  fld        dword ptr [esp + 0x1c]
004176d1  jmp        0x4176f7

// block 004176d3
004176d3  fld        dword ptr [ebx + 0x38]
004176d6  fld        dword ptr [ebx + 0x15f8]
004176dc  faddp      st(2)
004176de  fcompp     
004176e0  fnstsw     ax
004176e2  test       ah, 0x41
004176e5  jne        0x4176fb

// block 004176e7
004176e7  fld        dword ptr [esp + 0x18]
004176eb  fabs       
004176ed  fstp       dword ptr [esp + 0x1c]
004176f1  fld        dword ptr [esp + 0x1c]
004176f5  fchs       

// block 004176f7
004176f7  fstp       dword ptr [esp + 0x18]

// block 004176fb
004176fb  fld        dword ptr [esp + 0x18]
004176ff  fld        st(0)
00417701  fld        qword ptr [0x4a4058]
00417707  fadd       st(1), st(0)
00417709  fxch       st(1)
0041770b  fstp       dword ptr [esp + 0x1c]
0041770f  fld        dword ptr [esp + 0x1c]
00417713  fabs       
00417715  fstp       dword ptr [esp + 0x1c]
00417719  fld        dword ptr [esp + 0x1c]
0041771d  fcomp      qword ptr [0x4a40e0]
00417723  fnstsw     ax
00417725  test       ah, 5
00417728  jp         0x417751

// block 0041772a
0041772a  fstp       st(0)
0041772c  fcomp      qword ptr [0x4a4438]
00417732  fnstsw     ax
00417734  test       ah, 5
00417737  jp         0x417745

// block 00417739
00417739  fld        dword ptr [0x4a4434]
0041773f  fstp       dword ptr [esp + 0x18]
00417743  jmp        0x417799

// block 00417745
00417745  fld        dword ptr [0x4a4430]
0041774b  fstp       dword ptr [esp + 0x18]
0041774f  jmp        0x417799

// block 00417751
00417751  fld        st(1)
00417753  fsub       st(1)
00417755  fstp       dword ptr [esp + 0x1c]
00417759  fld        dword ptr [esp + 0x1c]
0041775d  fabs       
0041775f  fstp       dword ptr [esp + 0x1c]
00417763  fld        dword ptr [esp + 0x1c]
00417767  fcomp      qword ptr [0x4a4000]
0041776d  fnstsw     ax
0041776f  test       ah, 5
00417772  jp         0x417795

// block 00417774
00417774  fcompp     
00417776  fnstsw     ax
00417778  test       ah, 0x41
0041777b  jne        0x417789

// block 0041777d
0041777d  fld        dword ptr [0x4a442c]
00417783  fstp       dword ptr [esp + 0x18]
00417787  jmp        0x417799

// block 00417789
00417789  fld        dword ptr [0x4a4428]
0041778f  fstp       dword ptr [esp + 0x18]
00417793  jmp        0x417799

// block 00417795
00417795  fstp       st(1)
00417797  fstp       st(0)

// block 00417799
00417799  mov        ecx, 1
0041779e  mov        eax, ebx
004177a0  call       0x41a900

// block 004177a5
004177a5  fld        dword ptr [esp + 0x18]
004177a9  fst        dword ptr [esp + 0x1c]
004177ad  mov        dword ptr [edi + 0x38], eax
004177b0  fldz       
004177b2  mov        eax, dword ptr [ebx + 0x174c]
004177b8  mov        eax, dword ptr [eax + 4]
004177bb  fstp       dword ptr [esp + 0x20]
004177bf  mov        ecx, 2
004177c4  fstp       dword ptr [esp + 0x24]
004177c8  call       0x468ec0

// block 004177cd
004177cd  xor        ecx, ecx
004177cf  fstp       dword ptr [esp + 0x28]
004177d3  mov        eax, ebx
004177d5  call       0x41a900

// block 004177da
004177da  mov        dword ptr [edi + 0x34], eax
004177dd  mov        eax, edi
004177df  call       0x41c300

// block 004177e4
004177e4  mov        eax, edi
004177e6  call       0x41c320

// block 004177eb
004177eb  mov        edx, dword ptr [esp + 0x24]
004177ef  mov        eax, dword ptr [esp + 0x28]
004177f3  mov        ecx, dword ptr [esp + 0x1c]
004177f7  mov        dword ptr [edi], edx
004177f9  mov        edx, dword ptr [esp + 0x20]
004177fd  mov        dword ptr [edi + 4], eax
00417800  mov        eax, edi
00417802  mov        dword ptr [edi + 8], ecx
00417805  mov        dword ptr [edi + 0xc], edx
00417808  call       0x41c350

// block 0041780d
0041780d  mov        eax, dword ptr [esp + 0x10]
00417811  and        dword ptr [eax + 0x30], 0xfffffffc
00417815  jmp        0x41962b

// block 0041781a
0041781a  mov        eax, dword ptr [esi + 4]
0041781d  mov        ecx, 1
00417822  call       0x468ec0

// block 00417827
00417827  fstp       dword ptr [esp + 0x1c]
0041782b  mov        eax, dword ptr [ebx + 0x160c]
00417831  mov        dword ptr [esp + 0x40], eax
00417835  mov        ecx, 2
0041783a  mov        eax, ebx
0041783c  call       0x41a900

// block 00417841
00417841  fld        dword ptr [esp + 0x1c]
00417845  fidiv      dword ptr [esp + 0x40]
00417849  push       ecx
0041784a  mov        esi, eax
0041784c  xor        ecx, ecx
0041784e  mov        eax, ebx
00417850  fstp       dword ptr [esp + 0x20]
00417854  fld        dword ptr [esp + 0x20]
00417858  fstp       dword ptr [esp]
0041785b  call       0x41a900

// block 00417860
00417860  mov        edx, esi
00417862  mov        ecx, eax
00417864  call       0x4126f0

// block 00417869
00417869  jmp        0x41962b

// block 0041786e
0041786e  xor        ecx, ecx
00417870  mov        eax, ebx
00417872  call       0x41a900

// block 00417877
00417877  mov        ecx, dword ptr [eax*4 + 0x4af0fc]
0041787e  mov        dword ptr [ebx + 0x1768], ecx
00417884  jmp        0x41962b

// block 00417889
00417889  xor        ecx, ecx
0041788b  mov        eax, ebx
0041788d  call       0x41a900

// block 00417892
00417892  mov        edx, dword ptr [eax*4 + 0x4af118]
00417899  mov        dword ptr [ebx + 0x176c], edx
0041789f  jmp        0x41962b

// block 004178a4
004178a4  xor        ecx, ecx
004178a6  mov        eax, ebx
004178a8  call       0x41a900

// block 004178ad
004178ad  mov        eax, dword ptr [eax*4 + 0x4af120]
004178b4  mov        dword ptr [ebx + 0x1770], eax
004178ba  jmp        0x41962b

// block 004178bf
004178bf  xor        ecx, ecx
004178c1  mov        eax, ebx
004178c3  call       0x41a900

// block 004178c8
004178c8  mov        edx, dword ptr [eax*4 + 0x4af0fc]
004178cf  mov        ecx, ebx
004178d1  call       edx

// block 004178d3
004178d3  jmp        0x41962b

// block 004178d8
004178d8  mov        eax, dword ptr [esi + 4]
004178db  xor        ecx, ecx
004178dd  call       0x468ec0

// block 004178e2
004178e2  fstp       dword ptr [ebx + 0xd0]
004178e8  mov        eax, dword ptr [ebx + 0x174c]
004178ee  mov        eax, dword ptr [eax + 4]
004178f1  mov        ecx, 1
004178f6  call       0x468ec0

// block 004178fb
004178fb  fstp       dword ptr [ebx + 0xd4]
00417901  jmp        0x41962b

// block 00417906
00417906  mov        eax, dword ptr [esi + 4]
00417909  xor        ecx, ecx
0041790b  call       0x468ec0

// block 00417910
00417910  fstp       dword ptr [ebx + 0xd8]
00417916  mov        eax, dword ptr [ebx + 0x174c]
0041791c  mov        eax, dword ptr [eax + 4]
0041791f  mov        ecx, 1
00417924  call       0x468ec0

// block 00417929
00417929  fstp       dword ptr [ebx + 0xdc]
0041792f  jmp        0x41962b

// block 00417934
00417934  xor        ecx, ecx
00417936  mov        eax, ebx
00417938  call       0x41a900

// block 0041793d
0041793d  or         dword ptr [ebx + 0x16b8], eax
00417943  test       byte ptr [ebx + 0x16b8], 0x20
0041794a  je         0x41962b

// block 00417950
00417950  lea        esi, [ebx + 0xe0]
00417956  mov        edi, 0x10
0041795b  jmp        0x417960

// block 00417960
00417960  mov        eax, esi
00417962  call       0x461d80

// block 00417967
00417967  add        esi, 4
0041796a  sub        edi, 1
0041796d  jne        0x417960

// block 0041796f
0041796f  jmp        0x41962b

// block 00417974
00417974  xor        ecx, ecx
00417976  mov        eax, ebx
00417978  call       0x41a900

// block 0041797d
0041797d  not        eax
0041797f  and        dword ptr [ebx + 0x16b8], eax
00417985  test       byte ptr [ebx + 0x16b8], 0x20
0041798c  jne        0x41962b

// block 00417992
00417992  lea        esi, [ebx + 0xe0]
00417998  mov        edi, 0x10
0041799d  lea        ecx, [ecx]

// block 004179a0
004179a0  mov        eax, esi
004179a2  call       0x461d40

// block 004179a7
004179a7  add        esi, 4
004179aa  sub        edi, 1
004179ad  jne        0x4179a0

// block 004179af
004179af  jmp        0x41962b

// block 004179b4
004179b4  push       0x4c
004179b6  push       0
004179b8  add        ebx, 0x1624
004179be  push       ebx
004179bf  call       0x477420

// block 004179c4
004179c4  add        esp, 0xc
004179c7  jmp        0x41962b

// block 004179cc
004179cc  xor        ecx, ecx
004179ce  mov        eax, ebx
004179d0  call       0x41a900

// block 004179d5
004179d5  mov        esi, eax
004179d7  mov        ecx, 1
004179dc  mov        eax, ebx
004179de  call       0x41a900

// block 004179e3
004179e3  mov        dword ptr [ebx + esi*4 + 0x1620], eax
004179ea  jmp        0x41962b

// block 004179ef
004179ef  mov        eax, dword ptr [esi + 4]
004179f2  mov        ecx, 1
004179f7  call       0x468ec0

// block 004179fc
004179fc  fstp       dword ptr [esp + 0x1c]
00417a00  mov        eax, dword ptr [ebx + 0x174c]
00417a06  mov        eax, dword ptr [eax + 4]
00417a09  xor        ecx, ecx
00417a0b  call       0x468ec0

// block 00417a10
00417a10  fstp       dword ptr [ebx + 0x1670]
00417a16  fld        dword ptr [esp + 0x1c]
00417a1a  fstp       dword ptr [ebx + 0x1674]
00417a20  jmp        0x41962b

// block 00417a25
00417a25  lea        eax, [ebx + 0x34]
00417a28  lea        esi, [ebx + 0x1620]
00417a2e  call       0x412100

// block 00417a33
00417a33  jmp        0x41962b

// block 00417a38
00417a38  xor        ecx, ecx
00417a3a  mov        eax, ebx
00417a3c  call       0x41a900

// block 00417a41
00417a41  mov        dword ptr [ebx + 0x1620], eax
00417a47  jmp        0x41962b

// block 00417a4c
00417a4c  xor        ecx, ecx
00417a4e  mov        eax, ebx
00417a50  call       0x41a900

// block 00417a55
00417a55  test       dword ptr [ebx + 0x16b8], 0x400000
00417a5f  mov        dword ptr [ebx + 0x1608], eax
00417a65  mov        dword ptr [ebx + 0x160c], eax
00417a6b  je         0x417aac

// block 00417a6d
00417a6d  fldz       
00417a6f  push       ecx
00417a70  xor        edx, edx
00417a72  fstp       dword ptr [esp]
00417a75  xor        ecx, ecx
00417a77  call       0x4126f0

// block 00417a7c
00417a7c  fldz       
00417a7e  push       ecx
00417a7f  fstp       dword ptr [esp]
00417a82  xor        edx, edx
00417a84  lea        ecx, [edx + 1]
00417a87  call       0x4126f0

// block 00417a8c
00417a8c  fldz       
00417a8e  push       ecx
00417a8f  fstp       dword ptr [esp]
00417a92  xor        edx, edx
00417a94  lea        ecx, [edx + 2]
00417a97  call       0x4126f0

// block 00417a9c
00417a9c  fldz       
00417a9e  push       ecx
00417a9f  fstp       dword ptr [esp]
00417aa2  xor        edx, edx
00417aa4  lea        ecx, [edx + 3]
00417aa7  call       0x4126f0

// block 00417aac
00417aac  mov        eax, dword ptr [ebx + 0x16b8]
00417ab2  mov        ecx, dword ptr [ebx + 0x1608]
00417ab8  mov        dword ptr [ebx + 0x1610], ecx
00417abe  test       eax, 0x400000
00417ac3  je         0x417ad0

// block 00417ac5
00417ac5  or         eax, 0x20000000
00417aca  mov        dword ptr [ebx + 0x16b8], eax

// block 00417ad0
00417ad0  lea        eax, [ecx*8]
00417ad7  sub        eax, ecx
00417ad9  mov        dword ptr [ebx + 0x1614], eax
00417adf  jmp        0x41962b

// block 00417ae4
00417ae4  xor        ecx, ecx
00417ae6  mov        eax, ebx
00417ae8  call       0x41a900

// block 00417aed
00417aed  push       0
00417aef  mov        esi, eax
00417af1  call       0x4124e0

// block 00417af6
00417af6  test       esi, esi
00417af8  jge        0x417b29

// block 00417afa
00417afa  test       dword ptr [ebx + 0x16b8], 0x400000
00417b04  je         0x417b1a

// block 00417b06
00417b06  mov        ecx, dword ptr [ebx + 0x16c4]
00417b0c  mov        edx, dword ptr [0x4b43dc]
00417b12  mov        dword ptr [edx + ecx*4 + 0x1c], 0

// block 00417b1a
00417b1a  and        dword ptr [ebx + 0x16b8], 0xffbfffff
00417b24  jmp        0x41962b

// block 00417b29
00417b29  mov        eax, dword ptr [ebx + 0x174c]
00417b2f  mov        ecx, dword ptr [0x4b43dc]
00417b35  or         dword ptr [ebx + 0x16b8], 0x400000
00417b3f  mov        dword ptr [ecx + esi*4 + 0x1c], eax
00417b43  mov        dword ptr [ebx + 0x16c4], esi
00417b49  jmp        0x41962b

// block 00417b4e
00417b4e  xor        ecx, ecx
00417b50  mov        eax, ebx
00417b52  call       0x41a900

// block 00417b57
00417b57  push       eax
00417b58  lea        eax, [ebx + 0x1690]
00417b5e  call       0x4067e0

// block 00417b63
00417b63  jmp        0x41962b

// block 00417b68
00417b68  xor        ecx, ecx
00417b6a  mov        eax, ebx
00417b6c  call       0x41a900

// block 00417b71
00417b71  push       eax
00417b72  lea        eax, [ebx + 0x16a4]
00417b78  call       0x4067e0

// block 00417b7d
00417b7d  jmp        0x41962b

// block 00417b82
00417b82  xor        ecx, ecx
00417b84  mov        eax, ebx
00417b86  call       0x41a900

// block 00417b8b
00417b8b  push       eax
00417b8c  call       0x414dc0

// block 00417b91
00417b91  jmp        0x41962b

// block 00417b96
00417b96  mov        eax, dword ptr [0x4b0ca8]
00417b9b  cmp        eax, 4
00417b9e  ja         0x41962b

// block 00417ba4
00417ba4  jmp        dword ptr [eax*4 + 0x4199f4]

// block 00417bab
00417bab  mov        eax, ebx

// block 00417bad
00417bad  call       0x41a910

// block 00417bb2
00417bb2  mov        esi, eax
00417bb4  mov        ecx, 1
00417bb9  mov        eax, ebx
00417bbb  call       0x41a900

// block 00417bc0
00417bc0  mov        dword ptr [esi], eax
00417bc2  jmp        0x41962b

// block 00417bc7
00417bc7  mov        eax, ebx
00417bc9  call       0x41a910

// block 00417bce
00417bce  mov        esi, eax
00417bd0  mov        ecx, 2
00417bd5  mov        eax, ebx
00417bd7  call       0x41a900

// block 00417bdc
00417bdc  mov        dword ptr [esi], eax
00417bde  jmp        0x41962b

// block 00417be3
00417be3  mov        eax, ebx
00417be5  call       0x41a910

// block 00417bea
00417bea  mov        esi, eax
00417bec  mov        ecx, 3
00417bf1  mov        eax, ebx
00417bf3  call       0x41a900

// block 00417bf8
00417bf8  mov        dword ptr [esi], eax
00417bfa  jmp        0x41962b

// block 00417bff
00417bff  mov        eax, ebx
00417c01  call       0x41a910

// block 00417c06
00417c06  mov        esi, eax
00417c08  mov        ecx, 4
00417c0d  mov        eax, ebx
00417c0f  call       0x41a900

// block 00417c14
00417c14  mov        dword ptr [esi], eax
00417c16  jmp        0x41962b

// block 00417c1b
00417c1b  mov        eax, dword ptr [0x4b0ca8]
00417c20  cmp        eax, 4
00417c23  ja         0x41962b

// block 00417c29
00417c29  jmp        dword ptr [eax*4 + 0x419a08]

// block 00417c30
00417c30  mov        ecx, 1

// block 00417c35
00417c35  mov        eax, dword ptr [esi + 4]
00417c38  call       0x468ec0

// block 00417c3d
00417c3d  fstp       dword ptr [esp + 0x1c]
00417c41  mov        ebx, dword ptr [ebx + 0x174c]
00417c47  mov        edx, dword ptr [ebx + 4]
00417c4a  xor        ecx, ecx
00417c4c  call       0x4690b0

// block 00417c51
00417c51  fld        dword ptr [esp + 0x1c]
00417c55  jmp        0x419629

// block 00417c5a
00417c5a  mov        ecx, 2
00417c5f  jmp        0x417c35

// block 00417c61
00417c61  mov        ecx, 3
00417c66  jmp        0x417c35

// block 00417c68
00417c68  mov        ecx, 4
00417c6d  jmp        0x417c35

// block 00417c6f
00417c6f  xor        ecx, ecx
00417c71  mov        eax, ebx
00417c73  call       0x41a900

// block 00417c78
00417c78  mov        edi, eax
00417c7a  mov        edx, edi
00417c7c  imul       edx, edx, 0x214
00417c82  lea        esi, [edx + ebx]
00417c85  push       0x214
00417c8a  lea        eax, [esi + 0x48c]
00417c90  push       0
00417c92  push       eax
00417c93  call       0x477420

// block 00417c98
00417c98  mov        ecx, 1
00417c9d  fldz       
00417c9f  mov        word ptr [esi + 0x684], cx
00417ca6  fst        dword ptr [esi + 0x49c]
00417cac  mov        edx, ecx
00417cae  fld        dword ptr [0x4a4014]
00417cb4  mov        word ptr [esi + 0x686], dx
00417cbb  fstp       dword ptr [esi + 0x4a4]
00417cc1  mov        dword ptr [esi + 0x690], 0x16
00417ccb  mov        dword ptr [esi + 0x694], 0x28
00417cd5  mov        dword ptr [esi + 0x68c], 0x83
00417cdf  lea        eax, [edi + edi*2]
00417ce2  lea        eax, [ebx + eax*4]
00417ce5  lea        ecx, [edi + edi*2 + 0x54c]
00417cec  fst        dword ptr [eax + 0x152c]
00417cf2  lea        edx, [edi + edi*2 + 0x564]
00417cf9  fst        dword ptr [ebx + ecx*4]
00417cfc  add        esp, 0xc
00417cff  fst        dword ptr [eax + 0x158c]
00417d05  fst        dword ptr [ebx + edx*4]
00417d08  fstp       dword ptr [eax + 0x1594]
00417d0e  jmp        0x41962b

// block 00417d13
00417d13  xor        ecx, ecx
00417d15  mov        eax, ebx
00417d17  call       0x41a900

// block 00417d1c
00417d1c  mov        edi, eax
00417d1e  mov        ecx, 1
00417d23  mov        eax, ebx
00417d25  mov        dword ptr [esp + 0x1c], edi
00417d29  call       0x41a900

// block 00417d2e
00417d2e  imul       edi, edi, 0x214
00417d34  mov        ecx, eax
00417d36  imul       ecx, ecx, 0x214
00417d3c  lea        esi, [ecx + ebx + 0x48c]
00417d43  lea        edi, [edi + ebx + 0x48c]
00417d4a  mov        ecx, 0x85

// block 00417d4f
00417d4f  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00417d51
00417d51  mov        ecx, dword ptr [esp + 0x1c]
00417d55  lea        edx, [eax + eax*2]
00417d58  lea        eax, [ebx + edx*4]
00417d5b  mov        edx, dword ptr [eax + 0x152c]
00417d61  lea        ecx, [ecx + ecx*2]
00417d64  lea        ebx, [ebx + ecx*4]
00417d67  mov        dword ptr [ebx + 0x152c], edx
00417d6d  mov        ecx, dword ptr [eax + 0x1530]
00417d73  mov        dword ptr [ebx + 0x1530], ecx
00417d79  mov        edx, dword ptr [eax + 0x1534]
00417d7f  mov        dword ptr [ebx + 0x1534], edx
00417d85  add        eax, 0x158c
00417d8a  lea        ecx, [ebx + 0x158c]
00417d90  jmp        0x4174d7

// block 00417d95
00417d95  xor        ecx, ecx
00417d97  mov        eax, ebx
00417d99  call       0x41a900

// block 00417d9e
00417d9e  mov        edx, eax
00417da0  lea        ecx, [edx + edx*2]
00417da3  imul       edx, edx, 0x214
00417da9  fld        dword ptr [ebx + ecx*4 + 0x1594]
00417db0  fcomp      qword ptr [0x4a40e8]
00417db6  fnstsw     ax
00417db8  lea        ecx, [ebx + ecx*4]
00417dbb  test       ah, 0x41
00417dbe  lea        eax, [edx + ebx]
00417dc1  jne        0x417e11

// block 00417dc3
00417dc3  fld        dword ptr [ecx + 0x158c]
00417dc9  fadd       dword ptr [ecx + 0x152c]
00417dcf  fstp       dword ptr [esp + 0x24]
00417dd3  mov        edx, dword ptr [esp + 0x24]
00417dd7  fld        dword ptr [ecx + 0x1590]
00417ddd  fadd       dword ptr [ecx + 0x1530]
00417de3  fstp       dword ptr [esp + 0x28]
00417de7  fld        dword ptr [ecx + 0x1594]
00417ded  fadd       dword ptr [ecx + 0x1534]
00417df3  mov        ecx, dword ptr [esp + 0x28]
00417df7  mov        dword ptr [eax + 0x490], edx
00417dfd  mov        dword ptr [eax + 0x494], ecx
00417e03  fstp       dword ptr [esp + 0x2c]
00417e07  fldz       
00417e09  fstp       dword ptr [eax + 0x498]
00417e0f  jmp        0x417e56

// block 00417e11
00417e11  fld        dword ptr [ebx + 0x34]
00417e14  fadd       dword ptr [ecx + 0x152c]
00417e1a  fstp       dword ptr [esp + 0x24]
00417e1e  fld        dword ptr [ecx + 0x1530]
00417e24  fadd       dword ptr [ebx + 0x38]
00417e27  fstp       dword ptr [esp + 0x28]
00417e2b  mov        edx, dword ptr [esp + 0x28]
00417e2f  fld        dword ptr [ecx + 0x1534]
00417e35  mov        ecx, dword ptr [esp + 0x24]
00417e39  fadd       dword ptr [ebx + 0x3c]
00417e3c  mov        dword ptr [eax + 0x490], ecx
00417e42  mov        dword ptr [eax + 0x494], edx
00417e48  fstp       dword ptr [esp + 0x2c]
00417e4c  mov        ecx, dword ptr [esp + 0x2c]
00417e50  mov        dword ptr [eax + 0x498], ecx

// block 00417e56
00417e56  fld        dword ptr [ebx + 0x16c8]
00417e5c  mov        edx, dword ptr [0x4b43c8]
00417e62  lea        esi, [eax + 0x48c]
00417e68  fstp       dword ptr [edx + 0x60]
00417e6b  call       0x40b5e0

// block 00417e70
00417e70  fldz       
00417e72  mov        eax, dword ptr [0x4b43c8]
00417e77  fstp       dword ptr [eax + 0x60]
00417e7a  jmp        0x41962b

// block 00417e7f
00417e7f  xor        ecx, ecx
00417e81  mov        eax, ebx
00417e83  call       0x41a900

// block 00417e88
00417e88  imul       eax, eax, 0x214
00417e8e  lea        esi, [eax + ebx]
00417e91  mov        ecx, 1
00417e96  mov        eax, ebx
00417e98  call       0x41a900

// block 00417e9d
00417e9d  mov        word ptr [esi + 0x48c], ax
00417ea4  mov        ecx, 2
00417ea9  mov        eax, ebx
00417eab  call       0x41a900

// block 00417eb0
00417eb0  mov        word ptr [esi + 0x48e], ax
00417eb7  jmp        0x41962b

// block 00417ebc
00417ebc  xor        ecx, ecx
00417ebe  mov        eax, ebx
00417ec0  call       0x41a900

// block 00417ec5
00417ec5  mov        esi, eax
00417ec7  mov        eax, dword ptr [ebx + 0x174c]
00417ecd  mov        eax, dword ptr [eax + 4]
00417ed0  mov        ecx, 1
00417ed5  call       0x468ec0

// block 00417eda
00417eda  lea        ecx, [esi + esi*2]
00417edd  fstp       dword ptr [ebx + ecx*4 + 0x152c]
00417ee4  mov        eax, dword ptr [ebx + 0x174c]
00417eea  mov        eax, dword ptr [eax + 4]
00417eed  mov        ecx, 2
00417ef2  call       0x468ec0

// block 00417ef7
00417ef7  lea        edx, [esi + esi*2 + 0x54c]
00417efe  fstp       dword ptr [ebx + edx*4]
00417f01  jmp        0x41962b

// block 00417f06
00417f06  xor        ecx, ecx
00417f08  mov        eax, ebx
00417f0a  call       0x41a900

// block 00417f0f
00417f0f  mov        esi, eax
00417f11  mov        eax, dword ptr [ebx + 0x174c]
00417f17  mov        eax, dword ptr [eax + 4]
00417f1a  mov        ecx, 1
00417f1f  call       0x468ec0

// block 00417f24
00417f24  fstp       dword ptr [esp + 0x40]
00417f28  mov        eax, dword ptr [ebx + 0x174c]
00417f2e  mov        eax, dword ptr [eax + 4]
00417f31  mov        ecx, 2
00417f36  call       0x468ec0

// block 00417f3b
00417f3b  fstp       dword ptr [esp + 0x1c]
00417f3f  fld        dword ptr [esp + 0x1c]
00417f43  sub        esp, 8
00417f46  fstp       dword ptr [esp + 4]
00417f4a  lea        ecx, [esp + 0x2c]
00417f4e  fld        dword ptr [esp + 0x48]
00417f52  fstp       dword ptr [esp]
00417f55  call       0x41c580

// block 00417f5a
00417f5a  fld        dword ptr [esp + 0x24]
00417f5e  lea        eax, [esi + esi*2]
00417f61  fstp       dword ptr [ebx + eax*4 + 0x152c]
00417f68  lea        ecx, [esi + esi*2 + 0x54c]
00417f6f  fld        dword ptr [esp + 0x28]
00417f73  fstp       dword ptr [ebx + ecx*4]
00417f76  jmp        0x41962b

// block 00417f7b
00417f7b  xor        ecx, ecx
00417f7d  mov        eax, ebx
00417f7f  call       0x41a900

// block 00417f84
00417f84  mov        esi, eax
00417f86  mov        eax, dword ptr [ebx + 0x174c]
00417f8c  mov        eax, dword ptr [eax + 4]
00417f8f  mov        ecx, 1
00417f94  call       0x468ec0

// block 00417f99
00417f99  imul       esi, esi, 0x214
00417f9f  fstp       dword ptr [esi + ebx + 0x4ac]
00417fa6  jmp        0x41962b

// block 00417fab
00417fab  xor        ecx, ecx
00417fad  mov        eax, ebx
00417faf  call       0x41a900

// block 00417fb4
00417fb4  mov        esi, eax
00417fb6  mov        eax, dword ptr [ebx + 0x174c]
00417fbc  mov        eax, dword ptr [eax + 4]
00417fbf  lea        edx, [esi + esi*2]
00417fc2  mov        ecx, 1
00417fc7  lea        edi, [ebx + edx*4]
00417fca  call       0x468ec0

// block 00417fcf
00417fcf  fstp       dword ptr [edi + 0x158c]
00417fd5  mov        eax, dword ptr [ebx + 0x174c]
00417fdb  mov        eax, dword ptr [eax + 4]
00417fde  mov        ecx, 2
00417fe3  call       0x468ec0

// block 00417fe8
00417fe8  lea        eax, [esi + esi*2 + 0x564]
00417fef  fstp       dword ptr [ebx + eax*4]
00417ff2  fld        dword ptr [edi + 0x158c]
00417ff8  fcomp      qword ptr [0x4a3e70]
00417ffe  fnstsw     ax
00418000  test       ah, 5
00418003  jp         0x418012

// block 00418005
00418005  fldz       
00418007  fstp       dword ptr [edi + 0x1594]
0041800d  jmp        0x41962b

// block 00418012
00418012  fld1       
00418014  fstp       dword ptr [edi + 0x1594]
0041801a  jmp        0x41962b

// block 0041801f
0041801f  xor        ecx, ecx
00418021  mov        eax, ebx
00418023  call       0x41a900

// block 00418028
00418028  imul       eax, eax, 0x214
0041802e  mov        edx, dword ptr [ebx + 0x174c]
00418034  lea        esi, [eax + ebx]
00418037  mov        eax, dword ptr [edx + 4]
0041803a  mov        ecx, 1
0041803f  call       0x468ec0

// block 00418044
00418044  mov        ecx, 2
00418049  fstp       dword ptr [esi + 0x49c]
0041804f  mov        ebx, dword ptr [ebx + 0x174c]
00418055  mov        eax, dword ptr [ebx + 4]
00418058  call       0x468ec0

// block 0041805d
0041805d  fstp       dword ptr [esi + 0x4a0]
00418063  jmp        0x41962b

// block 00418068
00418068  xor        ecx, ecx
0041806a  mov        eax, ebx
0041806c  call       0x41a900

// block 00418071
00418071  imul       eax, eax, 0x214
00418077  mov        edx, dword ptr [ebx + 0x174c]
0041807d  lea        esi, [eax + ebx]
00418080  mov        eax, dword ptr [edx + 4]
00418083  mov        ecx, 1
00418088  call       0x468ec0

// block 0041808d
0041808d  mov        ecx, 2
00418092  fstp       dword ptr [esi + 0x4a4]
00418098  mov        ebx, dword ptr [ebx + 0x174c]
0041809e  mov        eax, dword ptr [ebx + 4]
004180a1  call       0x468ec0

// block 004180a6
004180a6  fstp       dword ptr [esi + 0x4a8]
004180ac  jmp        0x41962b

// block 004180b1
004180b1  xor        ecx, ecx
004180b3  mov        eax, ebx
004180b5  call       0x41a900

// block 004180ba
004180ba  imul       eax, eax, 0x214
004180c0  lea        esi, [eax + ebx]
004180c3  mov        eax, ebx

// block 004180c5
004180c5  mov        ecx, 1
004180ca  call       0x41a900

// block 004180cf
004180cf  mov        ecx, 2

// block 004180d4
004180d4  mov        word ptr [esi + 0x684], ax
004180db  mov        eax, ebx
004180dd  call       0x41a900

// block 004180e2
004180e2  mov        word ptr [esi + 0x686], ax
004180e9  jmp        0x41962b

// block 004180ee
004180ee  xor        ecx, ecx
004180f0  mov        eax, ebx
004180f2  call       0x41a900

// block 004180f7
004180f7  mov        esi, eax
004180f9  mov        eax, dword ptr [0x4b0ca8]
004180fe  test       eax, eax
00418100  jne        0x418107

// block 00418102
00418102  lea        ecx, [eax + 1]
00418105  jmp        0x418120

// block 00418107
00418107  cmp        eax, 1
0041810a  jne        0x418111

// block 0041810c
0041810c  lea        ecx, [eax + 1]
0041810f  jmp        0x418120

// block 00418111
00418111  mov        ecx, 3
00418116  cmp        eax, 2
00418119  je         0x418120

// block 0041811b
0041811b  mov        ecx, 4

// block 00418120
00418120  mov        eax, dword ptr [ebx + 0x174c]
00418126  mov        eax, dword ptr [eax + 4]
00418129  call       0x468ec0

// block 0041812e
0041812e  imul       esi, esi, 0x214
00418134  fstp       dword ptr [esp + 0x34]
00418138  fld        dword ptr [esp + 0x34]
0041813c  fstp       dword ptr [esi + ebx + 0x4a4]
00418143  mov        eax, dword ptr [0x4b0ca8]
00418148  add        esi, ebx
0041814a  test       eax, eax
0041814c  jne        0x418153

// block 0041814e
0041814e  lea        ecx, [eax + 5]
00418151  jmp        0x41816c

// block 00418153
00418153  cmp        eax, 1
00418156  jne        0x41815d

// block 00418158
00418158  lea        ecx, [eax + 5]
0041815b  jmp        0x41816c

// block 0041815d
0041815d  mov        ecx, 7
00418162  cmp        eax, 2
00418165  je         0x41816c

// block 00418167
00418167  mov        ecx, 8

// block 0041816c
0041816c  mov        ebx, dword ptr [ebx + 0x174c]
00418172  mov        eax, dword ptr [ebx + 4]
00418175  call       0x468ec0

// block 0041817a
0041817a  fstp       dword ptr [esp + 0x34]
0041817e  fld        dword ptr [esp + 0x34]
00418182  fstp       dword ptr [esi + 0x4a8]
00418188  jmp        0x41962b

// block 0041818d
0041818d  xor        ecx, ecx
0041818f  mov        eax, ebx
00418191  call       0x41a900

// block 00418196
00418196  mov        esi, eax
00418198  mov        eax, dword ptr [0x4b0ca8]
0041819d  test       eax, eax
0041819f  jne        0x4181a6

// block 004181a1
004181a1  lea        ecx, [eax + 1]
004181a4  jmp        0x4181bf

// block 004181a6
004181a6  cmp        eax, 1
004181a9  jne        0x4181b0

// block 004181ab
004181ab  lea        ecx, [eax + 1]
004181ae  jmp        0x4181bf

// block 004181b0
004181b0  mov        ecx, 3
004181b5  cmp        eax, 2
004181b8  je         0x4181bf

// block 004181ba
004181ba  mov        ecx, 4

// block 004181bf
004181bf  mov        eax, ebx
004181c1  call       0x41a900

// block 004181c6
004181c6  imul       esi, esi, 0x214
004181cc  movzx      eax, ax
004181cf  add        esi, ebx
004181d1  mov        word ptr [esi + 0x684], ax
004181d8  mov        eax, dword ptr [0x4b0ca8]
004181dd  test       eax, eax
004181df  jne        0x4181e6

// block 004181e1
004181e1  lea        ecx, [eax + 5]
004181e4  jmp        0x4181ff

// block 004181e6
004181e6  cmp        eax, 1
004181e9  jne        0x4181f0

// block 004181eb
004181eb  lea        ecx, [eax + 5]
004181ee  jmp        0x4181ff

// block 004181f0
004181f0  mov        ecx, 7
004181f5  cmp        eax, 2
004181f8  je         0x4181ff

// block 004181fa
004181fa  mov        ecx, 8

// block 004181ff
004181ff  mov        eax, ebx
00418201  call       0x41a900

// block 00418206
00418206  movzx      eax, ax
00418209  mov        word ptr [esi + 0x686], ax
00418210  jmp        0x41962b

// block 00418215
00418215  xor        ecx, ecx
00418217  mov        eax, ebx
00418219  call       0x41a900

// block 0041821e
0041821e  imul       eax, eax, 0x214
00418224  mov        ecx, dword ptr [0x4b0ccc]
0041822a  cmp        ecx, 0x200
00418230  lea        esi, [eax + ebx]
00418233  mov        eax, ebx
00418235  jl         0x41825e

// block 00418237
00418237  mov        ecx, 5
0041823c  call       0x41a950

// block 00418241
00418241  fstp       dword ptr [esi + 0x4a4]
00418247  mov        ecx, 6
0041824c  mov        eax, ebx
0041824e  call       0x41a950

// block 00418253
00418253  fstp       dword ptr [esi + 0x4a8]
00418259  jmp        0x41962b

// block 0041825e
0041825e  cmp        ecx, 0xfffffe00

// block 00418264
00418264  jl         0x41828d

// block 00418266
00418266  mov        ecx, 3
0041826b  call       0x41a950

// block 00418270
00418270  mov        ecx, 4

// block 00418275
00418275  mov        eax, ebx
00418277  fstp       dword ptr [esi + 0x4a4]
0041827d  call       0x41a950

// block 00418282
00418282  fstp       dword ptr [esi + 0x4a8]
00418288  jmp        0x41962b

// block 0041828d
0041828d  mov        ecx, 1
00418292  call       0x41a950

// block 00418297
00418297  fstp       dword ptr [esi + 0x4a4]
0041829d  mov        ecx, 2
004182a2  mov        eax, ebx
004182a4  call       0x41a950

// block 004182a9
004182a9  fstp       dword ptr [esi + 0x4a8]
004182af  jmp        0x41962b

// block 004182b4
004182b4  xor        ecx, ecx
004182b6  mov        eax, ebx
004182b8  call       0x41a900

// block 004182bd
004182bd  imul       eax, eax, 0x214
004182c3  mov        ecx, dword ptr [0x4b0ccc]
004182c9  cmp        ecx, 0x258
004182cf  lea        esi, [eax + ebx]
004182d2  mov        eax, ebx
004182d4  jl         0x4182e7

// block 004182d6
004182d6  mov        ecx, 9
004182db  call       0x41a950

// block 004182e0
004182e0  mov        ecx, 0xa
004182e5  jmp        0x418275

// block 004182e7
004182e7  cmp        ecx, 0xc8
004182ed  jl         0x418303

// block 004182ef
004182ef  mov        ecx, 7
004182f4  call       0x41a950

// block 004182f9
004182f9  mov        ecx, 8
004182fe  jmp        0x418275

// block 00418303
00418303  cmp        ecx, 0xffffff38
00418309  jl         0x41831f

// block 0041830b
0041830b  mov        ecx, 5
00418310  call       0x41a950

// block 00418315
00418315  mov        ecx, 6
0041831a  jmp        0x418275

// block 0041831f
0041831f  cmp        ecx, 0xfffffda8
00418325  jmp        0x418264

// block 0041832a
0041832a  xor        ecx, ecx
0041832c  mov        eax, ebx
0041832e  call       0x41a900

// block 00418333
00418333  mov        esi, eax
00418335  mov        ecx, 1
0041833a  mov        eax, ebx
0041833c  call       0x41a950

// block 00418341
00418341  fstp       dword ptr [esp + 0x40]
00418345  mov        ecx, 2
0041834a  mov        eax, ebx
0041834c  call       0x41a950

// block 00418351
00418351  fstp       dword ptr [esp + 0x4c]
00418355  mov        ecx, 3
0041835a  mov        eax, ebx
0041835c  call       0x41a950

// block 00418361
00418361  fstp       dword ptr [esp + 0x1c]
00418365  mov        ecx, 4
0041836a  mov        eax, ebx
0041836c  call       0x41a950

// block 00418371
00418371  fstp       dword ptr [esp + 0x54]
00418375  imul       esi, esi, 0x214
0041837b  fld        dword ptr [esp + 0x1c]
0041837f  fld        dword ptr [esp + 0x40]
00418383  fld        st(0)
00418385  fsubp      st(2)
00418387  fild       dword ptr [0x4b0ccc]
0041838d  fld        qword ptr [0x4a4420]
00418393  fadd       st(1), st(0)
00418395  lea        eax, [esi + ebx]
00418398  fxch       st(3)
0041839a  fmulp      st(1)
0041839c  fld        qword ptr [0x4a4300]
004183a2  fmul       st(1), st(0)
004183a4  fxch       st(1)
004183a6  faddp      st(2)
004183a8  fxch       st(1)
004183aa  fstp       dword ptr [eax + 0x4a4]
004183b0  fld        dword ptr [esp + 0x54]
004183b4  fld        dword ptr [esp + 0x4c]
004183b8  fld        st(0)
004183ba  fsubp      st(2)
004183bc  fild       dword ptr [0x4b0ccc]
004183c2  faddp      st(4)
004183c4  fxch       st(1)
004183c6  fmulp      st(3)
004183c8  fxch       st(2)
004183ca  fmulp      st(1)
004183cc  faddp      st(1)
004183ce  fstp       dword ptr [eax + 0x4a8]
004183d4  jmp        0x41962b

// block 004183d9
004183d9  xor        ecx, ecx
004183db  mov        eax, ebx
004183dd  call       0x41a900

// block 004183e2
004183e2  mov        ecx, dword ptr [0x4b0ccc]
004183e8  cmp        ecx, 0x200
004183ee  jl         0x41840f

// block 004183f0
004183f0  imul       eax, eax, 0x214
004183f6  lea        esi, [eax + ebx]
004183f9  mov        ecx, 5
004183fe  mov        eax, ebx
00418400  call       0x41a900

// block 00418405
00418405  mov        ecx, 6
0041840a  jmp        0x4180d4

// block 0041840f
0041840f  imul       eax, eax, 0x214
00418415  cmp        ecx, 0xfffffe00

// block 0041841b
0041841b  lea        esi, [eax + ebx]
0041841e  mov        eax, ebx
00418420  jl         0x4180c5

// block 00418426
00418426  mov        ecx, 3
0041842b  call       0x41a900

// block 00418430
00418430  mov        ecx, 4
00418435  jmp        0x4180d4

// block 0041843a
0041843a  xor        ecx, ecx
0041843c  mov        eax, ebx
0041843e  call       0x41a900

// block 00418443
00418443  mov        ecx, dword ptr [0x4b0ccc]
00418449  cmp        ecx, 0x258
0041844f  jl         0x418470

// block 00418451
00418451  imul       eax, eax, 0x214
00418457  lea        esi, [eax + ebx]
0041845a  mov        ecx, 9
0041845f  mov        eax, ebx
00418461  call       0x41a900

// block 00418466
00418466  mov        ecx, 0xa
0041846b  jmp        0x4180d4

// block 00418470
00418470  cmp        ecx, 0xc8
00418476  jl         0x418497

// block 00418478
00418478  imul       eax, eax, 0x214
0041847e  lea        esi, [eax + ebx]
00418481  mov        ecx, 7
00418486  mov        eax, ebx
00418488  call       0x41a900

// block 0041848d
0041848d  mov        ecx, 8
00418492  jmp        0x4180d4

// block 00418497
00418497  cmp        ecx, 0xffffff38
0041849d  jge        0x4183f0

// block 004184a3
004184a3  imul       eax, eax, 0x214
004184a9  cmp        ecx, 0xfffffda8
004184af  jmp        0x41841b

// block 004184b4
004184b4  xor        ecx, ecx
004184b6  mov        eax, ebx
004184b8  call       0x41a900

// block 004184bd
004184bd  mov        esi, eax
004184bf  mov        ecx, 1
004184c4  mov        eax, ebx
004184c6  call       0x41a900

// block 004184cb
004184cb  mov        edi, eax
004184cd  mov        ecx, 2
004184d2  mov        eax, ebx
004184d4  call       0x41a900

// block 004184d9
004184d9  mov        dword ptr [esp + 0x40], eax
004184dd  mov        ecx, 3
004184e2  mov        eax, ebx
004184e4  call       0x41a900

// block 004184e9
004184e9  mov        dword ptr [esp + 0x1c], eax
004184ed  mov        ecx, 4
004184f2  mov        eax, ebx
004184f4  call       0x41a900

// block 004184f9
004184f9  imul       esi, esi, 0x214
004184ff  mov        edx, dword ptr [0x4b0ccc]
00418505  mov        ecx, eax
00418507  mov        eax, dword ptr [esp + 0x1c]
0041850b  sub        eax, edi
0041850d  add        edx, 0x400
00418513  imul       eax, edx
00418516  cdq        
00418517  and        edx, 0x7ff
0041851d  add        eax, edx
0041851f  sar        eax, 0xb
00418522  add        eax, edi
00418524  mov        word ptr [esi + ebx + 0x684], ax
0041852c  mov        edx, dword ptr [0x4b0ccc]
00418532  add        esi, ebx
00418534  mov        eax, ecx
00418536  mov        ecx, dword ptr [esp + 0x40]
0041853a  sub        eax, ecx
0041853c  add        edx, 0x400
00418542  imul       eax, edx
00418545  cdq        
00418546  and        edx, 0x7ff
0041854c  add        eax, edx
0041854e  sar        eax, 0xb
00418551  add        eax, ecx
00418553  mov        word ptr [esi + 0x686], ax
0041855a  jmp        0x41962b

// block 0041855f
0041855f  xor        ecx, ecx
00418561  mov        eax, ebx
00418563  call       0x41a900

// block 00418568
00418568  mov        esi, eax
0041856a  mov        ecx, 1
0041856f  mov        eax, ebx
00418571  call       0x41a900

// block 00418576
00418576  imul       esi, esi, 0x214
0041857c  mov        word ptr [esi + ebx + 0x688], ax
00418584  jmp        0x41962b

// block 00418589
00418589  xor        ecx, ecx
0041858b  mov        eax, ebx
0041858d  call       0x41a900

// block 00418592
00418592  imul       eax, eax, 0x214
00418598  lea        esi, [eax + ebx]
0041859b  mov        ecx, 1
004185a0  mov        eax, ebx
004185a2  call       0x41a900

// block 004185a7
004185a7  mov        dword ptr [esi + 0x690], eax
004185ad  mov        ecx, 2
004185b2  mov        eax, ebx
004185b4  call       0x41a900

// block 004185b9
004185b9  mov        dword ptr [esi + 0x694], eax
004185bf  jmp        0x41962b

// block 004185c4
004185c4  xor        ecx, ecx
004185c6  mov        eax, ebx
004185c8  call       0x41a900

// block 004185cd
004185cd  mov        esi, eax
004185cf  mov        ecx, 1
004185d4  mov        eax, ebx
004185d6  call       0x41a900

// block 004185db
004185db  mov        edi, eax
004185dd  mov        eax, esi
004185df  imul       eax, eax, 0x214
004185e5  lea        ecx, [edi + edi*2]
004185e8  lea        edx, [ebx + ecx*8]
004185eb  mov        dword ptr [esp + 0x1c], eax
004185ef  lea        esi, [edx + eax]
004185f2  mov        ecx, 2
004185f7  mov        eax, ebx
004185f9  call       0x41a900

// block 004185fe
004185fe  mov        dword ptr [esi + 0x4c4], eax
00418604  mov        ecx, 3
00418609  mov        eax, ebx
0041860b  call       0x41a900

// block 00418610
00418610  mov        dword ptr [esi + 0x4c0], eax
00418616  mov        ecx, 4
0041861b  mov        eax, ebx
0041861d  call       0x41a900

// block 00418622
00418622  mov        dword ptr [esi + 0x4b8], eax
00418628  mov        ecx, 5
0041862d  mov        eax, ebx
0041862f  call       0x41a900

// block 00418634
00418634  mov        dword ptr [esi + 0x4bc], eax
0041863a  mov        ecx, 6
0041863f  mov        eax, ebx
00418641  call       0x41a950

// block 00418646
00418646  mov        edx, dword ptr [esp + 0x1c]
0041864a  lea        eax, [edi + edi*2 + 0x96]
00418651  lea        ecx, [ebx + eax*8]
00418654  fstp       dword ptr [ecx + edx]
00418657  mov        ecx, 7
0041865c  mov        eax, ebx
0041865e  call       0x41a950

// block 00418663
00418663  fstp       dword ptr [esi + 0x4b4]
00418669  jmp        0x41962b

// block 0041866e
0041866e  xor        ecx, ecx
00418670  mov        eax, ebx
00418672  call       0x41a900

// block 00418677
00418677  imul       eax, eax, 0x214
0041867d  lea        esi, [eax + ebx]
00418680  mov        ecx, 1
00418685  mov        eax, ebx
00418687  call       0x41a950

// block 0041868c
0041868c  mov        ecx, 2
00418691  mov        eax, ebx
00418693  fstp       dword ptr [esi + 0x490]
00418699  call       0x41a950

// block 0041869e
0041869e  mov        ecx, 3
004186a3  fstp       dword ptr [esi + 0x494]
004186a9  mov        eax, ebx
004186ab  call       0x41a950

// block 004186b0
004186b0  fstp       dword ptr [esi + 0x498]
004186b6  mov        ecx, 4
004186bb  mov        eax, ebx
004186bd  call       0x41a950

// block 004186c2
004186c2  fstp       dword ptr [esi + 0x660]
004186c8  jmp        0x41962b

// block 004186cd
004186cd  xor        ecx, ecx
004186cf  mov        eax, ebx
004186d1  call       0x41a900

// block 004186d6
004186d6  imul       eax, eax, 0x214
004186dc  lea        esi, [eax + ebx]
004186df  mov        ecx, 1
004186e4  mov        eax, ebx
004186e6  call       0x41a900

// block 004186eb
004186eb  mov        dword ptr [esi + 0x670], eax
004186f1  mov        ecx, 2
004186f6  mov        eax, ebx
004186f8  call       0x41a900

// block 004186fd
004186fd  mov        dword ptr [esi + 0x674], eax
00418703  mov        ecx, 3
00418708  mov        eax, ebx
0041870a  call       0x41a900

// block 0041870f
0041870f  mov        dword ptr [esi + 0x678], eax
00418715  mov        ecx, 4
0041871a  mov        eax, ebx
0041871c  call       0x41a900

// block 00418721
00418721  mov        dword ptr [esi + 0x67c], eax
00418727  mov        ecx, 5
0041872c  mov        eax, ebx
0041872e  call       0x41a900

// block 00418733
00418733  mov        dword ptr [esi + 0x680], eax
00418739  jmp        0x41962b

// block 0041873e
0041873e  mov        ecx, 2
00418743  mov        eax, ebx
00418745  call       0x41a900

// block 0041874a
0041874a  mov        edi, eax
0041874c  mov        ecx, 1
00418751  mov        eax, ebx
00418753  call       0x41a900

// block 00418758
00418758  mov        esi, eax
0041875a  xor        ecx, ecx
0041875c  mov        eax, ebx
0041875e  call       0x41a900

// block 00418763
00418763  mov        edx, dword ptr [ebx + 0x174c]
00418769  mov        ecx, eax
0041876b  shl        ecx, 4
0041876e  mov        dword ptr [ecx + edx + 0x270c], esi
00418775  test       esi, esi
00418777  jl         0x41962b

// block 0041877d
0041877d  mov        ecx, dword ptr [esp + 0x18]
00418781  add        ecx, 0x20
00418784  call       0x41a680

// block 00418789
00418789  jmp        0x41962b

// block 0041878e
0041878e  xor        ecx, ecx
00418790  mov        eax, ebx
00418792  call       0x41a900

// block 00418797
00418797  mov        edx, dword ptr [ebx + 0x174c]
0041879d  add        edi, 0x18
004187a0  shl        eax, 4
004187a3  mov        dword ptr [eax + edx + 0x2718], edi
004187aa  jmp        0x41962b

// block 004187af
004187af  push       1
004187b1  call       0x40cf60

// block 004187b6
004187b6  xor        edi, edi
004187b8  lea        ebx, [edi + 1]
004187bb  call       0x428750

// block 004187c0
004187c0  jmp        0x41962b

// block 004187c5
004187c5  fld        dword ptr [ebx + 0x34]
004187c8  push       ecx
004187c9  xor        ecx, ecx
004187cb  fstp       dword ptr [esp]
004187ce  mov        eax, ebx
004187d0  call       0x41a900

// block 004187d5
004187d5  call       0x453e20

// block 004187da
004187da  jmp        0x41962b

// block 004187df
004187df  push       0x46
004187e1  push       0
004187e3  mov        ecx, 2
004187e8  mov        eax, ebx
004187ea  call       0x41a900

// block 004187ef
004187ef  push       eax
004187f0  mov        ecx, 1
004187f5  mov        eax, ebx
004187f7  call       0x41a900

// block 004187fc
004187fc  push       eax
004187fd  xor        ecx, ecx
004187ff  mov        eax, ebx
00418801  call       0x41a900

// block 00418806
00418806  push       eax
00418807  push       1
00418809  call       0x4529a0

// block 0041880e
0041880e  jmp        0x41962b

// block 00418813
00418813  xor        ecx, ecx
00418815  mov        eax, ebx
00418817  call       0x41a900

// block 0041881c
0041881c  mov        ebx, eax
0041881e  call       0x41fbe0

// block 00418823
00418823  push       0
00418825  call       0x40cf60

// block 0041882a
0041882a  xor        edi, edi
0041882c  xor        ebx, ebx
0041882e  call       0x428750

// block 00418833
00418833  call       0x414c60

// block 00418838
00418838  jmp        0x41962b

// block 0041883d
0041883d  mov        eax, dword ptr [0x4b43e4]
00418842  cmp        dword ptr [eax + 0x6d30], 0
00418849  je         0x41962b

// block 0041884f
0041884f  call       0x412720

// block 00418854
00418854  test       eax, eax
00418856  jne        0x41962b

// block 0041885c
0041885c  or         eax, 0xffffffff
0041885f  jmp        0x41962d

// block 00418864
00418864  mov        ecx, dword ptr [0x4b43dc]
0041886a  cmp        dword ptr [ecx + 0x1c], 0
0041886e  je         0x41962b

// block 00418874
00418874  or         eax, 0xffffffff
00418877  jmp        0x41962d

// block 0041887c
0041887c  mov        eax, dword ptr [esp + 0x18]
00418880  mov        eax, dword ptr [eax + 0x1c]
00418883  mov        edx, edi
00418885  add        edx, 0x20
00418888  xor        edi, edi
0041888a  mov        cl, 0x77
0041888c  mov        byte ptr [esp + 0x4b], 7
00418891  mov        dword ptr [esp + 0x40], eax
00418895  test       eax, eax
00418897  jle        0x4188d5

// block 00418899
00418899  lea        eax, [esp + 0x28c]
004188a0  sub        edx, eax
004188a2  mov        dword ptr [esp + 0x1c], edx
004188a6  jmp        0x4188b4

// block 004188b0
004188b0  mov        edx, dword ptr [esp + 0x1c]

// block 004188b4
004188b4  lea        eax, [esp + edi + 0x28c]
004188bb  mov        dl, byte ptr [edx + eax]
004188be  xor        dl, cl
004188c0  mov        byte ptr [eax], dl
004188c2  mov        al, byte ptr [esp + 0x4b]
004188c6  add        cl, al
004188c8  add        al, 0x10
004188ca  inc        edi
004188cb  cmp        edi, dword ptr [esp + 0x40]
004188cf  mov        byte ptr [esp + 0x4b], al
004188d3  jl         0x4188b0

// block 004188d5
004188d5  mov        edx, dword ptr [esi + 4]
004188d8  push       0
004188da  call       0x468e20

// block 004188df
004188df  mov        esi, eax
004188e1  mov        eax, dword ptr [esp + 0x18]
004188e5  movsx      eax, word ptr [eax + 4]
004188e9  sub        eax, 0x1b5
004188ee  je         0x418912

// block 004188f0
004188f0  sub        eax, 1
004188f3  je         0x418906

// block 004188f5
004188f5  sub        eax, 1
004188f8  jne        0x418918

// block 004188fa
004188fa  mov        ecx, dword ptr [0x4b0ca8]
00418900  lea        esi, [esi + ecx - 2]
00418904  jmp        0x418918

// block 00418906
00418906  mov        edx, dword ptr [0x4b0ca8]
0041890c  lea        esi, [esi + edx - 1]
00418910  jmp        0x418918

// block 00418912
00418912  add        esi, dword ptr [0x4b0ca8]

// block 00418918
00418918  mov        eax, dword ptr [ebx + 0x174c]
0041891e  mov        edx, dword ptr [eax + 4]
00418921  push       2
00418923  call       0x468e20

// block 00418928
00418928  mov        eax, dword ptr [ebx + 0x174c]
0041892e  mov        edx, dword ptr [eax + 4]
00418931  push       1
00418933  call       0x468e20

// block 00418938
00418938  push       eax
00418939  lea        eax, [esp + 0x290]
00418940  mov        ecx, esi
00418942  call       0x40e060

// block 00418947
00418947  or         dword ptr [ebx + 0x161c], 1
0041894e  mov        eax, dword ptr [ebx + 0x1608]
00418954  lea        ecx, [eax*8]
0041895b  sub        ecx, eax
0041895d  mov        dword ptr [ebx + 0x1614], ecx

// block 00418963
00418963  push       0
00418965  lea        eax, [ebx + 0x26c]
0041896b  call       0x4067e0

// block 00418970
00418970  jmp        0x41962b

// block 00418975
00418975  call       0x40e5e0

// block 0041897a
0041897a  and        dword ptr [ebx + 0x161c], 0xfffffffe
00418981  jmp        0x41962b

// block 00418986
00418986  mov        eax, dword ptr [0x4b43cc]
0041898b  or         dword ptr [eax + 0x7c], 8
0041898f  jmp        0x41962b

// block 00418994
00418994  call       0x4127b0

// block 00418999
00418999  jmp        0x41962b

// block 0041899e
0041899e  xor        ecx, ecx
004189a0  mov        eax, ebx
004189a2  call       0x41a900

// block 004189a7
004189a7  shl        eax, 0x1a
004189aa  xor        eax, dword ptr [ebx + 0x16b8]
004189b0  and        eax, 0x4000000
004189b5  xor        dword ptr [ebx + 0x16b8], eax
004189bb  jmp        0x41962b

// block 004189c0
004189c0  push       0x1e8
004189c5  lea        edx, [esp + 0x88]
004189cc  push       0
004189ce  push       edx
004189cf  call       0x477420

// block 004189d4
004189d4  add        esp, 0xc
004189d7  xor        ecx, ecx
004189d9  mov        eax, ebx
004189db  call       0x41a900

// block 004189e0
004189e0  mov        ecx, eax
004189e2  imul       ecx, ecx, 0x214
004189e8  add        ecx, ebx
004189ea  mov        dword ptr [esp + 0x1c], ecx
004189ee  lea        esi, [ecx + 0x4b0]
004189f4  mov        ecx, 0x6c
004189f9  lea        edi, [esp + 0xb4]
00418a00  lea        edx, [eax + eax*2]

// block 00418a03
00418a03  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00418a05
00418a05  fld        dword ptr [ebx + edx*4 + 0x1594]
00418a0c  fcomp      qword ptr [0x4a40e8]
00418a12  fnstsw     ax
00418a14  fld        dword ptr [ebx + edx*4 + 0x152c]
00418a1b  lea        ecx, [ebx + edx*4]
00418a1e  test       ah, 0x41
00418a21  jne        0x418a63

// block 00418a23
00418a23  fadd       dword ptr [ecx + 0x158c]
00418a29  fstp       dword ptr [esp + 0x24]
00418a2d  fld        dword ptr [ecx + 0x1530]
00418a33  fadd       dword ptr [ecx + 0x1590]
00418a39  fstp       dword ptr [esp + 0x28]
00418a3d  fld        dword ptr [ecx + 0x1534]
00418a43  fadd       dword ptr [ecx + 0x1594]
00418a49  fstp       dword ptr [esp + 0x2c]
00418a4d  mov        edx, dword ptr [esp + 0x2c]
00418a51  fldz       
00418a53  mov        dword ptr [esp + 0x8c], edx
00418a5a  fstp       dword ptr [esp + 0x8c]
00418a61  jmp        0x418a8f

// block 00418a63
00418a63  fadd       dword ptr [ebx + 0x34]
00418a66  fstp       dword ptr [esp + 0x24]
00418a6a  fld        dword ptr [ecx + 0x1530]
00418a70  fadd       dword ptr [ebx + 0x38]
00418a73  fstp       dword ptr [esp + 0x28]
00418a77  fld        dword ptr [ecx + 0x1534]
00418a7d  fadd       dword ptr [ebx + 0x3c]
00418a80  fstp       dword ptr [esp + 0x2c]
00418a84  mov        edx, dword ptr [esp + 0x2c]
00418a88  mov        dword ptr [esp + 0x8c], edx

// block 00418a8f
00418a8f  mov        ecx, dword ptr [esp + 0x28]
00418a93  mov        esi, dword ptr [esp + 0x1c]
00418a97  mov        eax, dword ptr [esp + 0x24]
00418a9b  fld        dword ptr [esi + 0x49c]
00418aa1  mov        dword ptr [esp + 0x88], ecx
00418aa8  mov        cx, word ptr [esi + 0x48e]
00418aaf  mov        dword ptr [esp + 0x84], eax
00418ab6  mov        ax, word ptr [esi + 0x48c]
00418abd  mov        word ptr [esp + 0xa8], ax
00418ac5  mov        word ptr [esp + 0xaa], cx
00418acd  push       ecx
00418ace  fstp       dword ptr [esp]
00418ad1  call       0x4646e0

// block 00418ad6
00418ad6  mov        edx, dword ptr [esi + 0x690]
00418adc  fstp       dword ptr [esp + 0x90]
00418ae3  fld        dword ptr [esi + 0x4a4]
00418ae9  mov        eax, dword ptr [esi + 0x694]
00418aef  or         dword ptr [esp + 0xb0], 1
00418af7  fstp       dword ptr [esp + 0xa4]
00418afe  fld        dword ptr [esi + 0x490]
00418b04  push       0
00418b06  fstp       dword ptr [esp + 0x9c]
00418b0d  lea        edi, [esp + 0x88]
00418b14  fld        dword ptr [esi + 0x494]
00418b1a  mov        dword ptr [esp + 0x268], edx
00418b21  fstp       dword ptr [esp + 0x98]
00418b28  mov        dword ptr [esp + 0x26c], eax
00418b2f  fld        dword ptr [esi + 0x498]
00418b35  fstp       dword ptr [esp + 0xa0]
00418b3c  fld        dword ptr [esi + 0x660]
00418b42  fstp       dword ptr [esp + 0xa4]
00418b49  fld        dword ptr [esi + 0x4ac]
00418b4f  fstp       dword ptr [esp + 0xb0]
00418b56  call       0x428450

// block 00418b5b
00418b5b  jmp        0x41962b

// block 00418b60
00418b60  lea        esi, [esp + 0x84]
00418b67  call       0x4128e0

// block 00418b6c
00418b6c  xor        ecx, ecx
00418b6e  mov        eax, ebx
00418b70  call       0x41a900

// block 00418b75
00418b75  mov        ecx, eax
00418b77  imul       ecx, ecx, 0x214
00418b7d  add        ecx, ebx
00418b7f  mov        dword ptr [esp + 0x1c], ecx
00418b83  lea        esi, [ecx + 0x4b0]
00418b89  mov        ecx, 0x6c
00418b8e  lea        edi, [esp + 0xdc]
00418b95  lea        edx, [eax + eax*2]

// block 00418b98
00418b98  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00418b9a
00418b9a  fld        dword ptr [ebx + edx*4 + 0x1594]
00418ba1  fcomp      qword ptr [0x4a40e8]
00418ba7  fnstsw     ax
00418ba9  fld        dword ptr [ebx + edx*4 + 0x152c]
00418bb0  lea        ecx, [ebx + edx*4]
00418bb3  test       ah, 0x41
00418bb6  jne        0x418bf8

// block 00418bb8
00418bb8  fadd       dword ptr [ecx + 0x158c]
00418bbe  fstp       dword ptr [esp + 0x24]
00418bc2  fld        dword ptr [ecx + 0x1530]
00418bc8  fadd       dword ptr [ecx + 0x1590]
00418bce  fstp       dword ptr [esp + 0x28]
00418bd2  fld        dword ptr [ecx + 0x1534]
00418bd8  fadd       dword ptr [ecx + 0x1594]
00418bde  fstp       dword ptr [esp + 0x2c]
00418be2  mov        edx, dword ptr [esp + 0x2c]
00418be6  fldz       
00418be8  mov        dword ptr [esp + 0x8c], edx
00418bef  fstp       dword ptr [esp + 0x8c]
00418bf6  jmp        0x418c24

// block 00418bf8
00418bf8  fadd       dword ptr [ebx + 0x34]
00418bfb  fstp       dword ptr [esp + 0x24]
00418bff  fld        dword ptr [ecx + 0x1530]
00418c05  fadd       dword ptr [ebx + 0x38]
00418c08  fstp       dword ptr [esp + 0x28]
00418c0c  fld        dword ptr [ecx + 0x1534]
00418c12  fadd       dword ptr [ebx + 0x3c]
00418c15  fstp       dword ptr [esp + 0x2c]
00418c19  mov        edx, dword ptr [esp + 0x2c]
00418c1d  mov        dword ptr [esp + 0x8c], edx

// block 00418c24
00418c24  mov        ecx, dword ptr [esp + 0x28]
00418c28  mov        esi, dword ptr [esp + 0x1c]
00418c2c  mov        eax, dword ptr [esp + 0x24]
00418c30  fld        dword ptr [esi + 0x49c]
00418c36  mov        dword ptr [esp + 0x88], ecx
00418c3d  mov        cx, word ptr [esi + 0x48e]
00418c44  mov        dword ptr [esp + 0x84], eax
00418c4b  mov        ax, word ptr [esi + 0x48c]
00418c52  mov        word ptr [esp + 0xd4], ax
00418c5a  mov        word ptr [esp + 0xd6], cx
00418c62  push       ecx
00418c63  fstp       dword ptr [esp]
00418c66  call       0x4646e0

// block 00418c6b
00418c6b  mov        eax, dword ptr [esi + 0x674]
00418c71  fstp       dword ptr [esp + 0x9c]
00418c78  fld        dword ptr [esi + 0x4a4]
00418c7e  mov        edx, dword ptr [esi + 0x670]
00418c84  mov        ecx, dword ptr [esi + 0x678]
00418c8a  fstp       dword ptr [esp + 0xb0]
00418c91  fld        dword ptr [esi + 0x490]
00418c97  mov        dword ptr [esp + 0xb8], eax
00418c9e  mov        eax, dword ptr [esi + 0x680]
00418ca4  fstp       dword ptr [esp + 0xa8]
00418cab  fld        dword ptr [esi + 0x494]
00418cb1  mov        dword ptr [esp + 0xb4], edx
00418cb8  mov        edx, dword ptr [esi + 0x67c]
00418cbe  fstp       dword ptr [esp + 0xa4]
00418cc5  fld        dword ptr [esi + 0x660]
00418ccb  mov        dword ptr [esp + 0xbc], ecx
00418cd2  mov        ecx, dword ptr [esi + 0x690]
00418cd8  fstp       dword ptr [esp + 0xac]
00418cdf  fld        dword ptr [esi + 0x4ac]
00418ce5  or         eax, 2
00418ce8  mov        dword ptr [esp + 0xc0], edx
00418cef  fstp       dword ptr [esp + 0xd0]
00418cf6  mov        edx, dword ptr [esi + 0x694]
00418cfc  mov        dword ptr [esp + 0xd8], eax
00418d03  mov        dword ptr [esp + 0xc4], ecx
00418d0a  mov        ecx, 1
00418d0f  mov        eax, ebx
00418d11  mov        dword ptr [esp + 0xc8], edx
00418d18  call       0x41a900

// block 00418d1d
00418d1d  push       1
00418d1f  lea        edi, [esp + 0x88]
00418d26  mov        dword ptr [esp + 0xd0], eax
00418d2d  call       0x428450

// block 00418d32
00418d32  jmp        0x41962b

// block 00418d37
00418d37  push       0x1e0
00418d3c  lea        eax, [esp + 0x88]
00418d43  push       0
00418d45  push       eax
00418d46  call       0x477420

// block 00418d4b
00418d4b  add        esp, 0xc
00418d4e  xor        ecx, ecx
00418d50  mov        eax, ebx
00418d52  call       0x41a900

// block 00418d57
00418d57  mov        ecx, eax
00418d59  imul       ecx, ecx, 0x214
00418d5f  add        ecx, ebx
00418d61  mov        dword ptr [esp + 0x1c], ecx
00418d65  lea        esi, [ecx + 0x4b0]
00418d6b  mov        ecx, 0x6c
00418d70  lea        edi, [esp + 0xac]
00418d77  lea        edx, [eax + eax*2]

// block 00418d7a
00418d7a  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00418d7c
00418d7c  fld        dword ptr [ebx + edx*4 + 0x1594]
00418d83  fcomp      qword ptr [0x4a40e8]
00418d89  fnstsw     ax
00418d8b  fld        dword ptr [ebx + edx*4 + 0x152c]
00418d92  lea        ecx, [ebx + edx*4]
00418d95  test       ah, 0x41
00418d98  jne        0x418dda

// block 00418d9a
00418d9a  fadd       dword ptr [ecx + 0x158c]
00418da0  fstp       dword ptr [esp + 0x24]
00418da4  fld        dword ptr [ecx + 0x1530]
00418daa  fadd       dword ptr [ecx + 0x1590]
00418db0  fstp       dword ptr [esp + 0x28]
00418db4  fld        dword ptr [ecx + 0x1534]
00418dba  fadd       dword ptr [ecx + 0x1594]
00418dc0  fstp       dword ptr [esp + 0x2c]
00418dc4  mov        edx, dword ptr [esp + 0x2c]
00418dc8  fldz       
00418dca  mov        dword ptr [esp + 0x8c], edx
00418dd1  fstp       dword ptr [esp + 0x8c]
00418dd8  jmp        0x418e06

// block 00418dda
00418dda  fadd       dword ptr [ebx + 0x34]
00418ddd  fstp       dword ptr [esp + 0x24]
00418de1  fld        dword ptr [ecx + 0x1530]
00418de7  fadd       dword ptr [ebx + 0x38]
00418dea  fstp       dword ptr [esp + 0x28]
00418dee  fld        dword ptr [ecx + 0x1534]
00418df4  fadd       dword ptr [ebx + 0x3c]
00418df7  fstp       dword ptr [esp + 0x2c]
00418dfb  mov        edx, dword ptr [esp + 0x2c]
00418dff  mov        dword ptr [esp + 0x8c], edx

// block 00418e06
00418e06  mov        ecx, dword ptr [esp + 0x28]
00418e0a  mov        esi, dword ptr [esp + 0x1c]
00418e0e  mov        eax, dword ptr [esp + 0x24]
00418e12  fld        dword ptr [esi + 0x49c]
00418e18  mov        dword ptr [esp + 0x88], ecx
00418e1f  mov        cx, word ptr [esi + 0x48e]
00418e26  mov        dword ptr [esp + 0x84], eax
00418e2d  mov        ax, word ptr [esi + 0x48c]
00418e34  mov        word ptr [esp + 0x9c], ax
00418e3c  mov        word ptr [esp + 0x9e], cx
00418e44  push       ecx
00418e45  fstp       dword ptr [esp]
00418e48  call       0x4646e0

// block 00418e4d
00418e4d  mov        edx, dword ptr [esi + 0x670]
00418e53  fstp       dword ptr [esp + 0x90]
00418e5a  fld        dword ptr [esi + 0x4a4]
00418e60  mov        eax, dword ptr [esi + 0x690]
00418e66  mov        ecx, dword ptr [esi + 0x694]
00418e6c  fstp       dword ptr [esp + 0x98]
00418e73  fld        dword ptr [esi + 0x660]
00418e79  or         dword ptr [esp + 0xa8], 1
00418e81  fstp       dword ptr [esp + 0x94]
00418e88  push       2
00418e8a  fld        dword ptr [esi + 0x4ac]
00418e90  lea        edi, [esp + 0x88]
00418e97  fstp       dword ptr [esp + 0xa8]
00418e9e  mov        dword ptr [esp + 0xa4], edx
00418ea5  mov        dword ptr [esp + 0x260], eax
00418eac  mov        dword ptr [esp + 0x264], ecx
00418eb3  call       0x428450

// block 00418eb8
00418eb8  jmp        0x41962b

// block 00418ebd
00418ebd  xor        ecx, ecx
00418ebf  mov        eax, ebx
00418ec1  call       0x41a900

// block 00418ec6
00418ec6  mov        ecx, eax
00418ec8  call       0x412960

// block 00418ecd
00418ecd  mov        esi, eax
00418ecf  xor        edi, edi
00418ed1  cmp        esi, edi
00418ed3  je         0x41962b

// block 00418ed9
00418ed9  lea        esp, [esp]

// block 00418ee0
00418ee0  mov        edx, dword ptr [esi]
00418ee2  mov        eax, dword ptr [edx + 0x14]
00418ee5  push       edi
00418ee6  push       edi
00418ee7  mov        ecx, esi
00418ee9  call       eax

// block 00418eeb
00418eeb  xor        ecx, ecx
00418eed  mov        eax, ebx
00418eef  mov        dword ptr [esi + 0x80], edi
00418ef5  call       0x41a900

// block 00418efa
00418efa  mov        ecx, eax
00418efc  call       0x412960

// block 00418f01
00418f01  mov        esi, eax
00418f03  cmp        esi, edi
00418f05  jne        0x418ee0

// block 00418f07
00418f07  jmp        0x41962b

// block 00418f0c
00418f0c  xor        ecx, ecx
00418f0e  mov        eax, ebx
00418f10  call       0x41a900

// block 00418f15
00418f15  mov        ecx, eax
00418f17  call       0x412960

// block 00418f1c
00418f1c  mov        esi, eax
00418f1e  test       esi, esi
00418f20  je         0x41962b

// block 00418f26
00418f26  mov        ecx, 1
00418f2b  mov        eax, ebx
00418f2d  call       0x41a950

// block 00418f32
00418f32  fstp       dword ptr [esp + 0x24]
00418f36  mov        ecx, 2
00418f3b  mov        eax, ebx
00418f3d  call       0x41a950

// block 00418f42
00418f42  fstp       dword ptr [esp + 0x28]
00418f46  fldz       
00418f48  mov        ecx, dword ptr [esp + 0x24]
00418f4c  mov        edx, dword ptr [esp + 0x28]
00418f50  fstp       dword ptr [esp + 0x2c]
00418f54  mov        eax, dword ptr [esp + 0x2c]
00418f58  mov        dword ptr [esi + 0x50], ecx
00418f5b  mov        dword ptr [esi + 0x54], edx
00418f5e  mov        dword ptr [esi + 0x58], eax
00418f61  jmp        0x41962b

// block 00418f66
00418f66  xor        ecx, ecx
00418f68  mov        eax, ebx
00418f6a  call       0x41a900

// block 00418f6f
00418f6f  mov        ecx, eax
00418f71  call       0x412960

// block 00418f76
00418f76  mov        esi, eax
00418f78  test       esi, esi
00418f7a  je         0x41962b

// block 00418f80
00418f80  mov        ecx, 1
00418f85  mov        eax, ebx
00418f87  call       0x41a950

// block 00418f8c
00418f8c  fstp       dword ptr [esp + 0x24]
00418f90  mov        ecx, 2
00418f95  mov        eax, ebx
00418f97  call       0x41a950

// block 00418f9c
00418f9c  fstp       dword ptr [esp + 0x28]
00418fa0  fldz       
00418fa2  lea        eax, [esp + 0x24]
00418fa6  mov        ecx, esi
00418fa8  fstp       dword ptr [esp + 0x2c]
00418fac  call       0x412900

// block 00418fb1
00418fb1  jmp        0x41962b

// block 00418fb6
00418fb6  xor        ecx, ecx
00418fb8  mov        eax, ebx
00418fba  call       0x41a900

// block 00418fbf
00418fbf  mov        ecx, eax
00418fc1  call       0x412960

// block 00418fc6
00418fc6  mov        esi, eax
00418fc8  test       esi, esi
00418fca  je         0x41962b

// block 00418fd0
00418fd0  mov        ecx, 1
00418fd5  mov        eax, ebx
00418fd7  call       0x41a950

// block 00418fdc
00418fdc  fstp       dword ptr [esi + 0x74]
00418fdf  jmp        0x41962b

// block 00418fe4
00418fe4  xor        ecx, ecx
00418fe6  mov        eax, ebx
00418fe8  call       0x41a900

// block 00418fed
00418fed  mov        ecx, eax
00418fef  call       0x412960

// block 00418ff4
00418ff4  mov        esi, eax
00418ff6  test       esi, esi
00418ff8  je         0x41962b

// block 00418ffe
00418ffe  mov        ecx, 1
00419003  mov        eax, ebx
00419005  call       0x41a950

// block 0041900a
0041900a  fstp       dword ptr [esi + 0x70]
0041900d  jmp        0x41962b

// block 00419012
00419012  xor        ecx, ecx
00419014  mov        eax, ebx
00419016  call       0x41a900

// block 0041901b
0041901b  mov        ecx, eax
0041901d  call       0x412960

// block 00419022
00419022  mov        esi, eax
00419024  test       esi, esi
00419026  je         0x41962b

// block 0041902c
0041902c  mov        ecx, 1
00419031  mov        eax, ebx
00419033  call       0x41a950

// block 00419038
00419038  fstp       dword ptr [esi + 0x68]
0041903b  jmp        0x41962b

// block 00419040
00419040  xor        ecx, ecx
00419042  mov        eax, ebx
00419044  call       0x41a900

// block 00419049
00419049  mov        ecx, eax
0041904b  call       0x412960

// block 00419050
00419050  mov        esi, eax
00419052  test       esi, esi
00419054  je         0x41962b

// block 0041905a
0041905a  mov        ecx, 1
0041905f  mov        eax, ebx
00419061  call       0x41a950

// block 00419066
00419066  fstp       dword ptr [esi + 0x470]
0041906c  jmp        0x41962b

// block 00419071
00419071  xor        ecx, ecx
00419073  mov        eax, ebx
00419075  call       0x41a950

// block 0041907a
0041907a  fstp       dword ptr [esp + 0x10]
0041907e  fld        dword ptr [esp + 0x10]
00419082  push       0
00419084  push       1
00419086  lea        esi, [ebx + 0x34]
00419089  push       ecx
0041908a  mov        ebx, esi
0041908c  fstp       dword ptr [esp]
0041908f  call       0x40caa0

// block 00419094
00419094  fld        dword ptr [esp + 0x10]
00419098  push       1
0041909a  push       1
0041909c  push       ecx
0041909d  fstp       dword ptr [esp]
004190a0  call       0x4286f0

// block 004190a5
004190a5  jmp        0x41962b

// block 004190aa
004190aa  xor        ecx, ecx
004190ac  mov        eax, ebx
004190ae  call       0x41a950

// block 004190b3
004190b3  fstp       dword ptr [esp + 0x10]
004190b7  fld        dword ptr [esp + 0x10]
004190bb  push       0
004190bd  push       0
004190bf  lea        esi, [ebx + 0x34]
004190c2  push       ecx
004190c3  mov        ebx, esi
004190c5  fstp       dword ptr [esp]
004190c8  call       0x40caa0

// block 004190cd
004190cd  fld        dword ptr [esp + 0x10]
004190d1  push       1
004190d3  push       0
004190d5  push       ecx
004190d6  fstp       dword ptr [esp]
004190d9  call       0x4286f0

// block 004190de
004190de  jmp        0x41962b

// block 004190e3
004190e3  xor        ecx, ecx
004190e5  mov        eax, ebx
004190e7  call       0x41a950

// block 004190ec
004190ec  fstp       dword ptr [esp + 0x10]
004190f0  fld        dword ptr [esp + 0x10]
004190f4  push       1
004190f6  push       1
004190f8  lea        esi, [ebx + 0x34]
004190fb  push       ecx
004190fc  mov        ebx, esi
004190fe  fstp       dword ptr [esp]
00419101  call       0x40caa0

// block 00419106
00419106  fld        dword ptr [esp + 0x10]
0041910a  push       1
0041910c  push       1
0041910e  push       ecx
0041910f  fstp       dword ptr [esp]
00419112  call       0x4286f0

// block 00419117
00419117  jmp        0x41962b

// block 0041911c
0041911c  xor        ecx, ecx
0041911e  mov        eax, ebx
00419120  call       0x41a950

// block 00419125
00419125  fstp       dword ptr [esp + 0x10]
00419129  fld        dword ptr [esp + 0x10]
0041912d  push       1
0041912f  push       0
00419131  lea        esi, [ebx + 0x34]
00419134  push       ecx
00419135  mov        ebx, esi
00419137  fstp       dword ptr [esp]
0041913a  call       0x40caa0

// block 0041913f
0041913f  fld        dword ptr [esp + 0x10]
00419143  push       1
00419145  push       0
00419147  push       ecx
00419148  fstp       dword ptr [esp]
0041914b  call       0x4286f0

// block 00419150
00419150  jmp        0x41962b

// block 00419155
00419155  xor        ecx, ecx
00419157  mov        eax, ebx
00419159  call       0x41a900

// block 0041915e
0041915e  mov        ecx, eax
00419160  mov        eax, 0x4b0c40
00419165  call       0x41c550

// block 0041916a
0041916a  jmp        0x41962b

// block 0041916f
0041916f  mov        eax, dword ptr [0x4b0ccc]
00419174  cmp        eax, 0x200
00419179  jl         0x419199

// block 0041917b
0041917b  xor        ecx, ecx
0041917d  mov        eax, ebx
0041917f  call       0x41a960

// block 00419184
00419184  mov        esi, eax
00419186  mov        ecx, 2
0041918b  mov        eax, ebx
0041918d  call       0x41a950

// block 00419192
00419192  fstp       dword ptr [esi]
00419194  jmp        0x41962b

// block 00419199
00419199  xor        ecx, ecx
0041919b  cmp        eax, 0xfffffe00
004191a0  mov        eax, ebx

// block 004191a2
004191a2  call       0x41a960

// block 004191a7
004191a7  mov        esi, eax
004191a9  xor        ecx, ecx
004191ab  mov        eax, ebx
004191ad  call       0x41a950

// block 004191b2
004191b2  fstp       dword ptr [esi]
004191b4  jmp        0x41962b

// block 004191b9
004191b9  mov        eax, dword ptr [0x4b0ccc]
004191be  cmp        eax, 0x258
004191c3  jl         0x4191e3

// block 004191c5
004191c5  xor        ecx, ecx
004191c7  mov        eax, ebx
004191c9  call       0x41a960

// block 004191ce
004191ce  mov        esi, eax
004191d0  mov        ecx, 4
004191d5  mov        eax, ebx
004191d7  call       0x41a950

// block 004191dc
004191dc  fstp       dword ptr [esi]
004191de  jmp        0x41962b

// block 004191e3
004191e3  cmp        eax, 0xc8
004191e8  jl         0x419208

// block 004191ea
004191ea  xor        ecx, ecx
004191ec  mov        eax, ebx
004191ee  call       0x41a960

// block 004191f3
004191f3  mov        esi, eax
004191f5  mov        ecx, 3
004191fa  mov        eax, ebx
004191fc  call       0x41a950

// block 00419201
00419201  fstp       dword ptr [esi]
00419203  jmp        0x41962b

// block 00419208
00419208  cmp        eax, 0xffffff38
0041920d  jge        0x41917b

// block 00419213
00419213  xor        ecx, ecx
00419215  cmp        eax, 0xfffffe70
0041921a  mov        eax, ebx
0041921c  jl         0x4191a2

// block 0041921e
0041921e  call       0x41a960

// block 00419223
00419223  mov        esi, eax
00419225  mov        ecx, 1
0041922a  mov        eax, ebx
0041922c  call       0x41a950

// block 00419231
00419231  fstp       dword ptr [esi]
00419233  jmp        0x41962b

// block 00419238
00419238  mov        ecx, 1
0041923d  mov        eax, ebx
0041923f  call       0x41a950

// block 00419244
00419244  fstp       dword ptr [esp + 0x40]
00419248  mov        ecx, 2
0041924d  mov        eax, ebx
0041924f  call       0x41a950

// block 00419254
00419254  fstp       dword ptr [esp + 0x1c]
00419258  fld        dword ptr [esp + 0x1c]
0041925c  xor        ecx, ecx
0041925e  fld        dword ptr [esp + 0x40]
00419262  mov        eax, ebx
00419264  fld        st(0)
00419266  fsubp      st(2)
00419268  fild       dword ptr [0x4b0ccc]
0041926e  fadd       qword ptr [0x4a4420]
00419274  fmulp      st(2)
00419276  fxch       st(1)
00419278  fmul       qword ptr [0x4a4300]
0041927e  faddp      st(1)
00419280  fstp       dword ptr [esp + 0x1c]
00419284  call       0x41a960

// block 00419289
00419289  fld        dword ptr [esp + 0x1c]
0041928d  jmp        0x419629

// block 00419292
00419292  mov        eax, dword ptr [0x4b0ccc]
00419297  cmp        eax, 0x200
0041929c  jge        0x417bc7

// block 004192a2
004192a2  cmp        eax, 0xfffffe00
004192a7  mov        eax, ebx

// block 004192a9
004192a9  call       0x41a910

// block 004192ae
004192ae  mov        esi, eax
004192b0  xor        ecx, ecx
004192b2  mov        eax, ebx
004192b4  call       0x41a900

// block 004192b9
004192b9  mov        dword ptr [esi], eax
004192bb  jmp        0x41962b

// block 004192c0
004192c0  mov        eax, dword ptr [0x4b0ccc]
004192c5  cmp        eax, 0x258
004192ca  jge        0x417bff

// block 004192d0
004192d0  cmp        eax, 0xc8
004192d5  jl         0x4192f3

// block 004192d7
004192d7  mov        eax, ebx
004192d9  call       0x41a910

// block 004192de
004192de  mov        esi, eax
004192e0  mov        ecx, 3
004192e5  mov        eax, ebx
004192e7  call       0x41a900

// block 004192ec
004192ec  mov        dword ptr [esi], eax
004192ee  jmp        0x41962b

// block 004192f3
004192f3  cmp        eax, 0xffffff38
004192f8  jl         0x419316

// block 004192fa
004192fa  mov        eax, ebx
004192fc  call       0x41a910

// block 00419301
00419301  mov        esi, eax
00419303  mov        ecx, 2
00419308  mov        eax, ebx
0041930a  call       0x41a900

// block 0041930f
0041930f  mov        dword ptr [esi], eax
00419311  jmp        0x41962b

// block 00419316
00419316  cmp        eax, 0xfffffe70
0041931b  mov        eax, ebx
0041931d  jl         0x4192a9

// block 0041931f
0041931f  jmp        0x417bad

// block 00419324
00419324  mov        ecx, 1
00419329  mov        eax, ebx
0041932b  call       0x41a900

// block 00419330
00419330  mov        edi, eax
00419332  mov        ecx, 2
00419337  mov        eax, ebx
00419339  call       0x41a900

// block 0041933e
0041933e  mov        ecx, dword ptr [0x4b0ccc]
00419344  sub        eax, edi
00419346  add        ecx, 0x400
0041934c  imul       eax, ecx
0041934f  cdq        
00419350  and        edx, 0x7ff
00419356  add        eax, edx
00419358  mov        esi, eax
0041935a  sar        esi, 0xb
0041935d  mov        eax, ebx
0041935f  add        esi, edi
00419361  call       0x41a910

// block 00419366
00419366  mov        dword ptr [eax], esi
00419368  jmp        0x41962b

// block 0041936d
0041936d  xor        ecx, ecx
0041936f  mov        eax, ebx
00419371  call       0x41a900

// block 00419376
00419376  mov        edx, dword ptr [0x4b43e4]
0041937c  mov        dword ptr [edx + 0x6cf4], eax
00419382  jmp        0x41962b

// block 00419387
00419387  call       0x428670

// block 0041938c
0041938c  jmp        0x41962b

// block 00419391
00419391  xor        ecx, ecx
00419393  mov        eax, ebx
00419395  call       0x41a900

// block 0041939a
0041939a  shl        eax, 0x1b
0041939d  xor        eax, dword ptr [ebx + 0x16b8]
004193a3  mov        ecx, 1
004193a8  and        eax, 0x8000000
004193ad  xor        dword ptr [ebx + 0x16b8], eax
004193b3  mov        eax, ebx
004193b5  call       0x41a900

// block 004193ba
004193ba  and        dword ptr [ebx + 0x16b8], 0xefffffff
004193c4  mov        dword ptr [ebx + 0x16bc], eax
004193ca  mov        eax, dword ptr [ebx + 0x22c]
004193d0  mov        dword ptr [ebx + 0x16c0], eax
004193d6  jmp        0x41962b

// block 004193db
004193db  xor        ecx, ecx
004193dd  mov        eax, ebx
004193df  call       0x41a950

// block 004193e4
004193e4  fstp       dword ptr [0x4b2ed0]
004193ea  jmp        0x41962b

// block 004193ef
004193ef  mov        eax, dword ptr [0x4b4514]
004193f4  fld        dword ptr [eax + 0x97c]
004193fa  mov        ecx, 1
004193ff  fstp       qword ptr [esp + 0x24]
00419403  fld        dword ptr [eax + 0x980]
00419409  mov        eax, ebx
0041940b  fstp       qword ptr [esp + 0x5c]
0041940f  call       0x41a950

// block 00419414
00419414  fsubr      qword ptr [esp + 0x24]
00419418  push       ecx
00419419  mov        ecx, 2
0041941e  mov        eax, ebx
00419420  fstp       dword ptr [esp + 0x20]
00419424  fld        dword ptr [esp + 0x20]
00419428  fstp       dword ptr [esp]
0041942b  call       0x41a950

// block 00419430
00419430  fsubr      qword ptr [esp + 0x60]
00419434  push       ecx
00419435  fstp       dword ptr [esp + 0x24]
00419439  fld        dword ptr [esp + 0x24]
0041943d  fstp       dword ptr [esp]
00419440  call       0x4088c0

// block 00419445
00419445  xor        ecx, ecx
00419447  fstp       dword ptr [esp + 0x1c]
0041944b  mov        eax, ebx
0041944d  call       0x41a960

// block 00419452
00419452  fld        dword ptr [esp + 0x1c]
00419456  jmp        0x419629

// block 0041945b
0041945b  mov        eax, dword ptr [0x4b0ca8]
00419460  test       eax, eax
00419462  jne        0x419477

// block 00419464
00419464  xor        ecx, ecx
00419466  mov        eax, ebx
00419468  call       0x41a900

// block 0041946d
0041946d  mov        dword ptr [esp + 0x1c], eax
00419471  fild       dword ptr [esp + 0x1c]
00419475  jmp        0x4194bc

// block 00419477
00419477  cmp        eax, 1
0041947a  jne        0x41948f

// block 0041947c
0041947c  mov        ecx, eax
0041947e  mov        eax, ebx
00419480  call       0x41a900

// block 00419485
00419485  mov        dword ptr [esp + 0x1c], eax
00419489  fild       dword ptr [esp + 0x1c]
0041948d  jmp        0x4194bc

// block 0041948f
0041948f  cmp        eax, 2
00419492  mov        eax, ebx
00419494  jne        0x4194aa

// block 00419496
00419496  mov        ecx, 2
0041949b  call       0x41a900

// block 004194a0
004194a0  mov        dword ptr [esp + 0x1c], eax
004194a4  fild       dword ptr [esp + 0x1c]
004194a8  jmp        0x4194bc

// block 004194aa
004194aa  mov        ecx, 3
004194af  call       0x41a900

// block 004194b4
004194b4  mov        dword ptr [esp + 0x1c], eax
004194b8  fild       dword ptr [esp + 0x1c]

// block 004194bc
004194bc  mov        ebx, dword ptr [ebx + 0x174c]
004194c2  fstp       dword ptr [esp + 0x34]
004194c6  mov        eax, dword ptr [ebx + 4]
004194c9  fld        dword ptr [eax]
004194cb  fsub       dword ptr [esp + 0x34]
004194cf  jmp        0x419629

// block 004194d4
004194d4  mov        eax, dword ptr [ebx + 0x1750]
004194da  xor        edi, edi
004194dc  cmp        eax, edi
004194de  je         0x4194e5

// block 004194e0
004194e0  call       0x402f20

// block 004194e5
004194e5  xor        ecx, ecx
004194e7  mov        eax, ebx
004194e9  mov        dword ptr [ebx + 0x1750], edi
004194ef  call       0x41a950

// block 004194f4
004194f4  fstp       dword ptr [ebx + 0x1754]
004194fa  fld        dword ptr [0x4a3e68]
00419500  mov        ecx, 1
00419505  mov        eax, ebx
00419507  fstp       dword ptr [ebx + 0x1758]
0041950d  call       0x41a900

// block 00419512
00419512  fldz       
00419514  push       ecx
00419515  fstp       dword ptr [esp]
00419518  lea        esi, [ebx + 0x1760]
0041951e  mov        dword ptr [ebx + 0x175c], eax
00419524  call       0x4061b0

// block 00419529
00419529  fldz       
0041952b  push       ecx
0041952c  fstp       dword ptr [esp]
0041952f  lea        esi, [ebx + 0x1764]
00419535  call       0x4061b0

// block 0041953a
0041953a  fldz       
0041953c  fcomp      dword ptr [ebx + 0x1754]
00419542  fnstsw     ax
00419544  test       ah, 5
00419547  jp         0x41962b

// block 0041954d
0041954d  push       0x18
0041954f  call       0x46c9ea

// block 00419554
00419554  mov        esi, eax
00419556  add        esp, 4
00419559  mov        dword ptr [esp + 0x1c], esi
0041955d  mov        dword ptr [esp + 0x320], edi
00419564  cmp        esi, edi
00419566  je         0x41957f

// block 00419568
00419568  push       0x11
0041956a  mov        eax, 0x11
0041956f  call       0x40ff10

// block 00419574
00419574  mov        dword ptr [ebx + 0x1750], eax
0041957a  jmp        0x41962b

// block 0041957f
0041957f  xor        eax, eax
00419581  mov        dword ptr [ebx + 0x1750], eax
00419587  jmp        0x41962b

// block 0041958c
0041958c  xor        ecx, ecx
0041958e  mov        eax, ebx
00419590  call       0x41a900

// block 00419595
00419595  mov        edx, eax
00419597  call       0x4055d0

// block 0041959c
0041959c  jmp        0x41962b

// block 004195a1
004195a1  xor        ecx, ecx
004195a3  mov        eax, ebx
004195a5  call       0x41a900

// block 004195aa
004195aa  shl        eax, 0x1e
004195ad  xor        eax, dword ptr [ebx + 0x16b8]
004195b3  and        eax, 0x40000000
004195b8  xor        dword ptr [ebx + 0x16b8], eax
004195be  jmp        0x41962b

// block 004195c0
004195c0  xor        ecx, ecx
004195c2  mov        eax, ebx
004195c4  call       0x41a900

// block 004195c9
004195c9  mov        dword ptr [ebx + 0x234], eax
004195cf  jmp        0x41962b

// block 004195d1
004195d1  mov        ecx, 1
004195d6  mov        eax, ebx
004195d8  call       0x41a900

// block 004195dd
004195dd  mov        ecx, eax
004195df  call       0x412500

// block 004195e4
004195e4  mov        esi, eax
004195e6  mov        eax, ebx
004195e8  call       0x41a910

// block 004195ed
004195ed  mov        dword ptr [eax], esi
004195ef  jmp        0x41962b

// block 004195f1
004195f1  mov        ecx, 2
004195f6  mov        eax, ebx
004195f8  call       0x41a900

// block 004195fd
004195fd  mov        ecx, eax
004195ff  call       0x412530

// block 00419604
00419604  mov        esi, eax
00419606  xor        ecx, ecx
00419608  mov        eax, ebx
0041960a  call       0x41a960

// block 0041960f
0041960f  fld        dword ptr [esi + 0x1074]
00419615  fstp       dword ptr [eax]
00419617  mov        ecx, 1
0041961c  mov        eax, ebx
0041961e  call       0x41a960

// block 00419623
00419623  fld        dword ptr [esi + 0x1078]

// block 00419629
00419629  fstp       dword ptr [eax]

// block 0041962b
0041962b  xor        eax, eax

// block 0041962d
0041962d  mov        ecx, dword ptr [esp + 0x318]
00419634  mov        dword ptr fs:[0], ecx
0041963b  pop        ecx
0041963c  pop        edi
0041963d  pop        esi
0041963e  pop        ebx
0041963f  mov        ecx, dword ptr [esp + 0x300]
00419646  xor        ecx, esp
00419648  call       0x46c932

// block 0041964d
0041964d  mov        esp, ebp
0041964f  pop        ebp
00419650  ret        
