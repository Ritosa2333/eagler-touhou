
// block 0041fcc0
0041fcc0  sub        esp, 0x48
0041fcc3  push       ebx
0041fcc4  push       ebp
0041fcc5  mov        ebp, dword ptr [esp + 0x54]
0041fcc9  mov        eax, dword ptr [ebp + 0x8c]
0041fccf  push       esi
0041fcd0  push       edi
0041fcd1  test       eax, eax
0041fcd3  jle        0x41fcdc

// block 0041fcd5
0041fcd5  dec        eax
0041fcd6  mov        dword ptr [ebp + 0x8c], eax

// block 0041fcdc
0041fcdc  test       byte ptr [ebp + 0x90], 1
0041fce3  je         0x41fd30

// block 0041fce5
0041fce5  test       dword ptr [0x4d49d0], 0x200
0041fcef  je         0x41fd30

// block 0041fcf1
0041fcf1  mov        eax, dword ptr [ebp + 0x64]
0041fcf4  movzx      ecx, word ptr [eax]
0041fcf7  mov        eax, dword ptr [ebp + 0x28]
0041fcfa  mov        dword ptr [esp + 0x5c], ecx
0041fcfe  test       al, 1
0041fd00  jne        0x41fd22

// block 0041fd02
0041fd02  fldz       
0041fd04  or         eax, 1
0041fd07  fstp       dword ptr [ebp + 0x20]
0041fd0a  mov        dword ptr [ebp + 0x1c], 0
0041fd11  mov        dword ptr [ebp + 0x18], 0xfff0bdc1
0041fd18  mov        dword ptr [ebp + 0x24], 0x4b2ed0
0041fd1f  mov        dword ptr [ebp + 0x28], eax

// block 0041fd22
0041fd22  fild       dword ptr [esp + 0x5c]
0041fd26  mov        dword ptr [ebp + 0x1c], ecx
0041fd29  dec        ecx
0041fd2a  mov        dword ptr [ebp + 0x18], ecx
0041fd2d  fstp       dword ptr [ebp + 0x20]

// block 0041fd30
0041fd30  mov        ecx, dword ptr [ebp + 0x64]
0041fd33  movzx      edx, word ptr [ecx]
0041fd36  cmp        dword ptr [ebp + 0x1c], edx
0041fd39  jl         0x420d09

// block 0041fd3f
0041fd3f  nop        

// block 0041fd40
0041fd40  mov        eax, dword ptr [ebp + 0x64]
0041fd43  movzx      ecx, byte ptr [eax + 2]
0041fd47  cmp        ecx, 0x1b
0041fd4a  ja         0x420ced

// block 0041fd50
0041fd50  jmp        dword ptr [ecx*4 + 0x420d94]

// block 0041fd57
0041fd57  or         dword ptr [ebp + 0x90], 2
0041fd5e  jmp        0x420ced

// block 0041fd63
0041fd63  mov        eax, dword ptr [ebp + 0x4c]
0041fd66  mov        edx, dword ptr [0x4ce8cc]
0041fd6c  push       eax
0041fd6d  call       0x461920

// block 0041fd72
0041fd72  mov        esi, eax
0041fd74  test       esi, esi
0041fd76  jne        0x41fd7b

// block 0041fd78
0041fd78  mov        dword ptr [ebp + 0x4c], eax

// block 0041fd7b
0041fd7b  mov        eax, dword ptr [ebp + 0x64]
0041fd7e  add        eax, 4
0041fd81  call       0x420e40

// block 0041fd86
0041fd86  mov        edi, dword ptr [ebp + 0x90]
0041fd8c  mov        ecx, dword ptr [ebp + 0x9c]
0041fd92  mov        edx, dword ptr [ebp + ecx*4 + 0xa0]
0041fd99  push       eax
0041fd9a  push       0
0041fd9c  push       0
0041fd9e  shr        edi, 1
0041fda0  push       0
0041fda2  and        edi, 1
0041fda5  push       edx
0041fda6  call       0x460760

// block 0041fdab
0041fdab  mov        eax, dword ptr [ebp + 0x4c]
0041fdae  add        esp, 0x14
0041fdb1  push       eax
0041fdb2  mov        ebx, 2
0041fdb7  call       0x461970

// block 0041fdbc
0041fdbc  jmp        0x420ced

// block 0041fdc1
0041fdc1  mov        ecx, dword ptr [ebp + 0x50]
0041fdc4  mov        edx, dword ptr [0x4ce8cc]
0041fdca  push       ecx
0041fdcb  call       0x461920

// block 0041fdd0
0041fdd0  mov        esi, eax
0041fdd2  test       esi, esi
0041fdd4  jne        0x41fdd9

// block 0041fdd6
0041fdd6  mov        dword ptr [ebp + 0x50], eax

// block 0041fdd9
0041fdd9  mov        eax, dword ptr [ebp + 0x64]
0041fddc  add        eax, 4
0041fddf  call       0x420e40

// block 0041fde4
0041fde4  mov        edi, dword ptr [ebp + 0x90]
0041fdea  mov        edx, dword ptr [ebp + 0x9c]
0041fdf0  push       eax
0041fdf1  mov        eax, dword ptr [ebp + edx*4 + 0xa0]
0041fdf8  push       0
0041fdfa  push       0
0041fdfc  shr        edi, 1
0041fdfe  push       0
0041fe00  and        edi, 1
0041fe03  push       eax
0041fe04  call       0x460760

// block 0041fe09
0041fe09  mov        ecx, dword ptr [ebp + 0x50]
0041fe0c  add        esp, 0x14
0041fe0f  push       ecx
0041fe10  mov        ebx, 2
0041fe15  call       0x461970

// block 0041fe1a
0041fe1a  jmp        0x420ced

// block 0041fe1f
0041fe1f  cmp        dword ptr [ebp + 0x94], 0
0041fe26  jne        0x420012

// block 0041fe2c
0041fe2c  cmp        dword ptr [ebp + 0x98], 0
0041fe33  jne        0x41ff45

// block 0041fe39
0041fe39  mov        edx, dword ptr [ebp + 0x9c]
0041fe3f  mov        edi, dword ptr [ebp + 0x90]
0041fe45  mov        eax, dword ptr [ebp + edx*4 + 0xa0]
0041fe4c  push       0x49fb38
0041fe51  push       0
0041fe53  push       0
0041fe55  push       0
0041fe57  lea        ebx, [ebp + 0x4c]
0041fe5a  shr        edi, 1
0041fe5c  push       eax
0041fe5d  mov        esi, ebx
0041fe5f  and        edi, 1
0041fe62  call       0x461c50

// block 0041fe67
0041fe67  mov        esi, eax
0041fe69  call       0x460760

// block 0041fe6e
0041fe6e  mov        ecx, dword ptr [ebp + 0x9c]
0041fe74  mov        edi, dword ptr [ebp + 0x90]
0041fe7a  mov        edx, dword ptr [ebp + ecx*4 + 0xa0]
0041fe81  add        esp, 0x14
0041fe84  push       0x49fb38
0041fe89  push       0
0041fe8b  push       0
0041fe8d  push       0
0041fe8f  shr        edi, 1
0041fe91  lea        esi, [ebp + 0x50]
0041fe94  push       edx
0041fe95  and        edi, 1
0041fe98  call       0x461c50

// block 0041fe9d
0041fe9d  mov        esi, eax
0041fe9f  call       0x460760

// block 0041fea4
0041fea4  mov        eax, dword ptr [ebp + 0x9c]
0041feaa  mov        edi, dword ptr [ebp + 0x90]
0041feb0  mov        ecx, dword ptr [ebp + eax*4 + 0xa0]
0041feb7  add        esp, 0x14
0041feba  push       0x49fb38
0041febf  push       1
0041fec1  push       0
0041fec3  push       0
0041fec5  shr        edi, 1
0041fec7  lea        esi, [ebp + 0x54]
0041feca  push       ecx
0041fecb  and        edi, 1
0041fece  call       0x461c50

// block 0041fed3
0041fed3  mov        esi, eax
0041fed5  call       0x460760

// block 0041feda
0041feda  mov        edx, dword ptr [ebp + 0x9c]
0041fee0  mov        edi, dword ptr [ebp + 0x90]
0041fee6  mov        eax, dword ptr [ebp + edx*4 + 0xa0]
0041feed  add        esp, 0x14
0041fef0  push       0x49fb38
0041fef5  push       1
0041fef7  push       0
0041fef9  push       0
0041fefb  shr        edi, 1
0041fefd  lea        esi, [ebp + 0x58]
0041ff00  push       eax
0041ff01  and        edi, 1
0041ff04  call       0x461c50

// block 0041ff09
0041ff09  mov        esi, eax
0041ff0b  call       0x460760

// block 0041ff10
0041ff10  mov        ecx, dword ptr [ebx]
0041ff12  add        esp, 0x14
0041ff15  push       ecx
0041ff16  mov        ebx, 3
0041ff1b  mov        dword ptr [ebp + 0x98], 1
0041ff25  call       0x461970

// block 0041ff2a
0041ff2a  mov        edx, dword ptr [ebp + 0x50]
0041ff2d  push       edx
0041ff2e  call       0x461970

// block 0041ff33
0041ff33  mov        eax, dword ptr [ebp + 0x54]
0041ff36  push       eax
0041ff37  call       0x461970

// block 0041ff3c
0041ff3c  mov        ecx, dword ptr [ebp + 0x58]
0041ff3f  push       ecx
0041ff40  call       0x461970

// block 0041ff45
0041ff45  mov        eax, dword ptr [ebp + 0x64]
0041ff48  add        eax, 4
0041ff4b  call       0x420e40

// block 0041ff50
0041ff50  cmp        byte ptr [eax], 0x7c
0041ff53  jne        0x41ffc6

// block 0041ff55
0041ff55  lea        esi, [eax + 1]
0041ff58  push       esi
0041ff59  call       0x46d143

// block 0041ff5e
0041ff5e  add        esp, 4
0041ff61  push       0x2c
0041ff63  push       esi
0041ff64  mov        edi, eax
0041ff66  call       0x46d1a0

// block 0041ff6b
0041ff6b  lea        esi, [eax + 1]
0041ff6e  push       esi
0041ff6f  call       0x46d143

// block 0041ff74
0041ff74  add        esp, 0xc
0041ff77  push       0x2c
0041ff79  push       esi
0041ff7a  mov        dword ptr [esp + 0x64], eax
0041ff7e  call       0x46d1a0

// block 0041ff83
0041ff83  mov        edx, dword ptr [esp + 0x64]
0041ff87  add        esp, 8
0041ff8a  inc        eax
0041ff8b  push       eax
0041ff8c  mov        eax, dword ptr [ebp + 0x9c]
0041ff92  mov        ecx, dword ptr [ebp + eax*4 + 0xa0]
0041ff99  push       edx
0041ff9a  push       edi
0041ff9b  push       0
0041ff9d  lea        ebx, [ebp + 0x54]
0041ffa0  push       ecx
0041ffa1  mov        esi, ebx
0041ffa3  call       0x461c50

// block 0041ffa8
0041ffa8  mov        edi, 2
0041ffad  mov        esi, eax
0041ffaf  call       0x460760

// block 0041ffb4
0041ffb4  mov        edx, dword ptr [ebx]
0041ffb6  add        esp, 0x14
0041ffb9  push       edx
0041ffba  mov        ebx, edi
0041ffbc  call       0x461970

// block 0041ffc1
0041ffc1  jmp        0x420ced

// block 0041ffc6
0041ffc6  mov        edi, dword ptr [ebp + 0x90]
0041ffcc  push       eax
0041ffcd  mov        eax, dword ptr [ebp + 0x9c]
0041ffd3  mov        ecx, dword ptr [ebp + eax*4 + 0xa0]
0041ffda  push       0
0041ffdc  push       0
0041ffde  push       0
0041ffe0  lea        ebx, [ebp + 0x4c]
0041ffe3  shr        edi, 1
0041ffe5  push       ecx
0041ffe6  mov        esi, ebx
0041ffe8  and        edi, 1
0041ffeb  call       0x461c50

// block 0041fff0
0041fff0  mov        esi, eax
0041fff2  call       0x460760

// block 0041fff7
0041fff7  mov        edx, dword ptr [ebx]
0041fff9  add        esp, 0x14
0041fffc  push       edx
0041fffd  mov        ebx, 2
00420002  call       0x461970

// block 00420007
00420007  inc        dword ptr [ebp + 0x94]
0042000d  jmp        0x420ced

// block 00420012
00420012  add        eax, 4
00420015  call       0x420e40

// block 0042001a
0042001a  cmp        byte ptr [eax], 0x7c
0042001d  jne        0x420090

// block 0042001f
0042001f  lea        esi, [eax + 1]
00420022  push       esi
00420023  call       0x46d143

// block 00420028
00420028  add        esp, 4
0042002b  push       0x2c
0042002d  push       esi
0042002e  mov        edi, eax
00420030  call       0x46d1a0

// block 00420035
00420035  lea        esi, [eax + 1]
00420038  push       esi
00420039  call       0x46d143

// block 0042003e
0042003e  add        esp, 0xc
00420041  push       0x2c
00420043  push       esi
00420044  mov        dword ptr [esp + 0x64], eax
00420048  call       0x46d1a0

// block 0042004d
0042004d  mov        ecx, dword ptr [ebp + 0x9c]
00420053  mov        edx, dword ptr [ebp + ecx*4 + 0xa0]
0042005a  add        esp, 8
0042005d  inc        eax
0042005e  push       eax
0042005f  mov        eax, dword ptr [esp + 0x60]
00420063  push       eax
00420064  push       edi
00420065  push       0
00420067  lea        ebx, [ebp + 0x58]
0042006a  push       edx
0042006b  mov        esi, ebx
0042006d  call       0x461c50

// block 00420072
00420072  mov        edi, 2
00420077  mov        esi, eax
00420079  call       0x460760

// block 0042007e
0042007e  mov        eax, dword ptr [ebx]
00420080  add        esp, 0x14
00420083  push       eax
00420084  mov        ebx, edi
00420086  call       0x461970

// block 0042008b
0042008b  jmp        0x420ced

// block 00420090
00420090  mov        ecx, dword ptr [ebp + 0x9c]
00420096  mov        edi, dword ptr [ebp + 0x90]
0042009c  mov        edx, dword ptr [ebp + ecx*4 + 0xa0]
004200a3  push       eax
004200a4  push       0
004200a6  push       0
004200a8  push       0
004200aa  lea        ebx, [ebp + 0x50]
004200ad  shr        edi, 1
004200af  push       edx
004200b0  mov        esi, ebx
004200b2  and        edi, 1
004200b5  call       0x461c50

// block 004200ba
004200ba  mov        esi, eax
004200bc  call       0x460760

// block 004200c1
004200c1  mov        eax, dword ptr [ebx]
004200c3  add        esp, 0x14
004200c6  push       eax
004200c7  mov        ebx, 2
004200cc  call       0x461970

// block 004200d1
004200d1  xor        eax, eax
004200d3  mov        dword ptr [ebp + 0x94], eax
004200d9  mov        dword ptr [ebp + 0x98], eax
004200df  jmp        0x420ced

// block 004200e4
004200e4  mov        ecx, dword ptr [ebp + 0x4c]
004200e7  push       ecx
004200e8  mov        ebx, 3
004200ed  call       0x461970

// block 004200f2
004200f2  mov        edx, dword ptr [ebp + 0x50]
004200f5  push       edx
004200f6  call       0x461970

// block 004200fb
004200fb  mov        eax, dword ptr [ebp + 0x54]
004200fe  push       eax
004200ff  call       0x461970

// block 00420104
00420104  mov        ecx, dword ptr [ebp + 0x58]
00420107  push       ecx
00420108  call       0x461970

// block 0042010d
0042010d  jmp        0x420ced

// block 00420112
00420112  mov        eax, dword ptr [0x4b43e4]
00420117  mov        ecx, dword ptr [eax + 0x6d44]
0042011d  push       0
0042011f  push       0x34
00420121  lea        edx, [esp + 0x18]
00420125  push       edx
00420126  push       ecx
00420127  mov        eax, 0x17
0042012c  xor        ecx, ecx
0042012e  call       0x4615a0

// block 00420133
00420133  mov        edx, dword ptr [esp + 0x10]
00420137  mov        dword ptr [ebp + 0x48], edx
0042013a  jmp        0x420ced

// block 0042013f
0042013f  mov        eax, dword ptr [0x4b0c90]
00420144  mov        ecx, dword ptr [eax*4 + 0x4b2f84]
0042014b  mov        eax, dword ptr [0x4b4514]
00420150  push       0
00420152  push       ecx
00420153  mov        ecx, dword ptr [eax + 0x10]
00420156  lea        edx, [esp + 0x1c]
0042015a  push       edx
0042015b  push       ecx
0042015c  mov        eax, 0x17
00420161  xor        ecx, ecx
00420163  call       0x4615a0

// block 00420168
00420168  mov        edx, dword ptr [esp + 0x14]
0042016c  mov        dword ptr [ebp + 0x40], edx
0042016f  jmp        0x420ced

// block 00420174
00420174  mov        eax, dword ptr [0x4b0cb0]
00420179  cmp        eax, 5
0042017c  jne        0x4201af

// block 0042017e
0042017e  cmp        dword ptr [ebp], 2
00420182  jl         0x4201e5

// block 00420184
00420184  mov        ecx, dword ptr [0x4b43dc]
0042018a  mov        edx, dword ptr [ecx + 0x4c]
0042018d  push       0
0042018f  push       0xa
00420191  lea        eax, [esp + 0x20]
00420195  push       eax
00420196  push       edx
00420197  mov        eax, 0x17
0042019c  xor        ecx, ecx
0042019e  call       0x4615a0

// block 004201a3
004201a3  mov        eax, dword ptr [esp + 0x18]
004201a7  mov        dword ptr [ebp + 0x44], eax
004201aa  jmp        0x420ced

// block 004201af
004201af  cmp        eax, 7
004201b2  jne        0x4201e5

// block 004201b4
004201b4  cmp        dword ptr [ebp], 2
004201b8  jl         0x4201e5

// block 004201ba
004201ba  mov        edx, dword ptr [0x4b43dc]
004201c0  mov        eax, dword ptr [edx + 0x4c]
004201c3  push       0
004201c5  push       0xd
004201c7  lea        ecx, [esp + 0x24]
004201cb  push       ecx
004201cc  push       eax
004201cd  mov        eax, 0x17
004201d2  xor        ecx, ecx
004201d4  call       0x4615a0

// block 004201d9
004201d9  mov        ecx, dword ptr [esp + 0x1c]
004201dd  mov        dword ptr [ebp + 0x44], ecx
004201e0  jmp        0x420ced

// block 004201e5
004201e5  mov        edx, dword ptr [eax*4 + 0x4b2f90]
004201ec  mov        ecx, dword ptr [0x4b43dc]
004201f2  push       0
004201f4  push       edx
004201f5  mov        edx, dword ptr [ecx + 0x48]
004201f8  lea        eax, [esp + 0x28]
004201fc  push       eax
004201fd  push       edx
004201fe  mov        eax, 0x17
00420203  xor        ecx, ecx
00420205  call       0x4615a0

// block 0042020a
0042020a  mov        eax, dword ptr [esp + 0x20]
0042020e  mov        dword ptr [ebp + 0x44], eax
00420211  jmp        0x420ced

// block 00420216
00420216  mov        ecx, dword ptr [ebp + 0x4c]
00420219  mov        edi, dword ptr [0x4ce8cc]
0042021f  push       ecx
00420220  mov        edx, edi
00420222  call       0x461920

// block 00420227
00420227  xor        esi, esi
00420229  cmp        eax, esi
0042022b  jne        0x420230

// block 0042022d
0042022d  mov        dword ptr [ebp + 0x4c], esi

// block 00420230
00420230  mov        edx, dword ptr [ebp + 0x64]
00420233  fild       dword ptr [edx + 4]
00420236  mov        edx, edi
00420238  fstp       dword ptr [eax + 0x440]
0042023e  mov        eax, dword ptr [ebp + 0x50]
00420241  push       eax
00420242  call       0x461920

// block 00420247
00420247  cmp        eax, esi
00420249  jne        0x42024e

// block 0042024b
0042024b  mov        dword ptr [ebp + 0x50], esi

// block 0042024e
0042024e  mov        ecx, dword ptr [ebp + 0x64]
00420251  fild       dword ptr [ecx + 4]
00420254  fstp       dword ptr [eax + 0x440]
0042025a  mov        edx, dword ptr [ebp + 0x54]
0042025d  push       edx
0042025e  mov        edx, edi
00420260  call       0x461920

// block 00420265
00420265  cmp        eax, esi
00420267  jne        0x42026c

// block 00420269
00420269  mov        dword ptr [ebp + 0x54], esi

// block 0042026c
0042026c  mov        ecx, dword ptr [ebp + 0x64]
0042026f  fild       dword ptr [ecx + 4]
00420272  fstp       dword ptr [eax + 0x440]
00420278  mov        edx, dword ptr [ebp + 0x58]
0042027b  push       edx
0042027c  mov        edx, edi
0042027e  call       0x461920

// block 00420283
00420283  cmp        eax, esi
00420285  jne        0x42028a

// block 00420287
00420287  mov        dword ptr [ebp + 0x58], esi

// block 0042028a
0042028a  mov        ecx, dword ptr [ebp + 0x64]
0042028d  fild       dword ptr [ecx + 4]
00420290  fstp       dword ptr [eax + 0x440]
00420296  jmp        0x420ced

// block 0042029b
0042029b  mov        edx, dword ptr [ebp + 0x40]
0042029e  push       edx
0042029f  mov        ebx, 7
004202a4  call       0x461970

// block 004202a9
004202a9  jmp        0x420ced

// block 004202ae
004202ae  mov        eax, dword ptr [ebp + 0x44]
004202b1  push       eax
004202b2  mov        ebx, 7
004202b7  call       0x461970

// block 004202bc
004202bc  jmp        0x420ced

// block 004202c1
004202c1  mov        ecx, dword ptr [ebp + 0x40]
004202c4  push       ecx
004202c5  mov        ebx, 1
004202ca  call       0x461970

// block 004202cf
004202cf  mov        dword ptr [ebp + 0x40], 0
004202d6  jmp        0x420ced

// block 004202db
004202db  mov        edx, dword ptr [ebp + 0x44]
004202de  push       edx
004202df  mov        ebx, 1
004202e4  call       0x461970

// block 004202e9
004202e9  mov        eax, dword ptr [ebp + 0x5c]
004202ec  push       eax
004202ed  mov        dword ptr [ebp + 0x44], 0
004202f4  call       0x461970

// block 004202f9
004202f9  jmp        0x420ced

// block 004202fe
004202fe  mov        ecx, dword ptr [ebp + 0x48]
00420301  push       ecx
00420302  mov        ebx, 1
00420307  call       0x461970

// block 0042030c
0042030c  mov        edx, dword ptr [ebp + 0x4c]
0042030f  push       edx
00420310  call       0x461970

// block 00420315
00420315  mov        eax, dword ptr [ebp + 0x50]
00420318  push       eax
00420319  call       0x461970

// block 0042031e
0042031e  mov        ecx, dword ptr [ebp + 0x54]
00420321  push       ecx
00420322  call       0x461970

// block 00420327
00420327  mov        edx, dword ptr [ebp + 0x58]
0042032a  push       edx
0042032b  call       0x461970

// block 00420330
00420330  jmp        0x420ced

// block 00420335
00420335  mov        eax, dword ptr [ebp + 0x44]
00420338  push       eax
00420339  mov        ebx, 3
0042033e  call       0x461970

// block 00420343
00420343  mov        ecx, dword ptr [ebp + 0x40]
00420346  push       ecx
00420347  mov        ebx, 2
0042034c  call       0x461970

// block 00420351
00420351  mov        edx, dword ptr [ebp + 0x48]
00420354  push       edx
00420355  call       0x461970

// block 0042035a
0042035a  lea        ebx, [ebp + 0x4c]
0042035d  lea        esi, [ebp + 0x68]
00420360  mov        eax, ebx
00420362  mov        dword ptr [ebp + 0x9c], 0
0042036c  call       0x461dd0

// block 00420371
00420371  mov        eax, dword ptr [ebp + 0x9c]
00420377  lea        eax, [eax + eax*2]
0042037a  lea        edi, [ebp + 0x50]
0042037d  lea        esi, [ebp + eax*4 + 0x68]
00420381  mov        eax, edi
00420383  call       0x461dd0

// block 00420388
00420388  mov        ecx, dword ptr [ebx]
0042038a  mov        esi, dword ptr [0x4ce8cc]
00420390  push       ecx
00420391  mov        edx, esi
00420393  call       0x461920

// block 00420398
00420398  test       eax, eax
0042039a  jne        0x42039e

// block 0042039c
0042039c  mov        dword ptr [ebx], eax

// block 0042039e
0042039e  fldz       
004203a0  fst        dword ptr [eax + 0x440]
004203a6  mov        edx, dword ptr [edi]
004203a8  push       edx
004203a9  mov        edx, esi
004203ab  call       0x461920

// block 004203b0
004203b0  test       eax, eax
004203b2  jne        0x4203b6

// block 004203b4
004203b4  mov        dword ptr [edi], eax

// block 004203b6
004203b6  fst        dword ptr [eax + 0x440]
004203bc  mov        eax, dword ptr [ebp + 0x9c]
004203c2  lea        eax, [eax + eax*2]
004203c5  lea        ebx, [ebp + 0x54]
004203c8  lea        esi, [ebp + eax*4 + 0x68]
004203cc  mov        eax, ebx
004203ce  call       0x461dd0

// block 004203d3
004203d3  mov        eax, dword ptr [ebp + 0x9c]
004203d9  lea        ecx, [eax + eax*2]
004203dc  lea        edi, [ebp + 0x58]
004203df  lea        esi, [ebp + ecx*4 + 0x68]
004203e3  mov        eax, edi
004203e5  call       0x461dd0

// block 004203ea
004203ea  mov        edx, dword ptr [ebx]
004203ec  push       edx
004203ed  mov        edx, dword ptr [0x4ce8cc]
004203f3  call       0x461920

// block 004203f8
004203f8  test       eax, eax
004203fa  jne        0x4203fe

// block 004203fc
004203fc  mov        dword ptr [ebx], eax

// block 004203fe
004203fe  mov        edx, dword ptr [0x4ce8cc]
00420404  fst        dword ptr [eax + 0x440]
0042040a  mov        eax, dword ptr [edi]
0042040c  push       eax
0042040d  call       0x461920

// block 00420412
00420412  xor        ecx, ecx
00420414  cmp        eax, ecx
00420416  jne        0x42041a

// block 00420418
00420418  mov        dword ptr [edi], ecx

// block 0042041a
0042041a  fstp       dword ptr [eax + 0x440]
00420420  and        dword ptr [ebp + 0x90], 0xfffffffd
00420427  mov        dword ptr [ebp + 0x94], ecx
0042042d  mov        dword ptr [ebp + 0x98], ecx
00420433  jmp        0x420ced

// block 00420438
00420438  mov        ecx, dword ptr [ebp + 0x40]
0042043b  push       ecx
0042043c  mov        ebx, 3
00420441  call       0x461970

// block 00420446
00420446  mov        edx, dword ptr [ebp + 0x44]
00420449  push       edx
0042044a  mov        ebx, 2
0042044f  call       0x461970

// block 00420454
00420454  mov        eax, dword ptr [ebp + 0x48]
00420457  push       eax
00420458  mov        ebx, 3
0042045d  call       0x461970

// block 00420462
00420462  lea        ebx, [ebp + 0x4c]
00420465  lea        esi, [ebp + 0x74]
00420468  mov        eax, ebx
0042046a  mov        dword ptr [ebp + 0x9c], 1
00420474  call       0x461dd0

// block 00420479
00420479  mov        eax, dword ptr [ebp + 0x9c]
0042047f  lea        ecx, [eax + eax*2]
00420482  lea        edi, [ebp + 0x50]
00420485  lea        esi, [ebp + ecx*4 + 0x68]
00420489  mov        eax, edi
0042048b  call       0x461dd0

// block 00420490
00420490  mov        edx, dword ptr [ebx]
00420492  push       edx
00420493  mov        edx, dword ptr [0x4ce8cc]
00420499  call       0x461920

// block 0042049e
0042049e  test       eax, eax
004204a0  jne        0x4204a4

// block 004204a2
004204a2  mov        dword ptr [ebx], eax

// block 004204a4
004204a4  fldz       
004204a6  mov        edx, dword ptr [0x4ce8cc]
004204ac  fst        dword ptr [eax + 0x440]
004204b2  mov        eax, dword ptr [edi]
004204b4  push       eax
004204b5  call       0x461920

// block 004204ba
004204ba  test       eax, eax
004204bc  jne        0x4204c0

// block 004204be
004204be  mov        dword ptr [edi], eax

// block 004204c0
004204c0  fst        dword ptr [eax + 0x440]
004204c6  mov        eax, dword ptr [ebp + 0x9c]
004204cc  lea        ecx, [eax + eax*2]
004204cf  lea        ebx, [ebp + 0x54]
004204d2  lea        esi, [ebp + ecx*4 + 0x68]
004204d6  mov        eax, ebx
004204d8  call       0x461dd0

// block 004204dd
004204dd  mov        eax, dword ptr [ebp + 0x9c]
004204e3  lea        edx, [eax + eax*2]
004204e6  lea        edi, [ebp + 0x58]
004204e9  lea        esi, [ebp + edx*4 + 0x68]
004204ed  mov        eax, edi
004204ef  call       0x461dd0

// block 004204f4
004204f4  mov        eax, dword ptr [ebx]
004204f6  mov        edx, dword ptr [0x4ce8cc]
004204fc  push       eax
004204fd  call       0x461920

// block 00420502
00420502  xor        esi, esi
00420504  cmp        eax, esi
00420506  jne        0x42050a

// block 00420508
00420508  mov        dword ptr [ebx], esi

// block 0042050a
0042050a  mov        edx, dword ptr [0x4ce8cc]
00420510  fst        dword ptr [eax + 0x440]
00420516  mov        ecx, dword ptr [edi]
00420518  push       ecx
00420519  call       0x461920

// block 0042051e
0042051e  cmp        eax, esi
00420520  jne        0x420524

// block 00420522
00420522  mov        dword ptr [edi], esi

// block 00420524
00420524  fstp       dword ptr [eax + 0x440]
0042052a  and        dword ptr [ebp + 0x90], 0xfffffffd
00420531  mov        dword ptr [ebp + 0x94], esi
00420537  mov        dword ptr [ebp + 0x98], esi
0042053d  jmp        0x420ced

// block 00420542
00420542  mov        edx, dword ptr [ebp + 0x44]
00420545  push       edx
00420546  mov        ebx, 8
0042054b  call       0x461970

// block 00420550
00420550  mov        eax, dword ptr [ebp + 0x40]
00420553  push       eax
00420554  call       0x461970

// block 00420559
00420559  mov        ecx, dword ptr [ebp + 0x48]
0042055c  push       ecx
0042055d  call       0x461970

// block 00420562
00420562  lea        ebx, [ebp + 0x4c]
00420565  lea        esi, [ebp + 0x80]
0042056b  mov        eax, ebx
0042056d  mov        dword ptr [ebp + 0x9c], 2
00420577  call       0x461dd0

// block 0042057c
0042057c  mov        eax, dword ptr [ebp + 0x9c]
00420582  lea        edx, [eax + eax*2]
00420585  lea        edi, [ebp + 0x50]
00420588  lea        esi, [ebp + edx*4 + 0x68]
0042058c  mov        eax, edi
0042058e  call       0x461dd0

// block 00420593
00420593  mov        eax, dword ptr [ebx]
00420595  mov        esi, dword ptr [0x4ce8cc]
0042059b  push       eax
0042059c  mov        edx, esi
0042059e  call       0x461920

// block 004205a3
004205a3  test       eax, eax
004205a5  jne        0x4205a9

// block 004205a7
004205a7  mov        dword ptr [ebx], eax

// block 004205a9
004205a9  fldz       
004205ab  mov        edx, esi
004205ad  fst        dword ptr [eax + 0x440]
004205b3  mov        ecx, dword ptr [edi]
004205b5  push       ecx
004205b6  call       0x461920

// block 004205bb
004205bb  test       eax, eax
004205bd  jne        0x4205c1

// block 004205bf
004205bf  mov        dword ptr [edi], eax

// block 004205c1
004205c1  fst        dword ptr [eax + 0x440]
004205c7  mov        eax, dword ptr [ebp + 0x9c]
004205cd  lea        edx, [eax + eax*2]
004205d0  lea        ebx, [ebp + 0x54]
004205d3  lea        esi, [ebp + edx*4 + 0x68]
004205d7  mov        eax, ebx
004205d9  call       0x461dd0

// block 004205de
004205de  mov        eax, dword ptr [ebp + 0x9c]
004205e4  lea        eax, [eax + eax*2]
004205e7  lea        edi, [ebp + 0x58]
004205ea  lea        esi, [ebp + eax*4 + 0x68]
004205ee  mov        eax, edi
004205f0  call       0x461dd0

// block 004205f5
004205f5  mov        ecx, dword ptr [ebx]
004205f7  mov        edx, dword ptr [0x4ce8cc]
004205fd  push       ecx
004205fe  call       0x461920

// block 00420603
00420603  test       eax, eax
00420605  jne        0x420609

// block 00420607
00420607  mov        dword ptr [ebx], eax

// block 00420609
00420609  fst        dword ptr [eax + 0x440]
0042060f  mov        edx, dword ptr [edi]
00420611  push       edx
00420612  mov        edx, dword ptr [0x4ce8cc]
00420618  call       0x461920

// block 0042061d
0042061d  xor        ecx, ecx
0042061f  cmp        eax, ecx
00420621  jne        0x420625

// block 00420623
00420623  mov        dword ptr [edi], ecx

// block 00420625
00420625  fstp       dword ptr [eax + 0x440]
0042062b  or         dword ptr [ebp + 0x90], 2
00420632  mov        dword ptr [ebp + 0x94], ecx
00420638  mov        dword ptr [ebp + 0x98], ecx
0042063e  jmp        0x420ced

// block 00420643
00420643  movsx      eax, byte ptr [eax + 4]
00420647  xor        eax, dword ptr [ebp + 0x90]
0042064d  and        eax, 1
00420650  xor        dword ptr [ebp + 0x90], eax
00420656  jmp        0x420ced

// block 0042065b
0042065b  mov        dword ptr [esp + 0x5c], 0
00420663  lea        esi, [ebp + 0x40]
00420666  jmp        0x420670

// block 00420670
00420670  mov        ecx, dword ptr [0x4b0c90]
00420676  mov        edx, dword ptr [esp + 0x5c]
0042067a  lea        eax, [edx + ecx*2]
0042067d  mov        edx, dword ptr [ebp + 0x64]
00420680  add        eax, ecx
00420682  add        eax, eax
00420684  mov        ecx, dword ptr [eax + eax + 0x4b2fd4]
0042068b  add        ecx, dword ptr [edx + 4]
0042068e  mov        edi, dword ptr [eax + eax + 0x4b2fb0]
00420695  add        eax, eax
00420697  lea        ebx, [esp + 0x44]
0042069b  mov        dword ptr [esp + 0x24], ecx
0042069f  call       0x462060

// block 004206a4
004206a4  mov        eax, dword ptr [eax]
004206a6  mov        edx, dword ptr [0x4ce8cc]
004206ac  push       eax
004206ad  call       0x461920

// block 004206b2
004206b2  test       eax, eax
004206b4  je         0x4206c5

// block 004206b6
004206b6  mov        ecx, dword ptr [esp + 0x24]
004206ba  mov        edx, dword ptr [eax + 0x3f8]
004206c0  call       0x454b80

// block 004206c5
004206c5  mov        eax, dword ptr [esp + 0x5c]
004206c9  inc        eax
004206ca  cmp        eax, 3
004206cd  mov        dword ptr [esp + 0x5c], eax
004206d1  jl         0x420670

// block 004206d3
004206d3  jmp        0x420ced

// block 004206d8
004206d8  mov        ecx, dword ptr [0x4b0cb0]
004206de  cmp        ecx, 5
004206e1  jne        0x420736

// block 004206e3
004206e3  cmp        dword ptr [ebp], 2
004206e7  jl         0x42078a

// block 004206ed
004206ed  xor        eax, eax
004206ef  mov        dword ptr [esp + 0x5c], eax

// block 004206f3
004206f3  mov        ecx, dword ptr [eax + 0x4b30d0]
004206f9  test       ecx, ecx
004206fb  jl         0x420725

// block 004206fd
004206fd  mov        edx, dword ptr [ebp + 0x64]
00420700  mov        eax, dword ptr [edx + 4]
00420703  add        eax, ecx
00420705  mov        ecx, dword ptr [esp + 0x5c]
00420709  mov        edi, dword ptr [ecx + 0x4b3058]
0042070f  push       eax
00420710  lea        esi, [ebp + 0x44]
00420713  lea        ebx, [esp + 0x4c]
00420717  call       0x462060

// block 0042071c
0042071c  call       0x461ea0

// block 00420721
00420721  mov        eax, dword ptr [esp + 0x5c]

// block 00420725
00420725  add        eax, 4
00420728  cmp        eax, 0xc
0042072b  mov        dword ptr [esp + 0x5c], eax
0042072f  jl         0x4206f3

// block 00420731
00420731  jmp        0x420ced

// block 00420736
00420736  cmp        ecx, 7
00420739  jne        0x42078a

// block 0042073b
0042073b  cmp        dword ptr [ebp], 2
0042073f  jl         0x42078a

// block 00420741
00420741  xor        eax, eax
00420743  mov        dword ptr [esp + 0x5c], eax

// block 00420747
00420747  mov        ecx, dword ptr [eax + 0x4b30dc]
0042074d  test       ecx, ecx
0042074f  jl         0x420779

// block 00420751
00420751  mov        edx, dword ptr [ebp + 0x64]
00420754  mov        eax, dword ptr [edx + 4]
00420757  add        eax, ecx
00420759  mov        ecx, dword ptr [esp + 0x5c]
0042075d  mov        edi, dword ptr [ecx + 0x4b3064]
00420763  push       eax
00420764  lea        esi, [ebp + 0x44]
00420767  lea        ebx, [esp + 0x50]
0042076b  call       0x462060

// block 00420770
00420770  call       0x461ea0

// block 00420775
00420775  mov        eax, dword ptr [esp + 0x5c]

// block 00420779
00420779  add        eax, 4
0042077c  cmp        eax, 0xc
0042077f  mov        dword ptr [esp + 0x5c], eax
00420783  jl         0x420747

// block 00420785
00420785  jmp        0x420ced

// block 0042078a
0042078a  mov        dword ptr [esp + 0x5c], 0

// block 00420792
00420792  mov        edx, dword ptr [esp + 0x5c]
00420796  lea        eax, [edx + ecx*2]
00420799  add        eax, ecx
0042079b  add        eax, eax
0042079d  mov        edx, dword ptr [eax + eax + 0x4b3070]
004207a4  add        eax, eax
004207a6  test       edx, edx
004207a8  jl         0x4207ef

// block 004207aa
004207aa  mov        ecx, dword ptr [ebp + 0x64]
004207ad  mov        ecx, dword ptr [ecx + 4]
004207b0  mov        edi, dword ptr [eax + 0x4b2ff8]
004207b6  add        ecx, edx
004207b8  lea        esi, [ebp + 0x44]
004207bb  lea        ebx, [esp + 0x50]
004207bf  mov        dword ptr [esp + 0x24], ecx
004207c3  call       0x462060

// block 004207c8
004207c8  mov        edx, dword ptr [eax]
004207ca  push       edx
004207cb  mov        edx, dword ptr [0x4ce8cc]
004207d1  call       0x461920

// block 004207d6
004207d6  test       eax, eax
004207d8  je         0x4207e9

// block 004207da
004207da  mov        ecx, dword ptr [esp + 0x24]
004207de  mov        edx, dword ptr [eax + 0x3f8]
004207e4  call       0x454b80

// block 004207e9
004207e9  mov        ecx, dword ptr [0x4b0cb0]

// block 004207ef
004207ef  mov        eax, dword ptr [esp + 0x5c]
004207f3  inc        eax
004207f4  cmp        eax, 3
004207f7  mov        dword ptr [esp + 0x5c], eax
004207fb  jl         0x420792

// block 004207fd
004207fd  jmp        0x420ced

// block 00420802
00420802  xor        edi, edi
00420804  cmp        dword ptr [ebp + 0x30], edi
00420807  jg         0x420815

// block 00420809
00420809  mov        eax, dword ptr [eax + 4]
0042080c  push       eax
0042080d  lea        eax, [ebp + 0x2c]
00420810  call       0x4067e0

// block 00420815
00420815  fld        dword ptr [0x4a3da8]
0042081b  push       ecx
0042081c  lea        esi, [ebp + 0x2c]
0042081f  fstp       dword ptr [esp]
00420822  call       0x464a20

// block 00420827
00420827  test       dword ptr [0x4d49dc], 0x80001
00420831  jne        0x42086e

// block 00420833
00420833  cmp        dword ptr [ebp + 0x30], edi
00420836  jle        0x42086e

// block 00420838
00420838  test       byte ptr [ebp + 0x90], 1
0042083f  je         0x420d85

// block 00420845
00420845  test       dword ptr [0x4d49d0], 0x200
0042084f  je         0x420d85

// block 00420855
00420855  push       edi
00420856  mov        eax, esi
00420858  call       0x4067e0

// block 0042085d
0042085d  mov        dword ptr [ebp + 0x94], edi
00420863  mov        dword ptr [ebp + 0x98], edi
00420869  jmp        0x420ced

// block 0042086e
0042086e  xor        edx, edx
00420870  call       0x453d90

// block 00420875
00420875  push       edi
00420876  mov        eax, esi
00420878  call       0x4067e0

// block 0042087d
0042087d  mov        dword ptr [ebp + 0x94], edi
00420883  mov        dword ptr [ebp + 0x98], edi
00420889  jmp        0x420ced

// block 0042088e
0042088e  mov        dword ptr [ebp + 0x8c], 1
00420898  jmp        0x420ced

// block 0042089d
0042089d  mov        ecx, dword ptr [0x4b452c]
004208a3  mov        edx, dword ptr [ecx + 0x38]
004208a6  push       edx
004208a7  push       1
004208a9  call       0x430150

// block 004208ae
004208ae  mov        ecx, dword ptr [0x4b43e4]
004208b4  mov        edx, dword ptr [ecx + 0x6ce4]
004208ba  push       0
004208bc  push       2
004208be  lea        eax, [esp + 0x5c]
004208c2  push       eax
004208c3  push       edx
004208c4  mov        eax, 0x17
004208c9  xor        ecx, ecx
004208cb  call       0x4615a0

// block 004208d0
004208d0  jmp        0x420ced

// block 004208d5
004208d5  mov        eax, dword ptr [0x4b0cb0]
004208da  dec        eax
004208db  cmp        eax, 6
004208de  ja         0x420ced

// block 004208e4
004208e4  jmp        dword ptr [eax*4 + 0x420e04]

// block 00420a16
00420a16  call       0x421350

// block 00420a1b
00420a1b  call       0x41d020

// block 00420a20
00420a20  call       0x406ed0

// block 00420a25
00420a25  mov        ecx, dword ptr [0x4b44e8]
00420a2b  cmp        dword ptr [ecx + 0x74], 0
00420a2f  mov        ebx, dword ptr [0x4b451c]
00420a35  jne        0x420a9b

// block 00420a37
00420a37  mov        eax, dword ptr [0x4b0ca8]
00420a3c  cmp        eax, 4
00420a3f  je         0x420a9b

// block 00420a41
00420a41  mov        esi, dword ptr [0x4b0c94]
00420a47  mov        edx, dword ptr [0x4b0c90]
00420a4d  lea        edx, [esi + edx*2]
00420a50  mov        esi, dword ptr [0x4b0cb0]
00420a56  imul       edx, edx, 0x45f4
00420a5c  lea        eax, [eax + eax*2]
00420a5f  lea        eax, [esi + eax*2]
00420a62  add        edx, ebx
00420a64  mov        byte ptr [edx + eax*8 + 0x5a9], 1
00420a6c  mov        eax, dword ptr [0x4b0c94]
00420a71  mov        edx, dword ptr [0x4b0c90]
00420a77  mov        esi, dword ptr [0x4b0cb0]
00420a7d  lea        edx, [eax + edx*2]
00420a80  mov        eax, dword ptr [0x4b0ca8]
00420a85  imul       edx, edx, 0x45f4
00420a8b  lea        eax, [eax + eax*2]
00420a8e  lea        eax, [esi + eax*2]
00420a91  add        edx, ebx
00420a93  mov        byte ptr [edx + eax*8 + 0x5a8], 1

// block 00420a9b
00420a9b  test       byte ptr [0x4b0ce0], 0x10
00420aa2  je         0x420aae

// block 00420aa4
00420aa4  call       0x433870

// block 00420aa9
00420aa9  jmp        0x420ced

// block 00420aae
00420aae  cmp        dword ptr [ecx + 0x74], 0
00420ab2  mov        edi, dword ptr [0x4b4518]
00420ab8  je         0x420acd

// block 00420aba
00420aba  mov        ecx, dword ptr [edi + 0x1c]
00420abd  test       byte ptr [ecx + 0xa], 1
00420ac1  je         0x420acd

// block 00420ac3
00420ac3  call       0x432850

// block 00420ac8
00420ac8  jmp        0x420ced

// block 00420acd
00420acd  mov        eax, dword ptr [0x4b0cb0]
00420ad2  cmp        eax, 6
00420ad5  jne        0x420c16

// block 00420adb
00420adb  mov        eax, dword ptr [0x4b43e4]
00420ae0  or         dword ptr [eax + 0x6d18], 0x20
00420ae7  mov        eax, 0x4b0c40
00420aec  call       0x421880

// block 00420af1
00420af1  mov        esi, eax
00420af3  mov        eax, dword ptr [0x4b0ca8]
00420af8  imul       esi, esi, 0x64
00420afb  cmp        eax, 4
00420afe  ja         0x420bac

// block 00420b04
00420b04  jmp        dword ptr [eax*4 + 0x420e20]

// block 00420b0b
00420b0b  mov        edx, dword ptr [0x4b0c98]
00420b11  mov        eax, dword ptr [0x4b0ca0]
00420b16  lea        ecx, [eax + edx*2]
00420b19  imul       ecx, ecx, 0x19
00420b1c  add        ecx, dword ptr [0x4b0c48]
00420b22  imul       ecx, ecx, 0x2710
00420b28  add        esi, ecx
00420b2a  jmp        0x420bac

// block 00420b2f
00420b2f  mov        edx, dword ptr [0x4b0c98]
00420b35  mov        eax, dword ptr [0x4b0ca0]
00420b3a  lea        ecx, [eax + edx*2]
00420b3d  imul       ecx, ecx, 0x32
00420b40  add        ecx, dword ptr [0x4b0c48]
00420b46  imul       ecx, ecx, 0x2710
00420b4c  add        esi, ecx
00420b4e  jmp        0x420bac

// block 00420b50
00420b50  mov        edx, dword ptr [0x4b0c98]
00420b56  mov        eax, dword ptr [0x4b0ca0]
00420b5b  lea        ecx, [eax + edx*2]
00420b5e  imul       ecx, ecx, 0x64
00420b61  add        ecx, dword ptr [0x4b0c48]
00420b67  imul       ecx, ecx, 0x2710
00420b6d  add        esi, ecx
00420b6f  jmp        0x420bac

// block 00420b71
00420b71  mov        edx, dword ptr [0x4b0c98]
00420b77  mov        eax, dword ptr [0x4b0ca0]
00420b7c  lea        ecx, [eax + edx*2]
00420b7f  imul       ecx, ecx, 0x96
00420b85  add        ecx, dword ptr [0x4b0c48]
00420b8b  imul       ecx, ecx, 0x2710
00420b91  add        esi, ecx
00420b93  jmp        0x420bac

// block 00420b95
00420b95  mov        edx, dword ptr [0x4b0c98]
00420b9b  imul       edx, edx, 0x64
00420b9e  add        edx, dword ptr [0x4b0c48]
00420ba4  imul       edx, edx, 0x61a80
00420baa  add        esi, edx

// block 00420bac
00420bac  push       esi
00420bad  mov        ecx, 0x4b0c40
00420bb2  call       0x40e730

// block 00420bb7
00420bb7  mov        eax, dword ptr [0x4b43e4]
00420bbc  add        dword ptr [eax + 0x6d48], esi
00420bc2  cmp        dword ptr [edi + 0x10], 1
00420bc6  je         0x420ac3

// block 00420bcc
00420bcc  or         dword ptr [eax + 0x6d18], 0x10
00420bd3  mov        dword ptr [eax + 0x6d4c], 0
00420bdd  mov        eax, dword ptr [0x4b0c90]
00420be2  mov        ecx, dword ptr [0x4b0c94]
00420be8  lea        edx, [ecx + eax*2]
00420beb  imul       edx, edx, 0x117d
00420bf1  add        edx, dword ptr [0x4b0ca8]
00420bf7  cmp        dword ptr [ebx + edx*4 + 0x598], 0x1869f
00420c02  lea        eax, [ebx + edx*4 + 0x598]
00420c09  jge        0x420ced

// block 00420c0f
00420c0f  inc        dword ptr [eax]
00420c11  jmp        0x420ced

// block 00420c16
00420c16  cmp        eax, 7
00420c19  jne        0x420cb1

// block 00420c1f
00420c1f  mov        ebx, dword ptr [0x4b43e4]
00420c25  or         dword ptr [ebx + 0x6d18], 0x20
00420c2c  mov        eax, 0x4b0c40
00420c31  call       0x421880

// block 00420c36
00420c36  mov        ecx, dword ptr [0x4b0ca0]
00420c3c  mov        esi, eax
00420c3e  mov        eax, dword ptr [0x4b0c98]
00420c43  lea        edx, [ecx + eax*2]
00420c46  imul       edx, edx, 0x96
00420c4c  add        edx, dword ptr [0x4b0c48]
00420c52  mov        ecx, 0x4b0c40
00420c57  imul       edx, edx, 0x64
00420c5a  add        esi, edx
00420c5c  imul       esi, esi, 0x64
00420c5f  push       esi
00420c60  call       0x40e730

// block 00420c65
00420c65  add        dword ptr [ebx + 0x6d48], esi
00420c6b  cmp        dword ptr [edi + 0x10], 1
00420c6f  je         0x420ac3

// block 00420c75
00420c75  call       0x433870

// block 00420c7a
00420c7a  mov        eax, dword ptr [0x4b0c90]
00420c7f  mov        ecx, dword ptr [0x4b0c94]
00420c85  lea        edx, [ecx + eax*2]
00420c88  mov        eax, dword ptr [0x4b451c]
00420c8d  imul       edx, edx, 0x117d
00420c93  add        edx, dword ptr [0x4b0ca8]
00420c99  cmp        dword ptr [eax + edx*4 + 0x598], 0x1869f
00420ca4  lea        eax, [eax + edx*4 + 0x598]
00420cab  jge        0x420ced

// block 00420cad
00420cad  inc        dword ptr [eax]
00420caf  jmp        0x420ced

// block 00420cb1
00420cb1  mov        ecx, 0xc
00420cb6  mov        eax, 0x4ce8e8
00420cbb  call       0x40f720

// block 00420cc0
00420cc0  call       0x4217e0

// block 00420cc5
00420cc5  jmp        0x420ced

// block 00420cc7
00420cc7  cmp        dword ptr [0x4b0cb0], 6
00420cce  push       ecx
00420ccf  je         0x420cd9

// block 00420cd1
00420cd1  fld        dword ptr [0x4a4014]
00420cd7  jmp        0x420ce5

// block 00420cd9
00420cd9  fld        dword ptr [0x4a3e54]
00420cdf  jmp        0x420ce5

// block 00420ce1
00420ce1  fld        dword ptr [eax + 4]
00420ce4  push       ecx

// block 00420ce5
00420ce5  fstp       dword ptr [esp]
00420ce8  call       0x430270

// block 00420ced
00420ced  mov        eax, dword ptr [ebp + 0x64]
00420cf0  movzx      ecx, byte ptr [eax + 3]
00420cf4  lea        edx, [ecx + eax + 4]
00420cf8  mov        dword ptr [ebp + 0x64], edx
00420cfb  mov        eax, edx
00420cfd  movzx      ecx, word ptr [eax]
00420d00  cmp        dword ptr [ebp + 0x1c], ecx
00420d03  jge        0x41fd40

// block 00420d09
00420d09  mov        edx, dword ptr [ebp + 0x1c]
00420d0c  mov        ecx, dword ptr [ebp + 0x24]
00420d0f  mov        dword ptr [ebp + 0x18], edx
00420d12  fld        dword ptr [ecx]
00420d14  fcomp      qword ptr [0x4a3db0]
00420d1a  fnstsw     ax
00420d1c  test       ah, 0x41
00420d1f  jne        0x420d6d

// block 00420d21
00420d21  fld        dword ptr [0x4a3dac]
00420d27  fcomp      dword ptr [ecx]
00420d29  fnstsw     ax
00420d2b  test       ah, 0x41
00420d2e  jne        0x420d6d

// block 00420d30
00420d30  fld        dword ptr [ebp + 0x20]
00420d33  inc        edx
00420d34  fadd       qword ptr [0x4a3ce0]
00420d3a  mov        dword ptr [ebp + 0x1c], edx
00420d3d  xor        eax, eax
00420d3f  fstp       dword ptr [ebp + 0x20]
00420d42  pop        edi
00420d43  pop        esi
00420d44  pop        ebp
00420d45  pop        ebx
00420d46  add        esp, 0x48
00420d49  ret        4

// block 00420d4c
00420d4c  xor        eax, eax
00420d4e  cmp        dword ptr [0x4b0cb8], eax
00420d54  je         0x420d5b

// block 00420d56
00420d56  mov        dword ptr [0x4b0cc0], eax

// block 00420d5b
00420d5b  mov        dword ptr [0x4b0cb8], eax
00420d60  or         eax, 0xffffffff
00420d63  pop        edi
00420d64  pop        esi
00420d65  pop        ebp
00420d66  pop        ebx
00420d67  add        esp, 0x48
00420d6a  ret        4

// block 00420d6d
00420d6d  fld        dword ptr [ecx]
00420d6f  fadd       dword ptr [ebp + 0x20]
00420d72  fstp       dword ptr [esp + 0x5c]
00420d76  fld        dword ptr [esp + 0x5c]
00420d7a  fst        dword ptr [ebp + 0x20]
00420d7d  call       0x4931e0

// block 00420d82
00420d82  mov        dword ptr [ebp + 0x1c], eax

// block 00420d85
00420d85  pop        edi
00420d86  pop        esi
00420d87  pop        ebp
00420d88  xor        eax, eax
00420d8a  pop        ebx
00420d8b  add        esp, 0x48
00420d8e  ret        4
