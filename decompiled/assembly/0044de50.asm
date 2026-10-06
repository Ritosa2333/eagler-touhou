
// block 0044de50
0044de50  sub        esp, 0x88
0044de56  push       ebp
0044de57  mov        ebp, dword ptr [esp + 0x94]
0044de5e  test       edi, edi
0044de60  jne        0x44de6e

// block 0044de62
0044de62  xor        al, al
0044de64  pop        ebp
0044de65  add        esp, 0x88
0044de6b  ret        0x14

// block 0044de6e
0044de6e  mov        ecx, dword ptr [eax + 0x104]
0044de74  mov        edx, dword ptr [eax + 0x108]
0044de7a  push       ebx
0044de7b  push       esi
0044de7c  mov        esi, dword ptr [eax + 0x114]
0044de82  mov        eax, dword ptr [esp + 0xa0]
0044de89  push       eax
0044de8a  push       esi
0044de8b  mov        dword ptr [esp + 0x18], ecx
0044de8f  mov        dword ptr [esp + 0x1c], edx
0044de93  call       dword ptr [0x49802c]

// block 0044de99
0044de99  push       1
0044de9b  push       esi
0044de9c  mov        dword ptr [esp + 0x24], eax
0044dea0  call       dword ptr [0x49801c]

// block 0044dea6
0044dea6  mov        eax, dword ptr [esp + 0xa8]
0044dead  mov        ebx, dword ptr [0x498208]
0044deb3  cmp        eax, -1
0044deb6  je         0x44dffa

// block 0044debc
0044debc  push       eax
0044debd  push       esi
0044debe  call       dword ptr [0x498018]

// block 0044dec4
0044dec4  mov        eax, dword ptr [esp + 0x98]
0044decb  mov        ecx, dword ptr [esp + 0x10]
0044decf  mov        edx, dword ptr [esp + 0x14]
0044ded3  mov        dword ptr [esp + 0x24], eax
0044ded7  add        eax, ecx
0044ded9  mov        dword ptr [esp + 0xc], eax
0044dedd  add        eax, -2
0044dee0  mov        dword ptr [esp + 0x2c], eax
0044dee4  lea        eax, [edx + ebp]
0044dee7  add        eax, -2
0044deea  push       0
0044deec  mov        dword ptr [esp + 0x34], eax
0044def0  lea        eax, [esp + 0x28]
0044def4  push       eax
0044def5  push       -1
0044def7  push       edi
0044def8  push       esi
0044def9  mov        dword ptr [esp + 0x3c], ebp
0044defd  call       ebx

// block 0044deff
0044deff  mov        edx, dword ptr [esp + 0x14]
0044df03  mov        eax, dword ptr [esp + 0x98]
0044df0a  add        edx, ebp
0044df0c  mov        dword ptr [esp + 0x18], edx
0044df10  mov        dword ptr [esp + 0x50], edx
0044df14  mov        edx, dword ptr [esp + 0x98]
0044df1b  inc        edx
0044df1c  mov        dword ptr [esp + 0x54], edx
0044df20  mov        ecx, dword ptr [esp + 0xc]
0044df24  lea        edx, [ebp + 2]
0044df27  mov        dword ptr [esp + 0x58], edx
0044df2b  mov        edx, dword ptr [esp + 0xc]
0044df2f  inc        edx
0044df30  mov        dword ptr [esp + 0x5c], edx
0044df34  mov        edx, dword ptr [esp + 0x18]
0044df38  add        edx, 2
0044df3b  add        eax, 2
0044df3e  mov        dword ptr [esp + 0x20], edx
0044df42  mov        dword ptr [esp + 0x60], edx
0044df46  mov        edx, dword ptr [esp + 0x18]
0044df4a  mov        dword ptr [esp + 0x70], edx
0044df4e  mov        edx, dword ptr [esp + 0x98]
0044df55  mov        dword ptr [esp + 0x44], eax
0044df59  mov        dword ptr [esp + 0x64], eax
0044df5d  mov        dword ptr [esp + 0x84], eax
0044df64  lea        eax, [ebp + 2]
0044df67  push       0
0044df69  mov        dword ptr [esp + 0x78], edx
0044df6d  mov        dword ptr [esp + 0x8c], eax
0044df74  lea        eax, [esp + 0x48]
0044df78  push       eax
0044df79  lea        edx, [ebp + 2]
0044df7c  mov        dword ptr [esp + 0x80], edx
0044df83  mov        edx, dword ptr [esp + 0x14]
0044df87  push       -1
0044df89  add        ecx, 2
0044df8c  mov        dword ptr [esp + 0x88], edx
0044df93  mov        edx, dword ptr [esp + 0x2c]
0044df97  push       edi
0044df98  push       esi
0044df99  mov        dword ptr [esp + 0x5c], ebp
0044df9d  mov        dword ptr [esp + 0x60], ecx
0044dfa1  mov        dword ptr [esp + 0x7c], ebp
0044dfa5  mov        dword ptr [esp + 0x80], ecx
0044dfac  mov        dword ptr [esp + 0x94], edx
0044dfb3  mov        dword ptr [esp + 0xa0], ecx
0044dfba  mov        dword ptr [esp + 0xa4], edx
0044dfc1  call       ebx

// block 0044dfc3
0044dfc3  push       0
0044dfc5  lea        ecx, [esp + 0x58]
0044dfc9  push       ecx
0044dfca  push       -1
0044dfcc  push       edi
0044dfcd  push       esi
0044dfce  call       ebx

// block 0044dfd0
0044dfd0  push       0
0044dfd2  lea        edx, [esp + 0x68]
0044dfd6  push       edx
0044dfd7  push       -1
0044dfd9  push       edi
0044dfda  push       esi
0044dfdb  call       ebx

// block 0044dfdd
0044dfdd  push       0
0044dfdf  lea        eax, [esp + 0x78]
0044dfe3  push       eax
0044dfe4  push       -1
0044dfe6  push       edi
0044dfe7  push       esi
0044dfe8  call       ebx

// block 0044dfea
0044dfea  push       0
0044dfec  lea        ecx, [esp + 0x88]
0044dff3  push       ecx
0044dff4  push       -1
0044dff6  push       edi
0044dff7  push       esi
0044dff8  call       ebx

// block 0044dffa
0044dffa  mov        edx, dword ptr [esp + 0xa4]
0044e001  push       edx
0044e002  push       esi
0044e003  call       dword ptr [0x498018]

// block 0044e009
0044e009  mov        eax, dword ptr [esp + 0x98]
0044e010  lea        ecx, [eax + 1]
0044e013  mov        dword ptr [esp + 0x34], ecx
0044e017  mov        ecx, dword ptr [esp + 0x10]
0044e01b  lea        edx, [ebp + 1]
0044e01e  mov        dword ptr [esp + 0x38], edx
0044e022  lea        edx, [ecx + eax + 1]
0044e026  mov        eax, dword ptr [esp + 0x14]
0044e02a  push       0
0044e02c  mov        dword ptr [esp + 0x40], edx
0044e030  lea        edx, [esp + 0x38]
0044e034  push       edx
0044e035  push       -1
0044e037  push       edi
0044e038  lea        ecx, [eax + ebp + 1]
0044e03c  push       esi
0044e03d  mov        dword ptr [esp + 0x54], ecx
0044e041  call       ebx

// block 0044e043
0044e043  mov        eax, dword ptr [esp + 0x1c]
0044e047  push       eax
0044e048  push       esi
0044e049  call       dword ptr [0x49802c]

// block 0044e04f
0044e04f  pop        esi
0044e050  pop        ebx
0044e051  mov        al, 1
0044e053  pop        ebp
0044e054  add        esp, 0x88
0044e05a  ret        0x14
