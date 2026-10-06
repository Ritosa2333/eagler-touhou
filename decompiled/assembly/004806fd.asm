
// block 004806fd
004806fd  mov        edi, edi
004806ff  push       ebp
00480700  mov        ebp, esp
00480702  sub        esp, 0x6c
00480705  push       ebx
00480706  push       esi
00480707  push       edi
00480708  xor        edi, edi
0048070a  mov        esi, 0xffff0000
0048070f  and        dword ptr [ebp - 0x20], esi
00480712  mov        dword ptr [ebp - 0x24], edi
00480715  call       0x47d64b

// block 0048071a
0048071a  mov        ecx, dword ptr [ebp + 0xc]
0048071d  mov        ebx, eax
0048071f  xor        eax, eax
00480721  inc        eax
00480722  cmp        dword ptr [ecx], edi
00480724  je         0x480732

// block 00480726
00480726  test       dword ptr [ecx + 4], 0x200
0048072d  mov        dword ptr [ebp - 0x18], eax
00480730  jne        0x480735

// block 00480732
00480732  mov        dword ptr [ebp - 0x18], edi

// block 00480735
00480735  cmp        ebx, 0xffff
0048073b  jne        0x48074f

// block 0048073d
0048073d  mov        ecx, dword ptr [ebp + 8]
00480740  push       2
00480742  call       0x47e1ce

// block 00480747
00480747  mov        eax, dword ptr [ebp + 8]
0048074a  jmp        0x481293

// block 0048074f
0048074f  cmp        ebx, 0xfffe
00480755  jne        0x480766

// block 00480757
00480757  push       ecx
00480758  push       eax
00480759  push       dword ptr [ebp + 8]
0048075c  call       0x47ec26

// block 00480761
00480761  add        esp, 0xc
00480764  jmp        0x480747

// block 00480766
00480766  cmp        ebx, 0xfffd
0048076c  jne        0x48077d

// block 0048076e
0048076e  mov        edx, dword ptr [ecx]
00480770  mov        eax, dword ptr [ebp + 8]
00480773  mov        dword ptr [eax], edx
00480775  mov        ecx, dword ptr [ecx + 4]
00480778  jmp        0x481290

// block 0048077d
0048077d  mov        dword ptr [ebp - 4], ebx
00480780  and        dword ptr [ebp - 4], 0x8000
00480787  mov        edi, 0x1800
0048078c  mov        ecx, 0x1000
00480791  je         0x480d33

// block 00480797
00480797  xor        eax, eax
00480799  mov        dword ptr [ebp - 0x10], ebx
0048079c  and        dword ptr [ebp - 0x10], edi
0048079f  cmp        dword ptr [ebp - 0x10], 0x800
004807a6  sete       al
004807a9  mov        dword ptr [ebp - 8], eax
004807ac  test       eax, eax
004807ae  mov        eax, ebx
004807b0  je         0x4807b9

// block 004807b2
004807b2  and        eax, 0x400
004807b7  jmp        0x4807bb

// block 004807b9
004807b9  and        eax, ecx

// block 004807bb
004807bb  test       eax, eax
004807bd  je         0x4807ce

// block 004807bf
004807bf  mov        eax, ebx
004807c1  and        eax, 0x1b00
004807c6  cmp        eax, ecx
004807c8  je         0x480d33

// block 004807ce
004807ce  cmp        dword ptr [ebp - 8], 0
004807d2  mov        eax, ebx
004807d4  je         0x4807dd

// block 004807d6
004807d6  and        eax, 0x400
004807db  jmp        0x4807df

// block 004807dd
004807dd  and        eax, ecx

// block 004807df
004807df  test       eax, eax
004807e1  je         0x480800

// block 004807e3
004807e3  mov        eax, ebx
004807e5  and        eax, 0x1b00
004807ea  cmp        eax, 0x1100
004807ef  je         0x480d33

// block 004807f5
004807f5  cmp        eax, 0x1200
004807fa  je         0x480d33

// block 00480800
00480800  test       ebx, 0x4000
00480806  je         0x48085e

// block 00480808
00480808  mov        eax, dword ptr [0x4b4328]
0048080d  mov        ecx, eax
0048080f  shr        ecx, 1
00480811  not        ecx
00480813  test       cl, 1
00480816  je         0x480846

// block 00480818
00480818  shr        eax, 3
0048081b  not        eax
0048081d  test       al, 1
0048081f  je         0x480846

// block 00480821
00480821  lea        eax, [ebp - 0x4c]
00480824  push       eax
00480825  call       0x480665

// block 0048082a
0048082a  push       eax
0048082b  lea        eax, [ebp - 0x44]
0048082e  push       0x20
00480830  push       eax
00480831  call       0x47ec02

// block 00480836
00480836  mov        ecx, dword ptr [eax]
00480838  mov        dword ptr [ebp - 0x24], ecx
0048083b  mov        eax, dword ptr [eax + 4]
0048083e  add        esp, 0x10
00480841  mov        dword ptr [ebp - 0x20], eax
00480844  jmp        0x480859

// block 00480846
00480846  lea        eax, [ebp - 0x4c]
00480849  push       eax
0048084a  call       0x480665

// block 0048084f
0048084f  pop        ecx
00480850  push       eax
00480851  lea        ecx, [ebp - 0x24]
00480854  call       0x47ddc1

// block 00480859
00480859  mov        ecx, 0x1000

// block 0048085e
0048085e  mov        edx, dword ptr [ebp - 8]
00480861  mov        eax, ebx
00480863  test       edx, edx
00480865  je         0x48086e

// block 00480867
00480867  and        eax, 0x400
0048086c  jmp        0x480870

// block 0048086e
0048086e  and        eax, ecx

// block 00480870
00480870  test       eax, eax
00480872  je         0x480971

// block 00480878
00480878  cmp        dword ptr [ebp - 0x10], edi
0048087b  jne        0x480971

// block 00480881
00480881  lea        eax, [ebp - 0x4c]
00480884  push       eax
00480885  call       0x47f377

// block 0048088a
0048088a  pop        ecx
0048088b  mov        ecx, dword ptr [ebp + 0xc]
0048088e  push       eax
0048088f  lea        eax, [ebp - 0x44]
00480892  push       eax
00480893  push       0x7b
00480895  lea        eax, [ebp - 0x3c]
00480898  push       eax
00480899  call       0x47ec6e

// block 0048089e
0048089e  mov        ecx, eax
004808a0  call       0x47e9ae

// block 004808a5
004808a5  push       eax
004808a6  lea        ecx, [ebp - 0x24]
004808a9  call       0x47e7bf

// block 004808ae
004808ae  lea        eax, [ebp - 0x4c]
004808b1  push       eax
004808b2  call       0x47e975

// block 004808b7
004808b7  mov        esi, 0x1000
004808bc  pop        ecx
004808bd  test       dword ptr [0x4b4328], esi
004808c3  jne        0x4808f0

// block 004808c5
004808c5  push       0x49e074
004808ca  lea        eax, [ebp - 0x44]
004808cd  push       eax
004808ce  lea        eax, [ebp - 0x4c]
004808d1  push       eax
004808d2  lea        eax, [ebp - 0x3c]
004808d5  push       0x2c
004808d7  push       eax
004808d8  call       0x47ec02

// block 004808dd
004808dd  add        esp, 0xc
004808e0  mov        ecx, eax
004808e2  call       0x47ec92

// block 004808e7
004808e7  push       eax
004808e8  lea        ecx, [ebp - 0x24]
004808eb  call       0x47e7bf

// block 004808f0
004808f0  push       0x49e070
004808f5  lea        ecx, [ebp - 0x24]
004808f8  call       0x47ea48

// block 004808fd
004808fd  lea        eax, [ebp - 0x4c]
00480900  push       eax
00480901  call       0x47e8cb

// block 00480906
00480906  mov        eax, dword ptr [0x4b4328]
0048090b  pop        ecx
0048090c  mov        ecx, eax
0048090e  shr        ecx, 1
00480910  not        ecx
00480912  test       cl, 1
00480915  je         0x480f88

// block 0048091b
0048091b  mov        ecx, eax
0048091d  shr        ecx, 4
00480920  not        ecx
00480922  test       cl, 1
00480925  je         0x480f88

// block 0048092b
0048092b  test       esi, eax
0048092d  jne        0x480f88

// block 00480933
00480933  lea        eax, [ebp - 0x24]
00480936  push       eax
00480937  lea        eax, [ebp - 0x44]
0048093a  push       eax
0048093b  push       0x20
0048093d  lea        eax, [ebp - 0x3c]
00480940  push       eax
00480941  lea        eax, [ebp - 0x4c]
00480944  push       eax
00480945  lea        eax, [ebp - 0x34]
00480948  push       0x20
0048094a  push       eax
0048094b  call       0x47ec02

// block 00480950
00480950  add        esp, 0xc
00480953  mov        ecx, eax
00480955  call       0x47ec6e

// block 0048095a
0048095a  mov        ecx, eax
0048095c  call       0x47e9ae

// block 00480961
00480961  mov        ecx, dword ptr [eax]
00480963  mov        dword ptr [ebp - 0x24], ecx
00480966  mov        ecx, dword ptr [eax + 4]
00480969  mov        dword ptr [ebp - 0x20], ecx
0048096c  jmp        0x480f90

// block 00480971
00480971  and        dword ptr [ebp - 0x40], esi
00480974  and        dword ptr [ebp - 0x38], esi
00480977  and        dword ptr [ebp - 0x10], esi
0048097a  and        dword ptr [ebp - 0x48], esi
0048097d  and        dword ptr [ebp - 0x28], esi
00480980  xor        edi, edi
00480982  mov        dword ptr [ebp - 0x44], edi
00480985  mov        dword ptr [ebp - 0x3c], edi
00480988  mov        dword ptr [ebp - 0x14], edi
0048098b  mov        dword ptr [ebp - 0x4c], edi
0048098e  mov        dword ptr [ebp - 0x2c], edi
00480991  mov        eax, ebx
00480993  cmp        edx, edi
00480995  je         0x48099e

// block 00480997
00480997  and        eax, 0x400
0048099c  jmp        0x4809a0

// block 0048099e
0048099e  and        eax, ecx

// block 004809a0
004809a0  cmp        eax, edi
004809a2  je         0x480a2e

// block 004809a8
004809a8  cmp        edx, edi
004809aa  je         0x480a17

// block 004809ac
004809ac  mov        eax, ebx
004809ae  and        eax, 0x700
004809b3  cmp        eax, 0x600
004809b8  jne        0x4809f0

// block 004809ba
004809ba  lea        eax, [ebp - 0x34]
004809bd  push       eax
004809be  call       0x47f361

// block 004809c3
004809c3  mov        ecx, dword ptr [eax]
004809c5  mov        eax, dword ptr [eax + 4]
004809c8  mov        dword ptr [ebp - 0x40], eax
004809cb  lea        eax, [ebp - 0x34]
004809ce  push       eax
004809cf  mov        dword ptr [ebp - 0x44], ecx
004809d2  call       0x47f361

// block 004809d7
004809d7  mov        ecx, dword ptr [eax]
004809d9  mov        eax, dword ptr [eax + 4]
004809dc  mov        dword ptr [ebp - 0x38], eax
004809df  lea        eax, [ebp - 0x34]
004809e2  push       eax
004809e3  mov        dword ptr [ebp - 0x3c], ecx
004809e6  call       0x47f361

// block 004809eb
004809eb  add        esp, 0xc
004809ee  jmp        0x480a0c

// block 004809f0
004809f0  cmp        edx, edi
004809f2  je         0x480a17

// block 004809f4
004809f4  mov        eax, ebx
004809f6  and        eax, 0x700
004809fb  cmp        eax, 0x500
00480a00  jne        0x480a17

// block 00480a02
00480a02  lea        eax, [ebp - 0x34]
00480a05  push       eax
00480a06  call       0x47f361

// block 00480a0b
00480a0b  pop        ecx

// block 00480a0c
00480a0c  mov        ecx, dword ptr [eax]
00480a0e  mov        eax, dword ptr [eax + 4]
00480a11  mov        dword ptr [ebp - 0x10], eax
00480a14  mov        dword ptr [ebp - 0x14], ecx

// block 00480a17
00480a17  lea        eax, [ebp - 0x34]
00480a1a  push       eax
00480a1b  call       0x47f361

// block 00480a20
00480a20  pop        ecx
00480a21  mov        ecx, dword ptr [eax]
00480a23  mov        eax, dword ptr [eax + 4]
00480a26  mov        dword ptr [ebp - 0x4c], ecx
00480a29  mov        dword ptr [ebp - 0x48], eax
00480a2c  xor        edi, edi

// block 00480a2e
00480a2e  cmp        dword ptr [ebp - 8], edi
00480a31  je         0x480a79

// block 00480a33
00480a33  cmp        dword ptr [ebp - 8], 0
00480a37  je         0x480a47

// block 00480a39
00480a39  mov        eax, ebx
00480a3b  and        eax, 0x700
00480a40  cmp        eax, 0x200
00480a45  je         0x480a79

// block 00480a47
00480a47  mov        eax, dword ptr [0x4b4328]
00480a4c  and        eax, 0x60
00480a4f  cmp        al, 0x60
00480a51  lea        eax, [ebp - 0x34]
00480a54  push       eax
00480a55  je         0x480a6a

// block 00480a57
00480a57  call       0x47e0f7

// block 00480a5c
00480a5c  pop        ecx
00480a5d  mov        ecx, dword ptr [eax]
00480a5f  mov        eax, dword ptr [eax + 4]
00480a62  mov        dword ptr [ebp - 0x2c], ecx
00480a65  mov        dword ptr [ebp - 0x28], eax
00480a68  jmp        0x480a79

// block 00480a6a
00480a6a  call       0x47e0f7

// block 00480a6f
00480a6f  pop        ecx
00480a70  push       eax
00480a71  lea        ecx, [ebp - 0x2c]
00480a74  call       0x47ddc1

// block 00480a79
00480a79  mov        eax, dword ptr [0x4b4328]
00480a7e  mov        ecx, eax
00480a80  shr        ecx, 1
00480a82  not        ecx
00480a84  test       cl, 1
00480a87  je         0x480ab8

// block 00480a89
00480a89  shr        eax, 4
00480a8c  not        eax
00480a8e  test       al, 1
00480a90  je         0x480ab8

// block 00480a92
00480a92  lea        eax, [ebp - 0x24]
00480a95  push       eax
00480a96  lea        eax, [ebp - 0x34]
00480a99  push       eax
00480a9a  lea        eax, [ebp - 0x54]
00480a9d  push       eax
00480a9e  call       0x47e8cb

// block 00480aa3
00480aa3  pop        ecx
00480aa4  mov        ecx, eax
00480aa6  call       0x47e9ae

// block 00480aab
00480aab  mov        ecx, dword ptr [eax]
00480aad  mov        dword ptr [ebp - 0x24], ecx
00480ab0  mov        eax, dword ptr [eax + 4]
00480ab3  mov        dword ptr [ebp - 0x20], eax
00480ab6  jmp        0x480acb

// block 00480ab8
00480ab8  lea        eax, [ebp - 0x54]
00480abb  push       eax
00480abc  call       0x47e8cb

// block 00480ac1
00480ac1  pop        ecx
00480ac2  push       eax
00480ac3  lea        ecx, [ebp - 0x24]
00480ac6  call       0x47ddc1

// block 00480acb
00480acb  mov        eax, dword ptr [ebp + 0xc]
00480ace  cmp        dword ptr [eax], 0
00480ad1  je         0x480b0a

// block 00480ad3
00480ad3  cmp        dword ptr [ebp - 0x24], 0
00480ad7  je         0x480aff

// block 00480ad9
00480ad9  test       dword ptr [0x4b4328], 0x1000
00480ae3  jne        0x480aff

// block 00480ae5
00480ae5  push       eax
00480ae6  lea        eax, [ebp - 0x54]
00480ae9  push       0x20
00480aeb  push       eax
00480aec  call       0x47ec02

// block 00480af1
00480af1  add        esp, 0xc
00480af4  push       eax
00480af5  lea        ecx, [ebp - 0x24]
00480af8  call       0x47e7bf

// block 00480afd
00480afd  jmp        0x480b0a

// block 00480aff
00480aff  mov        ecx, dword ptr [eax]
00480b01  mov        eax, dword ptr [eax + 4]
00480b04  mov        dword ptr [ebp - 0x24], ecx
00480b07  mov        dword ptr [ebp - 0x20], eax

// block 00480b0a
00480b0a  and        dword ptr [ebp - 0x30], esi
00480b0d  xor        edi, edi
00480b0f  mov        dword ptr [ebp - 0x34], edi
00480b12  cmp        dword ptr [ebp - 0x18], edi
00480b15  je         0x480b58

// block 00480b17
00480b17  lea        eax, [ebp - 0x54]
00480b1a  push       edi
00480b1b  push       eax
00480b1c  call       0x47e46b

// block 00480b21
00480b21  push       eax
00480b22  lea        eax, [ebp - 0x1c]
00480b25  push       0x49fb38
00480b2a  push       eax
00480b2b  call       0x47ec4a

// block 00480b30
00480b30  add        esp, 0x14
00480b33  push       eax
00480b34  lea        ecx, [ebp - 0x24]
00480b37  call       0x47e7bf

// block 00480b3c
00480b3c  test       dword ptr [0x4b4328], 0x1000
00480b46  je         0x480b91

// block 00480b48
00480b48  mov        ecx, dword ptr [ebp - 0x24]
00480b4b  mov        eax, dword ptr [ebp + 8]
00480b4e  mov        dword ptr [eax], ecx
00480b50  mov        ecx, dword ptr [ebp - 0x20]
00480b53  jmp        0x481290

// block 00480b58
00480b58  xor        edi, edi
00480b5a  push       edi
00480b5b  push       8
00480b5d  mov        ecx, 0x4b42f8
00480b62  call       0x47dc29

// block 00480b67
00480b67  cmp        eax, edi
00480b69  je         0x480b7a

// block 00480b6b
00480b6b  mov        dword ptr [eax], edi
00480b6d  mov        byte ptr [eax + 4], 0
00480b71  and        dword ptr [eax + 4], 0xffff00ff
00480b78  mov        edi, eax

// block 00480b7a
00480b7a  lea        eax, [ebp - 0x54]
00480b7d  push       edi
00480b7e  push       eax
00480b7f  call       0x47e46b

// block 00480b84
00480b84  pop        ecx
00480b85  pop        ecx
00480b86  mov        ecx, dword ptr [eax]
00480b88  mov        eax, dword ptr [eax + 4]
00480b8b  mov        dword ptr [ebp - 0x34], ecx
00480b8e  mov        dword ptr [ebp - 0x30], eax

// block 00480b91
00480b91  mov        esi, dword ptr [ebp - 8]
00480b94  mov        eax, ebx
00480b96  test       esi, esi
00480b98  je         0x480ba1

// block 00480b9a
00480b9a  and        eax, 0x400
00480b9f  jmp        0x480ba6

// block 00480ba1
00480ba1  and        eax, 0x1000

// block 00480ba6
00480ba6  test       eax, eax
00480ba8  je         0x480c7f

// block 00480bae
00480bae  test       esi, esi
00480bb0  je         0x480c58

// block 00480bb6
00480bb6  mov        eax, ebx
00480bb8  and        eax, 0x700
00480bbd  cmp        eax, 0x600
00480bc2  jne        0x480c19

// block 00480bc4
00480bc4  push       0x2c
00480bc6  lea        eax, [ebp - 0x54]
00480bc9  push       eax
00480bca  lea        eax, [ebp - 0x14]
00480bcd  push       eax
00480bce  lea        eax, [ebp - 0x1c]
00480bd1  push       eax
00480bd2  push       0x2c
00480bd4  lea        eax, [ebp - 0xc]
00480bd7  push       eax
00480bd8  lea        eax, [ebp - 0x3c]
00480bdb  push       eax
00480bdc  lea        eax, [ebp - 0x5c]
00480bdf  push       eax
00480be0  push       0x2c
00480be2  lea        eax, [ebp - 0x64]
00480be5  push       eax
00480be6  lea        eax, [ebp - 0x44]
00480be9  push       eax
00480bea  lea        eax, [ebp - 0x6c]
00480bed  push       0x49e060
00480bf2  push       eax
00480bf3  call       0x47ec4a

// block 00480bf8
00480bf8  add        esp, 0xc
00480bfb  mov        ecx, eax
00480bfd  call       0x47ec6e

// block 00480c02
00480c02  mov        ecx, eax
00480c04  call       0x47e9ae

// block 00480c09
00480c09  mov        ecx, eax
00480c0b  call       0x47ec6e

// block 00480c10
00480c10  mov        ecx, eax
00480c12  call       0x47e9ae

// block 00480c17
00480c17  jmp        0x480c46

// block 00480c19
00480c19  test       esi, esi
00480c1b  je         0x480c58

// block 00480c1d
00480c1d  mov        eax, ebx
00480c1f  and        eax, 0x700
00480c24  cmp        eax, 0x500
00480c29  jne        0x480c58

// block 00480c2b
00480c2b  push       0x2c
00480c2d  lea        eax, [ebp - 0x6c]
00480c30  push       eax
00480c31  lea        eax, [ebp - 0x14]
00480c34  push       eax
00480c35  lea        eax, [ebp - 0x64]
00480c38  push       0x49e054
00480c3d  push       eax
00480c3e  call       0x47ec4a

// block 00480c43
00480c43  add        esp, 0xc

// block 00480c46
00480c46  mov        ecx, eax
00480c48  call       0x47ec6e

// block 00480c4d
00480c4d  push       eax
00480c4e  lea        ecx, [ebp - 0x24]
00480c51  call       0x47e7bf

// block 00480c56
00480c56  jmp        0x480c65

// block 00480c58
00480c58  push       0x49e048
00480c5d  lea        ecx, [ebp - 0x24]
00480c60  call       0x47ea48

// block 00480c65
00480c65  push       0x49e074
00480c6a  lea        eax, [ebp - 0x6c]
00480c6d  push       eax
00480c6e  lea        ecx, [ebp - 0x4c]
00480c71  call       0x47ec92

// block 00480c76
00480c76  push       eax
00480c77  lea        ecx, [ebp - 0x24]
00480c7a  call       0x47e7bf

// block 00480c7f
00480c7f  push       0x29
00480c81  lea        eax, [ebp - 0x6c]
00480c84  push       eax
00480c85  lea        eax, [ebp - 0x64]
00480c88  push       eax
00480c89  call       0x47eedc

// block 00480c8e
00480c8e  push       eax
00480c8f  lea        eax, [ebp - 0x5c]
00480c92  push       0x28
00480c94  push       eax
00480c95  call       0x47ec02

// block 00480c9a
00480c9a  add        esp, 0x10
00480c9d  mov        ecx, eax
00480c9f  call       0x47ec6e

// block 00480ca4
00480ca4  push       eax
00480ca5  lea        ecx, [ebp - 0x24]
00480ca8  call       0x47e7bf

// block 00480cad
00480cad  test       esi, esi
00480caf  je         0x480ccb

// block 00480cb1
00480cb1  mov        eax, ebx
00480cb3  and        eax, 0x700
00480cb8  cmp        eax, 0x200
00480cbd  je         0x480ccb

// block 00480cbf
00480cbf  lea        eax, [ebp - 0x2c]
00480cc2  push       eax
00480cc3  lea        ecx, [ebp - 0x24]
00480cc6  call       0x47e7bf

// block 00480ccb
00480ccb  mov        eax, dword ptr [0x4b4328]
00480cd0  shr        eax, 8
00480cd3  not        eax
00480cd5  test       al, 1
00480cd7  lea        eax, [ebp - 0x6c]
00480cda  push       eax
00480cdb  je         0x480cee

// block 00480cdd
00480cdd  call       0x47efb8

// block 00480ce2
00480ce2  pop        ecx
00480ce3  push       eax
00480ce4  lea        ecx, [ebp - 0x24]
00480ce7  call       0x47e7bf

// block 00480cec
00480cec  jmp        0x480cfd

// block 00480cee
00480cee  call       0x47efb8

// block 00480cf3
00480cf3  pop        ecx
00480cf4  push       eax
00480cf5  lea        ecx, [ebp - 0x24]
00480cf8  call       0x47ddc1

// block 00480cfd
00480cfd  mov        eax, dword ptr [0x4b4328]
00480d02  shr        eax, 2
00480d05  not        eax
00480d07  test       al, 1
00480d09  je         0x480f88

// block 00480d0f
00480d0f  test       edi, edi
00480d11  je         0x480f88

// block 00480d17
00480d17  mov        eax, dword ptr [ebp - 0x24]
00480d1a  mov        ecx, dword ptr [ebp - 0x30]
00480d1d  mov        dword ptr [edi], eax
00480d1f  mov        eax, dword ptr [ebp - 0x20]
00480d22  mov        dword ptr [edi + 4], eax
00480d25  mov        eax, dword ptr [ebp - 0x34]
00480d28  mov        dword ptr [ebp - 0x24], eax
00480d2b  mov        dword ptr [ebp - 0x20], ecx
00480d2e  jmp        0x480f8b

// block 00480d33
00480d33  push       dword ptr [ebp + 0xc]
00480d36  lea        ecx, [ebp - 0x24]
00480d39  call       0x47e7bf

// block 00480d3e
00480d3e  mov        eax, dword ptr [ebp - 4]
00480d41  mov        ecx, 0x7c00
00480d46  test       eax, eax
00480d48  jne        0x480de3

// block 00480d4e
00480d4e  mov        edx, ebx
00480d50  and        edx, ecx
00480d52  cmp        edx, 0x6800
00480d58  jne        0x480d6d

// block 00480d5a
00480d5a  lea        eax, [ebp - 0x24]
00480d5d  push       eax
00480d5e  push       dword ptr [ebp + 8]
00480d61  call       0x47f3a3

// block 00480d66
00480d66  pop        ecx
00480d67  pop        ecx
00480d68  jmp        0x480747

// block 00480d6d
00480d6d  test       eax, eax
00480d6f  jne        0x480de3

// block 00480d71
00480d71  mov        edx, ebx
00480d73  and        edx, ecx
00480d75  cmp        edx, 0x7000
00480d7b  je         0x480d5a

// block 00480d7d
00480d7d  test       eax, eax
00480d7f  jne        0x480de3

// block 00480d81
00480d81  mov        edx, ebx
00480d83  and        edx, ecx
00480d85  cmp        edx, 0x6000
00480d8b  jne        0x480dc5

// block 00480d8d
00480d8d  push       0x49e070
00480d92  push       dword ptr [ebp + 8]
00480d95  lea        eax, [ebp - 0x6c]
00480d98  push       eax
00480d99  call       0x47f38d

// block 00480d9e
00480d9e  pop        ecx
00480d9f  push       eax
00480da0  lea        eax, [ebp - 0x64]
00480da3  push       eax
00480da4  push       0x7b
00480da6  lea        eax, [ebp - 0x5c]
00480da9  push       eax
00480daa  lea        ecx, [ebp - 0x24]
00480dad  call       0x47ec6e

// block 00480db2
00480db2  mov        ecx, eax
00480db4  call       0x47e9ae

// block 00480db9
00480db9  mov        ecx, eax
00480dbb  call       0x47ec92

// block 00480dc0
00480dc0  jmp        0x480747

// block 00480dc5
00480dc5  test       eax, eax
00480dc7  jne        0x480de3

// block 00480dc9
00480dc9  mov        edx, ebx
00480dcb  and        edx, ecx
00480dcd  cmp        edx, ecx
00480dcf  jne        0x480ddf

// block 00480dd1
00480dd1  lea        eax, [ebp - 0x24]
00480dd4  push       eax
00480dd5  push       dword ptr [ebp + 8]
00480dd8  call       0x47ebae

// block 00480ddd
00480ddd  jmp        0x480d66

// block 00480ddf
00480ddf  test       eax, eax
00480de1  je         0x480dee

// block 00480de3
00480de3  mov        eax, ebx
00480de5  and        eax, edi
00480de7  sub        eax, 0x800
00480dec  jmp        0x480df5

// block 00480dee
00480dee  mov        eax, ebx
00480df0  and        eax, 0x6000

// block 00480df5
00480df5  neg        eax
00480df7  sbb        eax, eax
00480df9  inc        eax
00480dfa  test       eax, eax
00480dfc  mov        esi, 0x1000
00480e01  mov        eax, ebx
00480e03  je         0x480e0c

// block 00480e05
00480e05  and        eax, 0x400
00480e0a  jmp        0x480e0e

// block 00480e0c
00480e0c  and        eax, esi

// block 00480e0e
00480e0e  test       eax, eax
00480e10  je         0x480e36

// block 00480e12
00480e12  mov        eax, dword ptr [ebp - 4]
00480e15  mov        edx, ebx
00480e17  and        edx, 0x1b00
00480e1d  sub        edx, esi
00480e1f  neg        edx
00480e21  sbb        edx, edx
00480e23  inc        edx
00480e24  neg        eax
00480e26  sbb        eax, eax
00480e28  test       edx, eax
00480e2a  je         0x480e36

// block 00480e2c
00480e2c  push       0x49e024
00480e31  jmp        0x480eda

// block 00480e36
00480e36  cmp        dword ptr [ebp - 4], 0
00480e3a  mov        eax, ebx
00480e3c  je         0x480e47

// block 00480e3e
00480e3e  and        eax, edi
00480e40  sub        eax, 0x800
00480e45  jmp        0x480e4c

// block 00480e47
00480e47  and        eax, 0x6000

// block 00480e4c
00480e4c  neg        eax
00480e4e  sbb        eax, eax
00480e50  inc        eax
00480e51  test       eax, eax
00480e53  mov        eax, ebx
00480e55  je         0x480e5e

// block 00480e57
00480e57  and        eax, 0x400
00480e5c  jmp        0x480e60

// block 00480e5e
00480e5e  and        eax, esi

// block 00480e60
00480e60  test       eax, eax
00480e62  je         0x480e89

// block 00480e64
00480e64  mov        eax, dword ptr [ebp - 4]
00480e67  mov        edx, ebx
00480e69  and        edx, 0x1b00
00480e6f  sub        edx, 0x1100
00480e75  neg        edx
00480e77  sbb        edx, edx
00480e79  inc        edx
00480e7a  neg        eax
00480e7c  sbb        eax, eax
00480e7e  test       edx, eax
00480e80  je         0x480e89

// block 00480e82
00480e82  push       0x49dff0
00480e87  jmp        0x480eda

// block 00480e89
00480e89  cmp        dword ptr [ebp - 4], 0
00480e8d  mov        eax, ebx
00480e8f  je         0x480e9a

// block 00480e91
00480e91  and        eax, edi
00480e93  sub        eax, 0x800
00480e98  jmp        0x480e9f

// block 00480e9a
00480e9a  and        eax, 0x6000

// block 00480e9f
00480e9f  neg        eax
00480ea1  sbb        eax, eax
00480ea3  inc        eax
00480ea4  test       eax, eax
00480ea6  mov        eax, ebx
00480ea8  je         0x480eb1

// block 00480eaa
00480eaa  and        eax, 0x400
00480eaf  jmp        0x480eb3

// block 00480eb1
00480eb1  and        eax, esi

// block 00480eb3
00480eb3  test       eax, eax
00480eb5  je         0x480ee4

// block 00480eb7
00480eb7  mov        eax, dword ptr [ebp - 4]
00480eba  mov        edx, ebx
00480ebc  and        edx, 0x1b00
00480ec2  sub        edx, 0x1200
00480ec8  neg        edx
00480eca  sbb        edx, edx
00480ecc  inc        edx
00480ecd  neg        eax
00480ecf  sbb        eax, eax
00480ed1  test       edx, eax
00480ed3  je         0x480ee4

// block 00480ed5
00480ed5  push       0x49dfc0

// block 00480eda
00480eda  lea        ecx, [ebp - 0x24]
00480edd  call       0x47ea48

// block 00480ee2
00480ee2  jmp        0x480ef9

// block 00480ee4
00480ee4  cmp        dword ptr [ebp - 4], 0
00480ee8  jne        0x480eff

// block 00480eea
00480eea  mov        eax, ebx
00480eec  and        eax, ecx
00480eee  cmp        eax, 0x7800
00480ef3  je         0x480b48

// block 00480ef9
00480ef9  cmp        dword ptr [ebp - 4], 0
00480efd  je         0x480f0a

// block 00480eff
00480eff  mov        eax, ebx
00480f01  and        eax, edi
00480f03  sub        eax, 0x800
00480f08  jmp        0x480f11

// block 00480f0a
00480f0a  mov        eax, ebx
00480f0c  and        eax, 0x6000

// block 00480f11
00480f11  neg        eax
00480f13  sbb        eax, eax
00480f15  inc        eax
00480f16  test       eax, eax
00480f18  mov        eax, ebx
00480f1a  je         0x480f23

// block 00480f1c
00480f1c  and        eax, 0x400
00480f21  jmp        0x480f25

// block 00480f23
00480f23  and        eax, esi

// block 00480f25
00480f25  test       eax, eax
00480f27  je         0x480f74

// block 00480f29
00480f29  mov        ecx, dword ptr [ebp - 4]
00480f2c  mov        eax, ebx
00480f2e  and        eax, 0x1b00
00480f33  xor        edx, edx
00480f35  cmp        eax, 0x1100
00480f3a  sete       dl
00480f3d  neg        ecx
00480f3f  sbb        ecx, ecx
00480f41  test       edx, ecx
00480f43  jne        0x480f5a

// block 00480f45
00480f45  mov        ecx, dword ptr [ebp - 4]
00480f48  xor        edx, edx
00480f4a  cmp        eax, 0x1200
00480f4f  sete       dl
00480f52  neg        ecx
00480f54  sbb        ecx, ecx
00480f56  test       edx, ecx
00480f58  je         0x480f74

// block 00480f5a
00480f5a  lea        eax, [ebp - 0x24]
00480f5d  push       eax
00480f5e  lea        eax, [ebp - 0x6c]
00480f61  push       0x49fb38
00480f66  push       eax
00480f67  call       0x47ec4a

// block 00480f6c
00480f6c  add        esp, 0xc
00480f6f  jmp        0x480961

// block 00480f74
00480f74  lea        eax, [ebp - 0x24]
00480f77  push       eax
00480f78  lea        eax, [ebp - 0x6c]
00480f7b  push       eax
00480f7c  call       0x48289f

// block 00480f81
00480f81  pop        ecx
00480f82  pop        ecx
00480f83  jmp        0x480961

// block 00480f88
00480f88  mov        ecx, dword ptr [ebp - 0x20]

// block 00480f8b
00480f8b  mov        edi, 0x1800

// block 00480f90
00480f90  xor        edx, edx
00480f92  mov        esi, 0x800
00480f97  mov        eax, ebx
00480f99  cmp        dword ptr [ebp - 4], edx
00480f9c  je         0x480fa4

// block 00480f9e
00480f9e  and        eax, edi
00480fa0  sub        eax, esi
00480fa2  jmp        0x480fa9

// block 00480fa4
00480fa4  and        eax, 0x6000

// block 00480fa9
00480fa9  neg        eax
00480fab  sbb        eax, eax
00480fad  inc        eax
00480fae  cmp        eax, edx
00480fb0  je         0x481209

// block 00480fb6
00480fb6  mov        eax, dword ptr [0x4b4328]
00480fbb  shr        eax, 9
00480fbe  not        eax
00480fc0  test       al, 1
00480fc2  je         0x481105

// block 00480fc8
00480fc8  mov        eax, ebx
00480fca  cmp        dword ptr [ebp - 4], edx
00480fcd  je         0x480fd5

// block 00480fcf
00480fcf  and        eax, edi
00480fd1  sub        eax, esi
00480fd3  jmp        0x480fda

// block 00480fd5
00480fd5  and        eax, 0x6000

// block 00480fda
00480fda  neg        eax
00480fdc  sbb        eax, eax
00480fde  inc        eax
00480fdf  cmp        eax, edx
00480fe1  je         0x481022

// block 00480fe3
00480fe3  cmp        dword ptr [ebp - 4], edx
00480fe6  je         0x480ffb

// block 00480fe8
00480fe8  mov        eax, ebx
00480fea  and        eax, 0x700
00480fef  sub        eax, 0x200
00480ff4  neg        eax
00480ff6  sbb        eax, eax
00480ff8  inc        eax
00480ff9  jmp        0x480ffe

// block 00480ffb
00480ffb  xor        eax, eax
00480ffd  inc        eax

// block 00480ffe
00480ffe  cmp        eax, edx
00481000  je         0x481022

// block 00481002
00481002  lea        eax, [ebp - 0x24]
00481005  push       eax
00481006  lea        eax, [ebp - 0x6c]
00481009  push       0x49dfb8
0048100e  push       eax
0048100f  call       0x47ec4a

// block 00481014
00481014  mov        ecx, dword ptr [eax]
00481016  mov        dword ptr [ebp - 0x24], ecx
00481019  mov        ecx, dword ptr [eax + 4]
0048101c  add        esp, 0xc
0048101f  mov        dword ptr [ebp - 0x20], ecx

// block 00481022
00481022  mov        edx, dword ptr [ebp - 4]
00481025  test       edx, edx
00481027  je         0x481047

// block 00481029
00481029  mov        eax, ebx
0048102b  and        eax, 0x700
00481030  cmp        eax, 0x100
00481035  je         0x4810e5

// block 0048103b
0048103b  test       edx, edx
0048103d  je         0x481047

// block 0048103f
0048103f  mov        eax, ebx
00481041  and        eax, edi
00481043  sub        eax, esi
00481045  jmp        0x48104e

// block 00481047
00481047  mov        eax, ebx
00481049  and        eax, 0x6000

// block 0048104e
0048104e  neg        eax
00481050  sbb        eax, eax
00481052  inc        eax
00481053  test       eax, eax
00481055  mov        eax, ebx
00481057  je         0x481060

// block 00481059
00481059  and        eax, 0x400
0048105e  jmp        0x481065

// block 00481060
00481060  and        eax, 0x1000

// block 00481065
00481065  test       eax, eax
00481067  je         0x481105

// block 0048106d
0048106d  mov        eax, ebx
0048106f  test       edx, edx
00481071  je         0x481079

// block 00481073
00481073  and        eax, edi
00481075  sub        eax, esi
00481077  jmp        0x48107e

// block 00481079
00481079  and        eax, 0x6000

// block 0048107e
0048107e  neg        eax
00481080  sbb        eax, eax
00481082  inc        eax
00481083  test       eax, eax
00481085  je         0x481095

// block 00481087
00481087  mov        eax, ebx
00481089  and        eax, 0x700
0048108e  cmp        eax, 0x500
00481093  je         0x4810e5

// block 00481095
00481095  mov        eax, ebx
00481097  test       edx, edx
00481099  je         0x4810a1

// block 0048109b
0048109b  and        eax, edi
0048109d  sub        eax, esi
0048109f  jmp        0x4810a6

// block 004810a1
004810a1  and        eax, 0x6000

// block 004810a6
004810a6  neg        eax
004810a8  sbb        eax, eax
004810aa  inc        eax
004810ab  test       eax, eax
004810ad  je         0x4810bd

// block 004810af
004810af  mov        eax, ebx
004810b1  and        eax, 0x700
004810b6  cmp        eax, 0x600
004810bb  je         0x4810e5

// block 004810bd
004810bd  mov        eax, ebx
004810bf  test       edx, edx
004810c1  je         0x4810c9

// block 004810c3
004810c3  and        eax, edi
004810c5  sub        eax, esi
004810c7  jmp        0x4810ce

// block 004810c9
004810c9  and        eax, 0x6000

// block 004810ce
004810ce  neg        eax
004810d0  sbb        eax, eax
004810d2  inc        eax
004810d3  test       eax, eax
004810d5  je         0x481105

// block 004810d7
004810d7  mov        eax, ebx
004810d9  and        eax, 0x700
004810de  cmp        eax, 0x400
004810e3  jne        0x481105

// block 004810e5
004810e5  lea        eax, [ebp - 0x24]
004810e8  push       eax
004810e9  lea        eax, [ebp - 0x6c]
004810ec  push       0x49dfac
004810f1  push       eax
004810f2  call       0x47ec4a

// block 004810f7
004810f7  mov        ecx, dword ptr [eax]
004810f9  mov        dword ptr [ebp - 0x24], ecx
004810fc  mov        ecx, dword ptr [eax + 4]
004810ff  add        esp, 0xc
00481102  mov        dword ptr [ebp - 0x20], ecx

// block 00481105
00481105  mov        eax, dword ptr [0x4b4328]
0048110a  shr        eax, 7
0048110d  not        eax
0048110f  test       al, 1
00481111  je         0x481209

// block 00481117
00481117  cmp        dword ptr [ebp - 4], 0
0048111b  mov        eax, ebx
0048111d  je         0x481125

// block 0048111f
0048111f  and        eax, edi
00481121  sub        eax, esi
00481123  jmp        0x48112a

// block 00481125
00481125  and        eax, 0x6000

// block 0048112a
0048112a  neg        eax
0048112c  sbb        eax, eax
0048112e  inc        eax
0048112f  test       eax, eax
00481131  je         0x481163

// block 00481133
00481133  cmp        dword ptr [ebp - 4], 0
00481137  mov        eax, ebx
00481139  je         0x481148

// block 0048113b
0048113b  and        al, 0xc0
0048113d  xor        edx, edx
0048113f  cmp        al, 0x40
00481141  sete       dl
00481144  mov        eax, edx
00481146  jmp        0x481151

// block 00481148
00481148  and        eax, edi
0048114a  sub        eax, esi
0048114c  neg        eax
0048114e  sbb        eax, eax
00481150  inc        eax

// block 00481151
00481151  test       eax, eax
00481153  je         0x481163

// block 00481155
00481155  lea        eax, [ebp - 0x24]
00481158  push       eax
00481159  push       0x49dfa0
0048115e  jmp        0x4811f2

// block 00481163
00481163  cmp        dword ptr [ebp - 4], 0
00481167  mov        eax, ebx
00481169  je         0x481171

// block 0048116b
0048116b  and        eax, edi
0048116d  sub        eax, esi
0048116f  jmp        0x481176

// block 00481171
00481171  and        eax, 0x6000

// block 00481176
00481176  neg        eax
00481178  sbb        eax, eax
0048117a  inc        eax
0048117b  test       eax, eax
0048117d  je         0x4811af

// block 0048117f
0048117f  cmp        dword ptr [ebp - 4], 0
00481183  mov        eax, ebx
00481185  je         0x481194

// block 00481187
00481187  and        al, 0xc0
00481189  xor        edx, edx
0048118b  cmp        al, 0x80
0048118d  sete       dl
00481190  mov        eax, edx
00481192  jmp        0x4811a0

// block 00481194
00481194  and        eax, edi
00481196  sub        eax, 0x1000
0048119b  neg        eax
0048119d  sbb        eax, eax
0048119f  inc        eax

// block 004811a0
004811a0  test       eax, eax
004811a2  je         0x4811af

// block 004811a4
004811a4  lea        eax, [ebp - 0x24]
004811a7  push       eax
004811a8  push       0x49df94
004811ad  jmp        0x4811f2

// block 004811af
004811af  cmp        dword ptr [ebp - 4], 0
004811b3  mov        eax, ebx
004811b5  je         0x4811bd

// block 004811b7
004811b7  and        eax, edi
004811b9  sub        eax, esi
004811bb  jmp        0x4811c2

// block 004811bd
004811bd  and        eax, 0x6000

// block 004811c2
004811c2  neg        eax
004811c4  sbb        eax, eax
004811c6  inc        eax
004811c7  test       eax, eax
004811c9  je         0x481209

// block 004811cb
004811cb  cmp        dword ptr [ebp - 4], 0
004811cf  je         0x4811dc

// block 004811d1
004811d1  push       0
004811d3  test       bl, 0xc0
004811d6  pop        eax
004811d7  sete       al
004811da  jmp        0x4811e5

// block 004811dc
004811dc  mov        eax, ebx
004811de  and        eax, edi
004811e0  neg        eax
004811e2  sbb        eax, eax
004811e4  inc        eax

// block 004811e5
004811e5  test       eax, eax
004811e7  je         0x481209

// block 004811e9
004811e9  lea        eax, [ebp - 0x24]
004811ec  push       eax
004811ed  push       0x49df88

// block 004811f2
004811f2  lea        eax, [ebp - 0x6c]
004811f5  push       eax
004811f6  call       0x47ec4a

// block 004811fb
004811fb  mov        ecx, dword ptr [eax]
004811fd  mov        dword ptr [ebp - 0x24], ecx
00481200  mov        ecx, dword ptr [eax + 4]
00481203  add        esp, 0xc
00481206  mov        dword ptr [ebp - 0x20], ecx

// block 00481209
00481209  cmp        dword ptr [ebp - 4], 0
0048120d  mov        eax, ebx
0048120f  je         0x481217

// block 00481211
00481211  and        eax, edi
00481213  sub        eax, esi
00481215  jmp        0x48121c

// block 00481217
00481217  and        eax, 0x6000

// block 0048121c
0048121c  neg        eax
0048121e  sbb        eax, eax
00481220  inc        eax
00481221  test       eax, eax
00481223  mov        eax, ebx
00481225  je         0x48122e

// block 00481227
00481227  and        eax, 0x400
0048122c  jmp        0x481233

// block 0048122e
0048122e  and        eax, 0x1000

// block 00481233
00481233  test       eax, eax
00481235  je         0x481263

// block 00481237
00481237  test       dword ptr [0x4b4328], 0x1000
00481241  jne        0x481263

// block 00481243
00481243  lea        eax, [ebp - 0x24]
00481246  push       eax
00481247  lea        eax, [ebp - 0x6c]
0048124a  push       0x49df7c
0048124f  push       eax
00481250  call       0x47ec4a

// block 00481255
00481255  mov        ecx, dword ptr [eax]
00481257  mov        dword ptr [ebp - 0x24], ecx
0048125a  mov        ecx, dword ptr [eax + 4]
0048125d  add        esp, 0xc
00481260  mov        dword ptr [ebp - 0x20], ecx

// block 00481263
00481263  test       ebx, 0x10000
00481269  je         0x481288

// block 0048126b
0048126b  lea        eax, [ebp - 0x24]
0048126e  push       eax
0048126f  lea        eax, [ebp - 0x6c]
00481272  push       0x49df70
00481277  push       eax
00481278  call       0x47ec4a

// block 0048127d
0048127d  mov        ecx, dword ptr [eax]
0048127f  mov        dword ptr [ebp - 0x24], ecx
00481282  mov        ecx, dword ptr [eax + 4]
00481285  add        esp, 0xc

// block 00481288
00481288  mov        eax, dword ptr [ebp + 8]
0048128b  mov        edx, dword ptr [ebp - 0x24]
0048128e  mov        dword ptr [eax], edx

// block 00481290
00481290  mov        dword ptr [eax + 4], ecx

// block 00481293
00481293  pop        edi
00481294  pop        esi
00481295  pop        ebx
00481296  leave      
00481297  ret        
