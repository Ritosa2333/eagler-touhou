
// block 0040aa60
0040aa60  push       ebp
0040aa61  mov        ebp, esp
0040aa63  and        esp, 0xfffffff8
0040aa66  sub        esp, 0x220
0040aa6c  push       ebx
0040aa6d  push       ebp
0040aa6e  push       esi
0040aa6f  push       edi
0040aa70  mov        ebp, ecx
0040aa72  cmp        dword ptr [ebp + 0x548], 0x12
0040aa79  jge        0x40b546

// block 0040aa7f
0040aa7f  nop        

// block 0040aa80
0040aa80  mov        esi, dword ptr [ebp + 0x548]
0040aa86  fld        qword ptr [0x4a3e70]
0040aa8c  fld        qword ptr [0x4a4268]
0040aa92  lea        eax, [esi + esi*2]
0040aa95  mov        edx, dword ptr [ebp + eax*8 + 0x560]
0040aa9c  lea        edi, [ebp + eax*8 + 0x550]
0040aaa3  xor        ebx, ebx
0040aaa5  cmp        edx, ebx
0040aaa7  je         0x40b542

// block 0040aaad
0040aaad  cmp        dword ptr [edi + 0x14], ebx
0040aab0  jne        0x40aabe

// block 0040aab2
0040aab2  cmp        dword ptr [ebp + 0x528], ebx
0040aab8  jne        0x40b542

// block 0040aabe
0040aabe  mov        eax, dword ptr [ebp + 0x528]
0040aac4  test       edx, eax
0040aac6  jne        0x40b542

// block 0040aacc
0040aacc  cmp        edx, 0x80000000
0040aad2  jne        0x40aae4

// block 0040aad4
0040aad4  fstp       st(1)
0040aad6  inc        esi
0040aad7  fstp       st(0)
0040aad9  mov        dword ptr [ebp + 0x548], esi
0040aadf  jmp        0x40b52d

// block 0040aae4
0040aae4  mov        ecx, edx
0040aae6  cmp        ecx, 0x4000
0040aaec  ja         0x40aed1

// block 0040aaf2
0040aaf2  je         0x40aeb6

// block 0040aaf8
0040aaf8  cmp        ecx, 0x100
0040aafe  ja         0x40ad16

// block 0040ab04
0040ab04  je         0x40ace5

// block 0040ab0a
0040ab0a  dec        ecx
0040ab0b  cmp        ecx, 0x3f
0040ab0e  ja         0x40b4f7

// block 0040ab14
0040ab14  movzx      ecx, byte ptr [ecx + 0x40b564]
0040ab1b  jmp        dword ptr [ecx*4 + 0x40b550]

// block 0040ab22
0040ab22  or         eax, 1
0040ab25  fstp       st(1)
0040ab27  mov        dword ptr [ebp + 0x528], eax
0040ab2d  fstp       st(0)
0040ab2f  push       ebx
0040ab30  lea        eax, [ebp + 0x700]
0040ab36  call       0x4067e0

// block 0040ab3b
0040ab3b  fldz       
0040ab3d  fstp       dword ptr [ebp + 0x724]
0040ab43  jmp        0x40b527

// block 0040ab48
0040ab48  or         eax, 4
0040ab4b  mov        dword ptr [ebp + 0x528], eax
0040ab51  fld        dword ptr [edi]
0040ab53  fstp       dword ptr [ebp + 0x748]
0040ab59  fld        dword ptr [edi + 4]
0040ab5c  fcomp      st(2)
0040ab5e  fnstsw     ax
0040ab60  fstp       st(1)
0040ab62  test       ah, 0x41
0040ab65  jp         0x40ab71

// block 0040ab67
0040ab67  fstp       st(0)
0040ab69  fld        dword ptr [ebp + 0x4d8]
0040ab6f  jmp        0x40ab8d

// block 0040ab71
0040ab71  fld        dword ptr [edi + 4]
0040ab74  fcompp     
0040ab76  fnstsw     ax
0040ab78  test       ah, 1
0040ab7b  jne        0x40ab8a

// block 0040ab7d
0040ab7d  lea        ecx, [ebp + 0x4bc]
0040ab83  call       0x4377a0

// block 0040ab88
0040ab88  jmp        0x40ab8d

// block 0040ab8a
0040ab8a  fld        dword ptr [edi + 4]

// block 0040ab8d
0040ab8d  fstp       dword ptr [esp + 0x14]
0040ab91  push       ebx
0040ab92  fld        dword ptr [esp + 0x18]
0040ab96  lea        eax, [ebp + 0x734]
0040ab9c  fstp       dword ptr [ebp + 0x74c]
0040aba2  call       0x4067e0

// block 0040aba7
0040aba7  fld        dword ptr [ebp + 0x748]
0040abad  mov        edx, dword ptr [edi + 8]
0040abb0  sub        esp, 8
0040abb3  fstp       dword ptr [esp + 4]
0040abb7  mov        dword ptr [ebp + 0x75c], edx
0040abbd  fld        dword ptr [ebp + 0x74c]
0040abc3  lea        ecx, [ebp + 0x750]

// block 0040abc9
0040abc9  fstp       dword ptr [esp]
0040abcc  call       0x40d640

// block 0040abd1
0040abd1  cmp        dword ptr [ebp + 0x548], ebx
0040abd7  je         0x40b527

// block 0040abdd
0040abdd  mov        edx, dword ptr [ebp + 0x544]
0040abe3  cmp        edx, ebx
0040abe5  jl         0x40b527

// block 0040abeb
0040abeb  call       0x453d90

// block 0040abf0
0040abf0  jmp        0x40b527

// block 0040abf5
0040abf5  fstp       st(1)
0040abf7  or         eax, 8
0040abfa  fstp       st(0)
0040abfc  mov        dword ptr [ebp + 0x528], eax
0040ac02  fld        dword ptr [edi]
0040ac04  push       ebx
0040ac05  fstp       dword ptr [ebp + 0x77c]
0040ac0b  lea        eax, [ebp + 0x768]
0040ac11  fld        dword ptr [edi + 4]
0040ac14  fstp       dword ptr [ebp + 0x780]
0040ac1a  call       0x4067e0

// block 0040ac1f
0040ac1f  mov        eax, dword ptr [edi + 8]
0040ac22  mov        dword ptr [ebp + 0x790], eax
0040ac28  cmp        dword ptr [ebp + 0x548], ebx
0040ac2e  je         0x40b527

// block 0040ac34
0040ac34  mov        edx, dword ptr [ebp + 0x544]
0040ac3a  cmp        edx, ebx
0040ac3c  jl         0x40b527

// block 0040ac42
0040ac42  call       0x453d90

// block 0040ac47
0040ac47  jmp        0x40b527

// block 0040ac4c
0040ac4c  or         eax, edx
0040ac4e  mov        dword ptr [ebp + 0x528], eax
0040ac54  fld        dword ptr [edi]
0040ac56  fcomp      st(2)
0040ac58  fnstsw     ax
0040ac5a  fstp       st(1)
0040ac5c  test       ah, 0x41
0040ac5f  jp         0x40ac6b

// block 0040ac61
0040ac61  fstp       st(0)
0040ac63  fld        dword ptr [ebp + 0x4d8]
0040ac69  jmp        0x40ac85

// block 0040ac6b
0040ac6b  fld        dword ptr [edi]
0040ac6d  fcompp     
0040ac6f  fnstsw     ax
0040ac71  test       ah, 1
0040ac74  jne        0x40ac83

// block 0040ac76
0040ac76  lea        ecx, [ebp + 0x4bc]
0040ac7c  call       0x4377a0

// block 0040ac81
0040ac81  jmp        0x40ac85

// block 0040ac83
0040ac83  fld        dword ptr [edi]

// block 0040ac85
0040ac85  fstp       dword ptr [esp + 0x14]
0040ac89  fld        dword ptr [esp + 0x14]
0040ac8d  fstp       dword ptr [ebp + 0x7b4]
0040ac93  fld        dword ptr [edi + 4]
0040ac96  fcomp      qword ptr [0x4a4150]
0040ac9c  fnstsw     ax
0040ac9e  test       ah, 0x41
0040aca1  jne        0x40aca8

// block 0040aca3
0040aca3  fld        dword ptr [edi + 4]
0040aca6  jmp        0x40acae

// block 0040aca8
0040aca8  fld        dword ptr [ebp + 0x4d4]

// block 0040acae
0040acae  fstp       dword ptr [esp + 0x14]
0040acb2  push       ebx
0040acb3  fld        dword ptr [esp + 0x18]
0040acb7  lea        eax, [ebp + 0x79c]
0040acbd  fstp       dword ptr [ebp + 0x7b0]
0040acc3  call       0x4067e0

// block 0040acc8
0040acc8  mov        ecx, dword ptr [edi + 8]
0040accb  mov        dword ptr [ebp + 0x7c4], ecx
0040acd1  mov        edx, dword ptr [edi + 0xc]
0040acd4  mov        dword ptr [ebp + 0x7c8], edx
0040acda  mov        dword ptr [ebp + 0x7cc], ebx
0040ace0  jmp        0x40b527

// block 0040ace5
0040ace5  fstp       st(1)
0040ace7  or         eax, edx
0040ace9  mov        dword ptr [ebp + 0x528], eax
0040acef  fstp       st(0)
0040acf1  fld        dword ptr [edi]
0040acf3  fstp       dword ptr [ebp + 0x7e4]
0040acf9  mov        eax, dword ptr [edi + 8]
0040acfc  mov        dword ptr [ebp + 0x7fc], eax
0040ad02  mov        dword ptr [ebp + 0x7f8], ebx
0040ad08  mov        ecx, dword ptr [edi + 0xc]
0040ad0b  mov        dword ptr [ebp + 0x800], ecx
0040ad11  jmp        0x40b527

// block 0040ad16
0040ad16  fstp       st(1)
0040ad18  fstp       st(0)
0040ad1a  cmp        ecx, 0x800
0040ad20  ja         0x40ae7a

// block 0040ad26
0040ad26  je         0x40ad6c

// block 0040ad28
0040ad28  cmp        ecx, 0x200
0040ad2e  je         0x40ad61

// block 0040ad30
0040ad30  cmp        ecx, 0x400
0040ad36  jne        0x40b527

// block 0040ad3c
0040ad3c  or         eax, edx
0040ad3e  mov        dword ptr [ebp + 0x528], eax
0040ad44  mov        edx, dword ptr [edi + 8]
0040ad47  push       edx
0040ad48  lea        eax, [ebp + 0x970]
0040ad4e  call       0x4067e0

// block 0040ad53
0040ad53  mov        eax, dword ptr [edi + 0xc]
0040ad56  mov        dword ptr [ebp + 0x998], eax
0040ad5c  jmp        0x40b527

// block 0040ad61
0040ad61  mov        ecx, dword ptr [edi + 8]
0040ad64  mov        dword ptr [ebp + 4], ecx
0040ad67  jmp        0x40b527

// block 0040ad6c
0040ad6c  mov        dx, word ptr [edi + 8]
0040ad70  mov        word ptr [ebp + 0x9f4], dx
0040ad77  mov        ax, word ptr [edi + 0xc]
0040ad7b  mov        word ptr [ebp + 0x9f6], ax
0040ad82  mov        ecx, dword ptr [edi + 8]
0040ad85  imul       ecx, ecx, 0xd0
0040ad8b  lea        esi, [ebp + 8]
0040ad8e  fld        dword ptr [ecx + 0x4af344]
0040ad94  fstp       dword ptr [esp + 0x14]
0040ad98  fld        dword ptr [esp + 0x14]
0040ad9c  fst        dword ptr [ebp + 0x4e0]
0040ada2  fstp       dword ptr [ebp + 0x4dc]
0040ada8  call       0x402520

// block 0040adad
0040adad  mov        ecx, dword ptr [0x4b43c8]
0040adb3  mov        dword ptr [ebp + 0x4b4], 0x40d430
0040adbd  mov        dword ptr [ebp + 0x4b8], ebp
0040adc3  mov        edx, dword ptr [edi + 8]
0040adc6  mov        ecx, dword ptr [ecx + 0x4debdc]
0040adcc  imul       edx, edx, 0xd0
0040add2  mov        edx, dword ptr [edx + 0x4af280]
0040add8  mov        eax, esi
0040adda  call       0x454ee0

// block 0040addf
0040addf  movsx      edx, word ptr [ebp + 0x9f4]
0040ade6  and        dword ptr [ebp], 0xffffffef
0040adea  imul       edx, edx, 0xd0
0040adf0  mov        ecx, dword ptr [edx + 0x4af34c]
0040adf6  mov        eax, dword ptr [ebp]
0040adf9  cmp        ecx, 5
0040adfc  ja         0x40b527

// block 0040ae02
0040ae02  jmp        dword ptr [ecx*4 + 0x40b5a4]

// block 0040ae09
0040ae09  movsx      eax, word ptr [ebp + 0x9f6]
0040ae10  lea        ecx, [eax + eax + 4]
0040ae14  mov        dword ptr [ebp + 0x524], ecx
0040ae1a  jmp        0x40b527

// block 0040ae1f
0040ae1f  movsx      edx, word ptr [ebp + 0x9f6]
0040ae26  mov        eax, dword ptr [edx*4 + 0x4b0bb0]
0040ae2d  mov        dword ptr [ebp + 0x524], eax
0040ae33  jmp        0x40b527

// block 0040ae38
0040ae38  or         eax, 0x10
0040ae3b  mov        dword ptr [ebp + 0x524], 0xffffffff
0040ae45  mov        dword ptr [ebp], eax
0040ae48  jmp        0x40b527

// block 0040ae4d
0040ae4d  mov        dword ptr [ebp + 0x524], 0x10
0040ae57  jmp        0x40b527

// block 0040ae5c
0040ae5c  mov        dword ptr [ebp + 0x524], 6
0040ae66  jmp        0x40b527

// block 0040ae6b
0040ae6b  mov        dword ptr [ebp + 0x524], 0xc
0040ae75  jmp        0x40b527

// block 0040ae7a
0040ae7a  cmp        ecx, 0x1000
0040ae80  je         0x40ae9a

// block 0040ae82
0040ae82  cmp        ecx, 0x2000
0040ae88  jne        0x40b527

// block 0040ae8e
0040ae8e  mov        esi, ebp
0040ae90  call       0x40c8b0

// block 0040ae95
0040ae95  jmp        0x40b527

// block 0040ae9a
0040ae9a  or         eax, edx
0040ae9c  mov        dword ptr [ebp + 0x528], eax
0040aea2  mov        ecx, dword ptr [edi + 8]
0040aea5  push       ecx
0040aea6  lea        eax, [ebp + 0x804]
0040aeac  call       0x4067e0

// block 0040aeb1
0040aeb1  jmp        0x40b527

// block 0040aeb6
0040aeb6  mov        eax, dword ptr [edi + 8]
0040aeb9  fstp       st(1)
0040aebb  fstp       st(0)
0040aebd  push       ecx
0040aebe  fld        dword ptr [ebp + 0x4bc]
0040aec4  fstp       dword ptr [esp]
0040aec7  call       0x453e20

// block 0040aecc
0040aecc  jmp        0x40b527

// block 0040aed1
0040aed1  cmp        ecx, 0x1000000
0040aed7  ja         0x40b1b4

// block 0040aedd
0040aedd  fstp       st(1)
0040aedf  fstp       st(0)
0040aee1  je         0x40b09a

// block 0040aee7
0040aee7  cmp        ecx, 0x200000
0040aeed  ja         0x40b045

// block 0040aef3
0040aef3  je         0x40b030

// block 0040aef9
0040aef9  cmp        ecx, 0x20000
0040aeff  je         0x40b014

// block 0040af05
0040af05  cmp        ecx, 0x40000
0040af0b  je         0x40aff8

// block 0040af11
0040af11  cmp        ecx, 0x80000
0040af17  jne        0x40b527

// block 0040af1d
0040af1d  lea        ecx, [esp + 0x18]
0040af21  call       0x409400

// block 0040af26
0040af26  fld        dword ptr [edi]
0040af28  mov        eax, dword ptr [ebp + 0x4c0]
0040af2e  fstp       dword ptr [esp + 0x30]
0040af32  mov        edx, dword ptr [ebp + 0x4bc]
0040af38  fld        dword ptr [edi + 4]
0040af3b  mov        ecx, dword ptr [ebp + 0x4c4]
0040af41  fstp       dword ptr [esp + 0x34]
0040af45  mov        dword ptr [esp + 0x1c], edx
0040af49  mov        dword ptr [esp + 0x20], eax
0040af4d  mov        eax, dword ptr [edi + 8]
0040af50  mov        edx, eax
0040af52  shr        edx, 0x18
0040af55  mov        dword ptr [esp + 0x24], ecx
0040af59  movzx      ecx, byte ptr [edi + 0xa]
0040af5d  mov        ebx, eax
0040af5f  and        edx, 0x7f
0040af62  and        eax, 0xff
0040af67  mov        word ptr [esp + 0x214], dx
0040af6f  movsx      dx, byte ptr [edi + 9]
0040af74  add        edi, 0x18
0040af77  mov        dword ptr [esp + 0x224], eax
0040af7e  mov        ax, word ptr [edi - 0xc]
0040af82  inc        esi
0040af83  mov        dword ptr [ebp + 0x548], esi
0040af89  mov        word ptr [esp + 0x18], cx
0040af8e  movzx      ecx, word ptr [edi + 8]
0040af92  fld        dword ptr [edi]
0040af94  fstp       dword ptr [esp + 0x28]
0040af98  fld        dword ptr [edi + 4]
0040af9b  mov        word ptr [esp + 0x1a], dx
0040afa0  fstp       dword ptr [esp + 0x2c]
0040afa4  mov        edx, dword ptr [edi + 0xc]
0040afa7  mov        word ptr [esp + 0x212], cx
0040afaf  mov        word ptr [esp + 0x210], ax
0040afb7  mov        dword ptr [esp + 0x218], edx
0040afbe  lea        esi, [ebp + 0x550]
0040afc4  mov        ecx, 0x6c
0040afc9  lea        edi, [esp + 0x3c]

// block 0040afcd
0040afcd  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0040afcf
0040afcf  lea        esi, [esp + 0x18]
0040afd3  and        ebx, 0x80000000
0040afd9  call       0x40b5e0

// block 0040afde
0040afde  inc        dword ptr [ebp + 0x548]
0040afe4  test       ebx, ebx
0040afe6  je         0x40b52d

// block 0040afec
0040afec  mov        esi, ebp
0040afee  call       0x40c8b0

// block 0040aff3
0040aff3  jmp        0x40b527

// block 0040aff8
0040aff8  or         eax, edx
0040affa  mov        dword ptr [ebp + 0x528], eax
0040b000  mov        eax, dword ptr [edi + 8]
0040b003  push       eax
0040b004  lea        eax, [ebp + 0x86c]
0040b00a  call       0x4067e0

// block 0040b00f
0040b00f  jmp        0x40b527

// block 0040b014
0040b014  or         eax, edx
0040b016  mov        dword ptr [ebp + 0x528], eax
0040b01c  mov        ecx, dword ptr [edi + 8]
0040b01f  push       ecx
0040b020  lea        eax, [ebp + 0x838]
0040b026  call       0x4067e0

// block 0040b02b
0040b02b  jmp        0x40b527

// block 0040b030
0040b030  mov        edx, dword ptr [edi + 8]
0040b033  inc        esi
0040b034  mov        dword ptr [ebp + 0x50c], edx
0040b03a  mov        dword ptr [ebp + 0x548], esi
0040b040  jmp        0x40b52d

// block 0040b045
0040b045  cmp        ecx, 0x400000
0040b04b  je         0x40b08c

// block 0040b04d
0040b04d  cmp        ecx, 0x800000
0040b053  jne        0x40b527

// block 0040b059
0040b059  or         eax, ecx
0040b05b  mov        dword ptr [ebp + 0x528], eax
0040b061  fld        dword ptr [edi]
0040b063  fstp       dword ptr [ebp + 0x8b4]
0040b069  push       ebx
0040b06a  fld        dword ptr [edi + 4]
0040b06d  lea        eax, [ebp + 0x8a0]
0040b073  fstp       dword ptr [ebp + 0x8b8]
0040b079  call       0x4067e0

// block 0040b07e
0040b07e  mov        eax, dword ptr [edi + 8]
0040b081  mov        dword ptr [ebp + 0x8c8], eax
0040b087  jmp        0x40b527

// block 0040b08c
0040b08c  mov        ecx, dword ptr [edi + 8]
0040b08f  mov        dword ptr [ebp + 0x548], ecx
0040b095  jmp        0x40b52d

// block 0040b09a
0040b09a  mov        dx, word ptr [edi + 8]
0040b09e  mov        word ptr [ebp + 0x9f4], dx
0040b0a5  mov        ax, word ptr [edi + 0xc]
0040b0a9  mov        word ptr [ebp + 0x9f6], ax
0040b0b0  mov        ecx, dword ptr [edi + 8]
0040b0b3  imul       ecx, ecx, 0xd0
0040b0b9  lea        esi, [ebp + 8]
0040b0bc  fld        dword ptr [ecx + 0x4af344]
0040b0c2  fstp       dword ptr [esp + 0x14]
0040b0c6  fld        dword ptr [esp + 0x14]
0040b0ca  fst        dword ptr [ebp + 0x4e0]
0040b0d0  fstp       dword ptr [ebp + 0x4dc]
0040b0d6  call       0x402520

// block 0040b0db
0040b0db  mov        ecx, dword ptr [0x4b43c8]
0040b0e1  mov        dword ptr [ebp + 0x4b4], 0x40d430
0040b0eb  mov        dword ptr [ebp + 0x4b8], ebp
0040b0f1  mov        edx, dword ptr [edi + 8]
0040b0f4  mov        ecx, dword ptr [ecx + 0x4debdc]
0040b0fa  imul       edx, edx, 0xd0
0040b100  mov        edx, dword ptr [edx + 0x4af280]
0040b106  mov        eax, esi
0040b108  call       0x454ee0

// block 0040b10d
0040b10d  movsx      edx, word ptr [ebp + 0x9f4]
0040b114  and        dword ptr [ebp], 0xffffffef
0040b118  imul       edx, edx, 0xd0
0040b11e  mov        ecx, dword ptr [edx + 0x4af34c]
0040b124  mov        eax, dword ptr [ebp]
0040b127  cmp        ecx, 5
0040b12a  ja         0x40b190

// block 0040b12c
0040b12c  jmp        dword ptr [ecx*4 + 0x40b5bc]

// block 0040b133
0040b133  movsx      eax, word ptr [ebp + 0x9f6]
0040b13a  lea        ecx, [eax + eax + 4]
0040b13e  mov        dword ptr [ebp + 0x524], ecx
0040b144  jmp        0x40b190

// block 0040b146
0040b146  movsx      edx, word ptr [ebp + 0x9f6]
0040b14d  mov        eax, dword ptr [edx*4 + 0x4b0bb0]
0040b154  mov        dword ptr [ebp + 0x524], eax
0040b15a  jmp        0x40b190

// block 0040b15c
0040b15c  or         eax, 0x10
0040b15f  mov        dword ptr [ebp + 0x524], 0xffffffff
0040b169  mov        dword ptr [ebp], eax
0040b16c  jmp        0x40b190

// block 0040b16e
0040b16e  mov        dword ptr [ebp + 0x524], 0x10
0040b178  jmp        0x40b190

// block 0040b17a
0040b17a  mov        dword ptr [ebp + 0x524], 6
0040b184  jmp        0x40b190

// block 0040b186
0040b186  mov        dword ptr [ebp + 0x524], 0xc

// block 0040b190
0040b190  mov        eax, dword ptr [esi + 0x494]
0040b196  cmp        eax, ebx
0040b198  je         0x40b1a3

// block 0040b19a
0040b19a  mov        edx, 2
0040b19f  mov        ecx, esi
0040b1a1  call       eax

// block 0040b1a3
0040b1a3  mov        ecx, 2
0040b1a8  mov        word ptr [esi + 0x3c4], cx
0040b1af  jmp        0x40b527

// block 0040b1b4
0040b1b4  cmp        ecx, 0x8000000
0040b1ba  ja         0x40b450

// block 0040b1c0
0040b1c0  je         0x40b3b7

// block 0040b1c6
0040b1c6  cmp        ecx, 0x2000000
0040b1cc  je         0x40b269

// block 0040b1d2
0040b1d2  cmp        ecx, 0x4000000
0040b1d8  jne        0x40b4f7

// block 0040b1de
0040b1de  fld        dword ptr [edi]
0040b1e0  fcompp     
0040b1e2  fnstsw     ax
0040b1e4  test       ah, 1
0040b1e7  jne        0x40b217

// block 0040b1e9
0040b1e9  fstp       st(0)
0040b1eb  sub        esp, 8
0040b1ee  fld        dword ptr [edi]
0040b1f0  lea        ecx, [ebp + 0x4bc]
0040b1f6  fsub       qword ptr [0x4a42c8]
0040b1fc  fstp       dword ptr [esp + 0x1c]
0040b200  fld        dword ptr [esp + 0x1c]
0040b204  fstp       dword ptr [esp + 4]
0040b208  call       0x4377a0

// block 0040b20d
0040b20d  fstp       dword ptr [esp]
0040b210  call       0x464640

// block 0040b215
0040b215  jmp        0x40b224

// block 0040b217
0040b217  fld        dword ptr [edi]
0040b219  fcompp     
0040b21b  fnstsw     ax
0040b21d  test       ah, 1
0040b220  jne        0x40b22a

// block 0040b222
0040b222  fld        dword ptr [edi]

// block 0040b224
0040b224  fstp       dword ptr [ebp + 0x4d8]

// block 0040b22a
0040b22a  fld        dword ptr [edi + 4]
0040b22d  fcomp      qword ptr [0x4a3e70]
0040b233  fnstsw     ax
0040b235  test       ah, 1
0040b238  jne        0x40b243

// block 0040b23a
0040b23a  fld        dword ptr [edi + 4]
0040b23d  fstp       dword ptr [ebp + 0x4d4]

// block 0040b243
0040b243  fld        dword ptr [ebp + 0x4d4]
0040b249  sub        esp, 8
0040b24c  fstp       dword ptr [esp + 4]
0040b250  lea        ecx, [ebp + 0x4c8]
0040b256  fld        dword ptr [ebp + 0x4d8]
0040b25c  fstp       dword ptr [esp]
0040b25f  call       0x40d640

// block 0040b264
0040b264  jmp        0x40b527

// block 0040b269
0040b269  fstp       st(1)
0040b26b  or         eax, 0x2000000
0040b270  fstp       st(0)
0040b272  mov        dword ptr [ebp + 0x528], eax
0040b278  fld        dword ptr [edi]
0040b27a  fstp       dword ptr [ebp + 0x8f0]
0040b280  fld        dword ptr [edi + 4]
0040b283  fstp       dword ptr [ebp + 0x8f4]
0040b289  test       dword ptr [edi + 0xc], 0x100
0040b290  je         0x40b2c8

// block 0040b292
0040b292  fld        dword ptr [ebp + 0x8f0]
0040b298  fadd       dword ptr [ebp + 0x4bc]
0040b29e  fstp       dword ptr [ebp + 0x8f0]
0040b2a4  fld        dword ptr [ebp + 0x4c0]
0040b2aa  fadd       dword ptr [ebp + 0x8f4]
0040b2b0  fstp       dword ptr [ebp + 0x8f4]
0040b2b6  fld        dword ptr [ebp + 0x4c4]
0040b2bc  fadd       dword ptr [ebp + 0x8f8]
0040b2c2  fstp       dword ptr [ebp + 0x8f8]

// block 0040b2c8
0040b2c8  fld        dword ptr [ebp + 0x4d4]
0040b2ce  push       ebx
0040b2cf  fstp       dword ptr [ebp + 0x8e8]
0040b2d5  fldz       
0040b2d7  fstp       dword ptr [ebp + 0x8f8]
0040b2dd  mov        edx, dword ptr [edi + 8]
0040b2e0  mov        dword ptr [ebp + 0x8fc], edx
0040b2e6  mov        eax, dword ptr [edi + 0xc]
0040b2e9  and        eax, 0xff
0040b2ee  mov        dword ptr [ebp + 0x900], eax
0040b2f4  lea        eax, [ebp + 0x8d4]
0040b2fa  call       0x4067e0

// block 0040b2ff
0040b2ff  mov        ecx, dword ptr [ebp + 0x4bc]
0040b305  mov        edx, dword ptr [ebp + 0x4c0]
0040b30b  mov        dword ptr [ebp + 0x9a8], ecx
0040b311  mov        ecx, dword ptr [ebp + 0x4c4]
0040b317  mov        dword ptr [ebp + 0x9ac], edx
0040b31d  mov        edx, dword ptr [ebp + 0x8f0]
0040b323  mov        dword ptr [ebp + 0x9b4], edx
0040b329  mov        edx, dword ptr [ebp + 0x8f8]
0040b32f  mov        dword ptr [ebp + 0x9b0], ecx
0040b335  mov        ecx, dword ptr [ebp + 0x8f4]
0040b33b  lea        eax, [ebp + 0x9a8]
0040b341  mov        dword ptr [ebp + 0x9b8], ecx
0040b347  mov        dword ptr [ebp + 0x9bc], edx
0040b34d  mov        ecx, dword ptr [0x4ce8d0]
0040b353  mov        dword ptr [ebp + 0x9c0], ecx
0040b359  mov        edx, dword ptr [0x4ce8d4]
0040b35f  mov        dword ptr [ebp + 0x9c4], edx
0040b365  mov        ecx, dword ptr [0x4ce8d8]
0040b36b  mov        dword ptr [ebp + 0x9c8], ecx
0040b371  mov        edx, dword ptr [0x4ce8d0]
0040b377  mov        dword ptr [ebp + 0x9cc], edx
0040b37d  mov        ecx, dword ptr [0x4ce8d4]
0040b383  mov        dword ptr [ebp + 0x9d0], ecx
0040b389  mov        edx, dword ptr [0x4ce8d8]
0040b38f  mov        dword ptr [ebp + 0x9d4], edx
0040b395  mov        ecx, dword ptr [edi + 8]
0040b398  mov        dword ptr [ebp + 0x9ec], ecx
0040b39e  mov        edx, dword ptr [edi + 0xc]
0040b3a1  and        edx, 0xff
0040b3a7  mov        dword ptr [ebp + 0x9f0], edx
0040b3ad  call       0x406340

// block 0040b3b2
0040b3b2  jmp        0x40b527

// block 0040b3b7
0040b3b7  fstp       st(1)
0040b3b9  or         eax, 0x8000000
0040b3be  fstp       st(0)
0040b3c0  mov        dword ptr [ebp + 0x528], eax
0040b3c6  fld        dword ptr [edi + 4]
0040b3c9  sub        esp, 8
0040b3cc  fstp       dword ptr [esp + 4]
0040b3d0  lea        ecx, [ebp + 0x924]
0040b3d6  fld        dword ptr [edi]
0040b3d8  fstp       dword ptr [esp]
0040b3db  call       0x40d640

// block 0040b3e0
0040b3e0  fldz       
0040b3e2  fst        dword ptr [ebp + 0x92c]
0040b3e8  fld        dword ptr [edi]
0040b3ea  fstp       dword ptr [ebp + 0x920]
0040b3f0  fld        dword ptr [edi + 4]
0040b3f3  fstp       dword ptr [ebp + 0x91c]
0040b3f9  mov        eax, dword ptr [edi + 8]
0040b3fc  mov        dword ptr [ebp + 0x930], eax
0040b402  mov        eax, dword ptr [ebp + 0x918]
0040b408  test       al, 1
0040b40a  jne        0x40b435

// block 0040b40c
0040b40c  or         eax, 1
0040b40f  fst        dword ptr [ebp + 0x910]
0040b415  mov        dword ptr [ebp + 0x90c], ebx
0040b41b  mov        dword ptr [ebp + 0x908], 0xfff0bdc1
0040b425  mov        dword ptr [ebp + 0x914], 0x4b2ed0
0040b42f  mov        dword ptr [ebp + 0x918], eax

// block 0040b435
0040b435  fstp       dword ptr [ebp + 0x910]
0040b43b  mov        dword ptr [ebp + 0x90c], ebx
0040b441  mov        dword ptr [ebp + 0x908], 0xffffffff
0040b44b  jmp        0x40b527

// block 0040b450
0040b450  cmp        ecx, 0x10000000
0040b456  je         0x40b4fd

// block 0040b45c
0040b45c  cmp        ecx, 0x20000000
0040b462  jne        0x40b4f7

// block 0040b468
0040b468  or         eax, ecx
0040b46a  mov        dword ptr [ebp + 0x528], eax
0040b470  fld        dword ptr [edi]
0040b472  fsub       dword ptr [ebp + 0x4d4]
0040b478  lea        esi, [edi + 8]
0040b47b  fidiv      dword ptr [esi]
0040b47d  fstp       dword ptr [ebp + 0x950]
0040b483  fld        dword ptr [edi + 4]
0040b486  fcomp      st(2)
0040b488  fnstsw     ax
0040b48a  fstp       st(1)
0040b48c  test       ah, 0x41
0040b48f  jp         0x40b49b

// block 0040b491
0040b491  fstp       st(0)
0040b493  fld        dword ptr [ebp + 0x4d8]
0040b499  jmp        0x40b4b7

// block 0040b49b
0040b49b  fld        dword ptr [edi + 4]
0040b49e  fcompp     
0040b4a0  fnstsw     ax
0040b4a2  test       ah, 1
0040b4a5  jne        0x40b4b4

// block 0040b4a7
0040b4a7  lea        ecx, [ebp + 0x4bc]
0040b4ad  call       0x4377a0

// block 0040b4b2
0040b4b2  jmp        0x40b4b7

// block 0040b4b4
0040b4b4  fld        dword ptr [edi + 4]

// block 0040b4b7
0040b4b7  fstp       dword ptr [esp + 0x14]
0040b4bb  push       ebx
0040b4bc  fld        dword ptr [esp + 0x18]
0040b4c0  lea        eax, [ebp + 0x93c]
0040b4c6  fstp       dword ptr [ebp + 0x954]
0040b4cc  call       0x4067e0

// block 0040b4d1
0040b4d1  mov        ecx, dword ptr [esi]
0040b4d3  fld        dword ptr [ebp + 0x950]
0040b4d9  sub        esp, 8
0040b4dc  fstp       dword ptr [esp + 4]
0040b4e0  mov        dword ptr [ebp + 0x964], ecx
0040b4e6  fld        dword ptr [ebp + 0x954]
0040b4ec  lea        ecx, [ebp + 0x958]
0040b4f2  jmp        0x40abc9

// block 0040b4f7
0040b4f7  fstp       st(1)
0040b4f9  fstp       st(0)
0040b4fb  jmp        0x40b527

// block 0040b4fd
0040b4fd  fstp       st(1)
0040b4ff  fstp       st(0)
0040b501  cmp        dword ptr [edi + 8], ebx
0040b504  je         0x40b51d

// block 0040b506
0040b506  mov        edx, dword ptr [ebp + 0x484]
0040b50c  and        edx, 0xffffff3f
0040b512  or         edx, 0x20
0040b515  mov        dword ptr [ebp + 0x484], edx
0040b51b  jmp        0x40b527

// block 0040b51d
0040b51d  and        dword ptr [ebp + 0x484], 0xffffff1f

// block 0040b527
0040b527  inc        dword ptr [ebp + 0x548]

// block 0040b52d
0040b52d  cmp        dword ptr [ebp + 0x548], 0x12
0040b534  jl         0x40aa80

// block 0040b53a
0040b53a  pop        edi
0040b53b  pop        esi
0040b53c  pop        ebp
0040b53d  pop        ebx
0040b53e  mov        esp, ebp
0040b540  pop        ebp
0040b541  ret        

// block 0040b542
0040b542  fstp       st(1)
0040b544  fstp       st(0)

// block 0040b546
0040b546  pop        edi
0040b547  pop        esi
0040b548  pop        ebp
0040b549  pop        ebx
0040b54a  mov        esp, ebp
0040b54c  pop        ebp
0040b54d  ret        
