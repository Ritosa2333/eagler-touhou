
// block 004364f0
004364f0  mov        eax, dword ptr [0x4d49d0]
004364f5  sub        esp, 0x20
004364f8  push       ebx
004364f9  mov        ecx, eax
004364fb  push       ebp
004364fc  and        ecx, 0x50
004364ff  xor        ebx, ebx
00436501  push       esi
00436502  lea        esi, [ebx + 4]
00436505  cmp        cl, 0x50
00436508  jne        0x436519

// block 0043650a
0043650a  mov        dword ptr [edi + 0xa1c], 5
00436514  jmp        0x4365a3

// block 00436519
00436519  mov        edx, eax
0043651b  and        edx, 0x60
0043651e  cmp        dl, 0x60
00436521  jne        0x43652f

// block 00436523
00436523  mov        dword ptr [edi + 0xa1c], 7
0043652d  jmp        0x4365a3

// block 0043652f
0043652f  mov        ecx, eax
00436531  and        ecx, 0x90
00436537  cmp        cl, 0x90
0043653a  jne        0x436548

// block 0043653c
0043653c  mov        dword ptr [edi + 0xa1c], 6
00436546  jmp        0x4365a3

// block 00436548
00436548  mov        edx, eax
0043654a  and        edx, 0xa0
00436550  cmp        dl, 0xa0
00436553  jne        0x436561

// block 00436555
00436555  mov        dword ptr [edi + 0xa1c], 8
0043655f  jmp        0x4365a3

// block 00436561
00436561  test       al, 0x20
00436563  je         0x436571

// block 00436565
00436565  mov        dword ptr [edi + 0xa1c], 2
0043656f  jmp        0x4365a3

// block 00436571
00436571  test       al, 0x10
00436573  je         0x436581

// block 00436575
00436575  mov        dword ptr [edi + 0xa1c], 1
0043657f  jmp        0x4365a3

// block 00436581
00436581  test       al, 0x40
00436583  je         0x436591

// block 00436585
00436585  mov        dword ptr [edi + 0xa1c], 3
0043658f  jmp        0x4365a3

// block 00436591
00436591  test       al, al
00436593  jns        0x43659d

// block 00436595
00436595  mov        dword ptr [edi + 0xa1c], esi
0043659b  jmp        0x4365a3

// block 0043659d
0043659d  mov        dword ptr [edi + 0xa1c], ebx

// block 004365a3
004365a3  mov        eax, dword ptr [0x4b43dc]
004365a8  cmp        eax, ebx
004365aa  je         0x4365cc

// block 004365ac
004365ac  cmp        dword ptr [eax + 0x70], ebx
004365af  je         0x4365cc

// block 004365b1
004365b1  cmp        dword ptr [edi + 0xa48], esi
004365b7  jl         0x4365cc

// block 004365b9
004365b9  mov        eax, dword ptr [0x4d49d0]
004365be  shr        eax, 3
004365c1  and        eax, 1
004365c4  mov        dword ptr [edi + 0xc598], eax
004365ca  jmp        0x4365dc

// block 004365cc
004365cc  mov        dword ptr [edi + 0xc598], ebx
004365d2  mov        dword ptr [edi + 0xc3fc], 0x1e

// block 004365dc
004365dc  mov        eax, dword ptr [edi + 0xa1c]
004365e2  add        eax, eax
004365e4  add        eax, eax
004365e6  mov        esi, dword ptr [eax + eax + 0x4a1148]
004365ed  mov        ebp, dword ptr [eax + eax + 0x4a114c]
004365f4  add        eax, eax
004365f6  cmp        dword ptr [edi + 0xc598], ebx
004365fc  je         0x436666

// block 004365fe
004365fe  cmp        dword ptr [edi + 0x8258], ebx
00436604  jne        0x436631

// block 00436606
00436606  mov        edx, dword ptr [0x4b43c8]
0043660c  mov        eax, dword ptr [edx + 0x4debdc]
00436612  push       ebx
00436613  push       0x4d
00436615  lea        ecx, [esp + 0x24]
00436619  push       ecx
0043661a  push       eax
0043661b  mov        eax, 0xc
00436620  xor        ecx, ecx
00436622  call       0x4615a0

// block 00436627
00436627  mov        ecx, dword ptr [esp + 0x1c]
0043662b  mov        dword ptr [edi + 0x8258], ecx

// block 00436631
00436631  mov        ecx, dword ptr [edi + 0xa1c]
00436637  cmp        ecx, 5
0043663a  jl         0x436644

// block 0043663c
0043663c  mov        eax, dword ptr [edi + 0x99c]
00436642  jmp        0x43664a

// block 00436644
00436644  mov        eax, dword ptr [edi + 0x994]

// block 0043664a
0043664a  imul       eax, esi
0043664d  cmp        ecx, 5
00436650  mov        dword ptr [esp + 0x10], eax
00436654  jl         0x43665e

// block 00436656
00436656  mov        eax, dword ptr [edi + 0x99c]
0043665c  jmp        0x4366d2

// block 0043665e
0043665e  mov        eax, dword ptr [edi + 0x994]
00436664  jmp        0x4366d2

// block 00436666
00436666  mov        edx, dword ptr [edi + 0x8258]
0043666c  push       edx
0043666d  mov        edx, dword ptr [0x4ce8cc]
00436673  call       0x461920

// block 00436678
00436678  cmp        eax, ebx
0043667a  jne        0x436684

// block 0043667c
0043667c  mov        dword ptr [edi + 0x8258], ebx
00436682  jmp        0x43669f

// block 00436684
00436684  mov        eax, dword ptr [edi + 0x8258]
0043668a  push       eax
0043668b  mov        ebx, 1
00436690  call       0x461970

// block 00436695
00436695  mov        dword ptr [edi + 0x8258], 0

// block 0043669f
0043669f  mov        ecx, dword ptr [edi + 0xa1c]
004366a5  cmp        ecx, 5
004366a8  jl         0x4366b2

// block 004366aa
004366aa  mov        eax, dword ptr [edi + 0x998]
004366b0  jmp        0x4366b8

// block 004366b2
004366b2  mov        eax, dword ptr [edi + 0x990]

// block 004366b8
004366b8  imul       eax, esi
004366bb  cmp        ecx, 5
004366be  mov        dword ptr [esp + 0x10], eax
004366c2  jl         0x4366cc

// block 004366c4
004366c4  mov        eax, dword ptr [edi + 0x998]
004366ca  jmp        0x4366d2

// block 004366cc
004366cc  mov        eax, dword ptr [edi + 0x990]

// block 004366d2
004366d2  fild       dword ptr [esp + 0x10]
004366d6  imul       eax, ebp
004366d9  fmul       dword ptr [edi + 0x825c]
004366df  mov        dword ptr [esp + 0x1c], eax
004366e3  call       0x4931e0

// block 004366e8
004366e8  mov        esi, eax
004366ea  fild       dword ptr [esp + 0x1c]
004366ee  mov        dword ptr [esp + 0x10], esi
004366f2  fmul       dword ptr [edi + 0x825c]
004366f8  call       0x4931e0

// block 004366fd
004366fd  mov        ebx, eax
004366ff  mov        dword ptr [esp + 0x1c], ebx
00436703  test       esi, esi
00436705  jge        0x436735

// block 00436707
00436707  cmp        dword ptr [edi + 0xa14], 0
0043670e  jl         0x436733

// block 00436710
00436710  push       1
00436712  lea        ecx, [edi + 0x14]
00436715  push       ecx
00436716  mov        ecx, dword ptr [edi + 0x10]
00436719  call       0x454d10

// block 0043671e
0043671e  test       esi, esi
00436720  jle        0x436752

// block 00436722
00436722  cmp        dword ptr [edi + 0xa14], 0
00436729  jg         0x436750

// block 0043672b
0043672b  push       3
0043672d  lea        eax, [edi + 0x14]
00436730  push       eax
00436731  jmp        0x436763

// block 00436733
00436733  test       esi, esi

// block 00436735
00436735  jne        0x43671e

// block 00436737
00436737  cmp        dword ptr [edi + 0xa14], 0
0043673e  jge        0x43675b

// block 00436740
00436740  mov        ecx, dword ptr [edi + 0x10]
00436743  push       2
00436745  lea        edx, [edi + 0x14]
00436748  push       edx
00436749  call       0x454d10

// block 0043674e
0043674e  jmp        0x436754

// block 00436750
00436750  test       esi, esi

// block 00436752
00436752  jne        0x43676b

// block 00436754
00436754  cmp        dword ptr [edi + 0xa14], 0

// block 0043675b
0043675b  jle        0x43676b

// block 0043675d
0043675d  push       4
0043675f  lea        ecx, [edi + 0x14]
00436762  push       ecx

// block 00436763
00436763  mov        ecx, dword ptr [edi + 0x10]
00436766  call       0x454d10

// block 0043676b
0043676b  cmp        dword ptr [edi + 0xa1c], 0
00436772  fild       dword ptr [esp + 0x10]
00436776  mov        dword ptr [edi + 0xa14], esi
0043677c  mov        dword ptr [edi + 0xa18], ebx
00436782  fmul       dword ptr [0x4b2ed0]
00436788  fstp       dword ptr [edi + 0x9a0]
0043678e  fild       dword ptr [esp + 0x1c]
00436792  fmul       dword ptr [0x4b2ed0]
00436798  fstp       dword ptr [edi + 0x9a4]
0043679e  je         0x4367c4

// block 004367a0
004367a0  mov        edx, dword ptr [edi + 0x9a0]
004367a6  mov        eax, dword ptr [edi + 0x9a4]
004367ac  mov        ecx, dword ptr [edi + 0x9a8]
004367b2  mov        dword ptr [edi + 0x9ac], edx
004367b8  mov        dword ptr [edi + 0x9b0], eax
004367be  mov        dword ptr [edi + 0x9b4], ecx

// block 004367c4
004367c4  fld        dword ptr [edi + 0x9a0]
004367ca  call       0x4931e0

// block 004367cf
004367cf  fld        dword ptr [edi + 0x9a4]
004367d5  mov        esi, eax
004367d7  mov        dword ptr [edi + 0x9b8], esi
004367dd  call       0x4931e0

// block 004367e2
004367e2  add        dword ptr [edi + 0x988], esi
004367e8  mov        ecx, dword ptr [edi + 0x988]
004367ee  add        dword ptr [edi + 0x98c], eax
004367f4  cmp        ecx, 0xffffa400
004367fa  mov        dword ptr [edi + 0x9bc], eax
00436800  mov        eax, dword ptr [edi + 0x98c]
00436806  jge        0x436814

// block 00436808
00436808  mov        dword ptr [edi + 0x988], 0xffffa400
00436812  jmp        0x436826

// block 00436814
00436814  cmp        ecx, 0x5c00
0043681a  jle        0x436826

// block 0043681c
0043681c  mov        dword ptr [edi + 0x988], 0x5c00

// block 00436826
00436826  cmp        eax, 0x1000
0043682b  jge        0x436839

// block 0043682d
0043682d  mov        dword ptr [edi + 0x98c], 0x1000
00436837  jmp        0x43684a

// block 00436839
00436839  cmp        eax, 0xd800
0043683e  jle        0x43684a

// block 00436840
00436840  mov        dword ptr [edi + 0x98c], 0xd800

// block 0043684a
0043684a  fild       dword ptr [edi + 0x988]
00436850  mov        esi, dword ptr [0x4ce8cc]
00436856  fld        qword ptr [0x4a3d40]
0043685c  fmul       st(1), st(0)
0043685e  fxch       st(1)
00436860  fstp       dword ptr [edi + 0x97c]
00436866  fild       dword ptr [edi + 0x98c]
0043686c  fmul       st(1)
0043686e  fstp       dword ptr [edi + 0x980]
00436874  mov        edx, dword ptr [edi + 0x8258]
0043687a  push       edx
0043687b  mov        edx, esi
0043687d  call       0x461920

// block 00436882
00436882  test       eax, eax
00436884  jne        0x43688e

// block 00436886
00436886  mov        dword ptr [edi + 0x8258], eax
0043688c  jmp        0x4368ee

// block 0043688e
0043688e  fld        dword ptr [edi + 0x97c]
00436894  mov        eax, dword ptr [edi + 0x8258]
0043689a  fadd       qword ptr [0x4a3d70]
004368a0  push       eax
004368a1  mov        edx, esi
004368a3  fadd       qword ptr [0x4a3d28]
004368a9  fstp       dword ptr [esp + 0x14]
004368ad  fld        dword ptr [edi + 0x980]
004368b3  fadd       qword ptr [0x4a3d68]
004368b9  fstp       dword ptr [esp + 0x18]
004368bd  fld        dword ptr [edi + 0x984]
004368c3  fstp       dword ptr [esp + 0x1c]
004368c7  call       0x461920

// block 004368cc
004368cc  test       eax, eax
004368ce  je         0x4368ee

// block 004368d0
004368d0  mov        ecx, dword ptr [esp + 0x10]
004368d4  mov        edx, dword ptr [esp + 0x14]
004368d8  mov        dword ptr [eax + 0x430], ecx
004368de  mov        ecx, dword ptr [esp + 0x18]
004368e2  mov        dword ptr [eax + 0x434], edx
004368e8  mov        dword ptr [eax + 0x438], ecx

// block 004368ee
004368ee  test       byte ptr [edi + 0xc414], 8
004368f5  je         0x4368fd

// block 004368f7
004368f7  inc        dword ptr [edi + 0xc418]

// block 004368fd
004368fd  lea        esi, [edi + 0x82c0]
00436903  mov        dword ptr [esp + 0x1c], 8

// block 0043690b
0043690b  xor        ebp, ebp
0043690d  cmp        dword ptr [esi - 0x60], ebp
00436910  lea        ecx, [esi - 0x60]
00436913  je         0x436ae8

// block 00436919
00436919  test       byte ptr [edi + 0xc414], 8
00436920  jne        0x4369c0

// block 00436926
00436926  lea        ebx, [esi + 0xc]
00436929  cmp        dword ptr [edi + 0xc598], ebp
0043692f  jne        0x436934

// block 00436931
00436931  lea        ebx, [esi + 4]

// block 00436934
00436934  mov        eax, dword ptr [edi + 0x988]
0043693a  mov        edx, dword ptr [edi + 0x98c]
00436940  add        eax, dword ptr [ebx]
00436942  add        edx, dword ptr [ebx + 4]
00436945  mov        dword ptr [esi - 0xc], eax
00436948  mov        dword ptr [esi - 8], edx
0043694b  mov        eax, dword ptr [esi + 0x7c]
0043694e  cmp        eax, ebp
00436950  je         0x43695c

// block 00436952
00436952  fstp       st(0)
00436954  call       eax

// block 00436956
00436956  fld        qword ptr [0x4a3d40]

// block 0043695c
0043695c  cmp        dword ptr [esi + 0x74], ebp
0043695f  jne        0x436a11

// block 00436965
00436965  mov        edx, dword ptr [edi + 0xc3fc]
0043696b  cmp        edx, 0x1e
0043696e  jl         0x436a1f

// block 00436974
00436974  mov        ebx, dword ptr [esi - 0xc]
00436977  mov        ecx, dword ptr [esi - 8]
0043697a  sub        ecx, dword ptr [esi]
0043697c  mov        eax, ebx
0043697e  sub        eax, dword ptr [esi - 4]
00436981  imul       ecx, edx
00436984  imul       eax, edx
00436987  mov        ebp, eax
00436989  mov        eax, 0x51eb851f
0043698e  imul       ebp
00436990  sar        edx, 5
00436993  mov        dword ptr [esp + 0x14], ecx
00436997  mov        ecx, edx
00436999  shr        ecx, 0x1f
0043699c  add        ecx, edx
0043699e  mov        eax, 0x51eb851f
004369a3  imul       dword ptr [esp + 0x14]
004369a7  sar        edx, 5
004369aa  mov        eax, edx
004369ac  shr        eax, 0x1f
004369af  add        eax, edx
004369b1  test       ecx, ecx
004369b3  jne        0x4369b9

// block 004369b5
004369b5  test       eax, eax
004369b7  je         0x436a07

// block 004369b9
004369b9  add        dword ptr [esi - 4], ecx
004369bc  add        dword ptr [esi], eax
004369be  jmp        0x436a1f

// block 004369c0
004369c0  mov        edx, dword ptr [edi + 0x988]
004369c6  mov        dword ptr [esi - 0xc], edx
004369c9  mov        eax, dword ptr [edi + 0x98c]
004369cf  mov        dword ptr [esi - 8], eax
004369d2  cmp        dword ptr [edi + 0xc418], 0x1e
004369d9  jl         0x43695c

// block 004369db
004369db  mov        dword ptr [ecx], ebp
004369dd  fstp       st(0)
004369df  mov        ecx, dword ptr [esi + 0x50]
004369e2  push       ecx
004369e3  mov        ebx, 1
004369e8  call       0x461970

// block 004369ed
004369ed  mov        edx, dword ptr [esi + 0x54]
004369f0  push       edx
004369f1  call       0x461970

// block 004369f6
004369f6  fld        qword ptr [0x4a3d40]
004369fc  mov        dword ptr [edi + 0xc41c], ebp
00436a02  jmp        0x436ae8

// block 00436a07
00436a07  mov        dword ptr [esi - 4], ebx
00436a0a  mov        eax, dword ptr [esi - 8]
00436a0d  mov        dword ptr [esi], eax
00436a0f  jmp        0x436a1f

// block 00436a11
00436a11  mov        dword ptr [esi + 0x74], ebp
00436a14  mov        ecx, dword ptr [esi - 0xc]
00436a17  mov        dword ptr [esi - 4], ecx
00436a1a  mov        edx, dword ptr [esi - 8]
00436a1d  mov        dword ptr [esi], edx

// block 00436a1f
00436a1f  fild       dword ptr [esi - 4]
00436a22  mov        ecx, dword ptr [esi + 0x50]
00436a25  fmul       st(1)
00436a27  fstp       dword ptr [esp + 0x20]
00436a2b  fild       dword ptr [esi]
00436a2d  fmul       st(1)
00436a2f  fstp       dword ptr [esp + 0x24]
00436a33  fld        dword ptr [esp + 0x24]
00436a37  fld        dword ptr [esp + 0x20]
00436a3b  test       ecx, ecx
00436a3d  je         0x436aa9

// block 00436a3f
00436a3f  mov        edx, dword ptr [0x4ce8cc]
00436a45  mov        eax, dword ptr [edx + 0x8856b8]
00436a4b  test       eax, eax
00436a4d  je         0x436a5c

// block 00436a4f
00436a4f  mov        ebx, dword ptr [eax]
00436a51  cmp        dword ptr [ebx], ecx
00436a53  je         0x436a75

// block 00436a55
00436a55  mov        eax, dword ptr [eax + 4]
00436a58  test       eax, eax
00436a5a  jne        0x436a4f

// block 00436a5c
00436a5c  mov        eax, dword ptr [edx + 0x8856c0]
00436a62  test       eax, eax
00436a64  je         0x436aa9

// block 00436a66
00436a66  mov        edx, dword ptr [eax]
00436a68  cmp        dword ptr [edx], ecx
00436a6a  je         0x436a79

// block 00436a6c
00436a6c  mov        eax, dword ptr [eax + 4]
00436a6f  test       eax, eax
00436a71  jne        0x436a66

// block 00436a73
00436a73  jmp        0x436aa9

// block 00436a75
00436a75  mov        eax, ebx
00436a77  jmp        0x436a7b

// block 00436a79
00436a79  mov        eax, edx

// block 00436a7b
00436a7b  test       eax, eax
00436a7d  je         0x436aa9

// block 00436a7f
00436a7f  fld        qword ptr [0x4a3d70]
00436a85  fadd       st(1)
00436a87  fadd       qword ptr [0x4a3d28]
00436a8d  fstp       dword ptr [eax + 0x430]
00436a93  fld        st(1)
00436a95  fadd       qword ptr [0x4a3d68]
00436a9b  fstp       dword ptr [eax + 0x434]
00436aa1  fldz       
00436aa3  fstp       dword ptr [eax + 0x438]

// block 00436aa9
00436aa9  mov        ecx, dword ptr [esi + 0x54]
00436aac  test       ecx, ecx
00436aae  je         0x436ae4

// block 00436ab0
00436ab0  mov        edx, dword ptr [0x4ce8cc]
00436ab6  mov        eax, dword ptr [edx + 0x8856b8]
00436abc  test       eax, eax
00436abe  je         0x436acd

// block 00436ac0
00436ac0  mov        ebx, dword ptr [eax]
00436ac2  cmp        dword ptr [ebx], ecx
00436ac4  je         0x436b04

// block 00436ac6
00436ac6  mov        eax, dword ptr [eax + 4]
00436ac9  test       eax, eax
00436acb  jne        0x436ac0

// block 00436acd
00436acd  mov        eax, dword ptr [edx + 0x8856c0]
00436ad3  test       eax, eax
00436ad5  je         0x436ae4

// block 00436ad7
00436ad7  mov        edx, dword ptr [eax]
00436ad9  cmp        dword ptr [edx], ecx
00436adb  je         0x436b08

// block 00436add
00436add  mov        eax, dword ptr [eax + 4]
00436ae0  test       eax, eax
00436ae2  jne        0x436ad7

// block 00436ae4
00436ae4  fstp       st(1)
00436ae6  fstp       st(0)

// block 00436ae8
00436ae8  add        esi, 0xe4
00436aee  sub        dword ptr [esp + 0x1c], 1
00436af3  jne        0x43690b

// block 00436af9
00436af9  pop        esi
00436afa  fstp       st(0)
00436afc  pop        ebp
00436afd  xor        eax, eax
00436aff  pop        ebx
00436b00  add        esp, 0x20
00436b03  ret        

// block 00436b04
00436b04  mov        eax, ebx
00436b06  jmp        0x436b0a

// block 00436b08
00436b08  mov        eax, edx

// block 00436b0a
00436b0a  test       eax, eax
00436b0c  je         0x436ae4

// block 00436b0e
00436b0e  fadd       qword ptr [0x4a3d70]
00436b14  fadd       qword ptr [0x4a3d28]
00436b1a  fstp       dword ptr [eax + 0x430]
00436b20  fadd       qword ptr [0x4a3d68]
00436b26  fstp       dword ptr [eax + 0x434]
00436b2c  fldz       
00436b2e  fstp       dword ptr [eax + 0x438]
00436b34  jmp        0x436ae8
