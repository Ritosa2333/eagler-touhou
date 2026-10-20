
// block 0048fea7
0048fea7  mov        edi, edi
0048fea9  push       ebp
0048feaa  mov        ebp, esp
0048feac  mov        eax, dword ptr [ebp + 0xc]
0048feaf  sub        esp, 0x10
0048feb2  push       ebx
0048feb3  push       esi
0048feb4  xor        edx, edx
0048feb6  push       edi
0048feb7  mov        edi, 0x300
0048febc  cmp        dword ptr [ebp + 0x10], edx
0048febf  je         0x490018

// block 0048fec5
0048fec5  wait       
0048fec6  fnstcw     word ptr [ebp - 8]
0048fec9  mov        ebx, dword ptr [ebp - 8]
0048fecc  test       bl, 1
0048fecf  je         0x48fed4

// block 0048fed1
0048fed1  push       0x10
0048fed3  pop        edx

// block 0048fed4
0048fed4  test       bl, 4
0048fed7  je         0x48fedc

// block 0048fed9
0048fed9  or         edx, 8

// block 0048fedc
0048fedc  test       bl, 8
0048fedf  je         0x48fee4

// block 0048fee1
0048fee1  or         edx, 4

// block 0048fee4
0048fee4  test       bl, 0x10
0048fee7  je         0x48feec

// block 0048fee9
0048fee9  or         edx, 2

// block 0048feec
0048feec  test       bl, 0x20
0048feef  je         0x48fef4

// block 0048fef1
0048fef1  or         edx, 1

// block 0048fef4
0048fef4  test       bl, 2
0048fef7  je         0x48feff

// block 0048fef9
0048fef9  or         edx, 0x80000

// block 0048feff
0048feff  movzx      eax, bx
0048ff02  mov        ecx, eax
0048ff04  mov        esi, 0xc00
0048ff09  and        ecx, esi
0048ff0b  je         0x48ff33

// block 0048ff0d
0048ff0d  cmp        ecx, 0x400
0048ff13  je         0x48ff2d

// block 0048ff15
0048ff15  cmp        ecx, 0x800
0048ff1b  je         0x48ff25

// block 0048ff1d
0048ff1d  cmp        ecx, esi
0048ff1f  jne        0x48ff33

// block 0048ff21
0048ff21  or         edx, edi
0048ff23  jmp        0x48ff33

// block 0048ff25
0048ff25  or         edx, 0x200
0048ff2b  jmp        0x48ff33

// block 0048ff2d
0048ff2d  or         edx, 0x100

// block 0048ff33
0048ff33  and        eax, edi
0048ff35  je         0x48ff46

// block 0048ff37
0048ff37  cmp        eax, 0x200
0048ff3c  jne        0x48ff4c

// block 0048ff3e
0048ff3e  or         edx, 0x10000
0048ff44  jmp        0x48ff4c

// block 0048ff46
0048ff46  or         edx, 0x20000

// block 0048ff4c
0048ff4c  test       ebx, 0x1000
0048ff52  je         0x48ff5a

// block 0048ff54
0048ff54  or         edx, 0x40000

// block 0048ff5a
0048ff5a  mov        ebx, dword ptr [ebp + 0xc]
0048ff5d  mov        eax, dword ptr [ebp + 8]
0048ff60  and        eax, dword ptr [ebp + 0xc]
0048ff63  not        ebx
0048ff65  and        ebx, edx
0048ff67  or         ebx, eax
0048ff69  cmp        ebx, edx
0048ff6b  je         0x490010

// block 0048ff71
0048ff71  call       0x48f849

// block 0048ff76
0048ff76  movzx      eax, ax
0048ff79  mov        dword ptr [ebp - 4], eax
0048ff7c  fldcw      word ptr [ebp - 4]
0048ff7f  wait       
0048ff80  fnstcw     word ptr [ebp - 4]
0048ff83  mov        edx, dword ptr [ebp - 4]
0048ff86  xor        ebx, ebx
0048ff88  test       dl, 1
0048ff8b  je         0x48ff90

// block 0048ff8d
0048ff8d  push       0x10
0048ff8f  pop        ebx

// block 0048ff90
0048ff90  test       dl, 4
0048ff93  je         0x48ff98

// block 0048ff95
0048ff95  or         ebx, 8

// block 0048ff98
0048ff98  test       dl, 8
0048ff9b  je         0x48ffa0

// block 0048ff9d
0048ff9d  or         ebx, 4

// block 0048ffa0
0048ffa0  test       dl, 0x10
0048ffa3  je         0x48ffa8

// block 0048ffa5
0048ffa5  or         ebx, 2

// block 0048ffa8
0048ffa8  test       dl, 0x20
0048ffab  je         0x48ffb0

// block 0048ffad
0048ffad  or         ebx, 1

// block 0048ffb0
0048ffb0  test       dl, 2
0048ffb3  je         0x48ffbb

// block 0048ffb5
0048ffb5  or         ebx, 0x80000

// block 0048ffbb
0048ffbb  movzx      ecx, dx
0048ffbe  mov        eax, ecx
0048ffc0  and        eax, esi
0048ffc2  je         0x48ffe8

// block 0048ffc4
0048ffc4  cmp        eax, 0x400
0048ffc9  je         0x48ffe2

// block 0048ffcb
0048ffcb  cmp        eax, 0x800
0048ffd0  je         0x48ffda

// block 0048ffd2
0048ffd2  cmp        eax, esi
0048ffd4  jne        0x48ffe8

// block 0048ffd6
0048ffd6  or         ebx, edi
0048ffd8  jmp        0x48ffe8

// block 0048ffda
0048ffda  or         ebx, 0x200
0048ffe0  jmp        0x48ffe8

// block 0048ffe2
0048ffe2  or         ebx, 0x100

// block 0048ffe8
0048ffe8  and        ecx, edi
0048ffea  je         0x48fffc

// block 0048ffec
0048ffec  cmp        ecx, 0x200
0048fff2  jne        0x490002

// block 0048fff4
0048fff4  or         ebx, 0x10000
0048fffa  jmp        0x490002

// block 0048fffc
0048fffc  or         ebx, 0x20000

// block 00490002
00490002  test       edx, 0x1000
00490008  je         0x490010

// block 0049000a
0049000a  or         ebx, 0x40000

// block 00490010
00490010  mov        eax, dword ptr [ebp + 0x10]
00490013  mov        dword ptr [eax], ebx
00490015  mov        eax, dword ptr [ebp + 0xc]

// block 00490018
00490018  cmp        dword ptr [ebp + 0x14], 0
0049001c  je         0x4901b3

// block 00490022
00490022  xor        esi, esi
00490024  cmp        dword ptr [0x4d52dc], esi
0049002a  je         0x4901ae

// block 00490030
00490030  and        eax, 0x308031f
00490035  mov        dword ptr [ebp - 0x10], eax
00490038  stmxcsr    dword ptr [ebp - 0xc]
0049003c  mov        eax, dword ptr [ebp - 0xc]
0049003f  test       al, al
00490041  jns        0x490046

// block 00490043
00490043  push       0x10
00490045  pop        esi

// block 00490046
00490046  test       eax, 0x200
0049004b  je         0x490050

// block 0049004d
0049004d  or         esi, 8

// block 00490050
00490050  test       eax, 0x400
00490055  je         0x49005a

// block 00490057
00490057  or         esi, 4

// block 0049005a
0049005a  test       eax, 0x800
0049005f  je         0x490064

// block 00490061
00490061  or         esi, 2

// block 00490064
00490064  test       eax, 0x1000
00490069  je         0x49006e

// block 0049006b
0049006b  or         esi, 1

// block 0049006e
0049006e  test       eax, 0x100
00490073  je         0x49007b

// block 00490075
00490075  or         esi, 0x80000

// block 0049007b
0049007b  mov        ecx, eax
0049007d  mov        edi, 0x6000
00490082  and        ecx, edi
00490084  je         0x4900b0

// block 00490086
00490086  cmp        ecx, 0x2000
0049008c  je         0x4900aa

// block 0049008e
0049008e  cmp        ecx, 0x4000
00490094  je         0x4900a2

// block 00490096
00490096  cmp        ecx, edi
00490098  jne        0x4900b0

// block 0049009a
0049009a  or         esi, 0x300
004900a0  jmp        0x4900b0

// block 004900a2
004900a2  or         esi, 0x200
004900a8  jmp        0x4900b0

// block 004900aa
004900aa  or         esi, 0x100

// block 004900b0
004900b0  mov        ebx, 0x8040
004900b5  and        eax, ebx
004900b7  sub        eax, 0x40
004900ba  je         0x4900d8

// block 004900bc
004900bc  sub        eax, 0x7fc0
004900c1  je         0x4900d0

// block 004900c3
004900c3  sub        eax, 0x40
004900c6  jne        0x4900de

// block 004900c8
004900c8  or         esi, 0x1000000
004900ce  jmp        0x4900de

// block 004900d0
004900d0  or         esi, 0x3000000
004900d6  jmp        0x4900de

// block 004900d8
004900d8  or         esi, 0x2000000

// block 004900de
004900de  mov        eax, dword ptr [ebp - 0x10]
004900e1  mov        edx, eax
004900e3  and        eax, dword ptr [ebp + 8]
004900e6  not        edx
004900e8  and        edx, esi
004900ea  or         edx, eax
004900ec  cmp        edx, esi
004900ee  jne        0x4900f7

// block 004900f0
004900f0  mov        eax, esi
004900f2  jmp        0x4901a7

// block 004900f7
004900f7  call       0x48f9ba

// block 004900fc
004900fc  push       eax
004900fd  mov        dword ptr [ebp + 0x10], eax
00490100  call       0x491220

// block 00490105
00490105  pop        ecx
00490106  stmxcsr    dword ptr [ebp + 0x10]
0049010a  mov        ecx, dword ptr [ebp + 0x10]
0049010d  xor        edx, edx
0049010f  test       cl, cl
00490111  jns        0x490116

// block 00490113
00490113  push       0x10
00490115  pop        edx

// block 00490116
00490116  test       ecx, 0x200
0049011c  je         0x490121

// block 0049011e
0049011e  or         edx, 8

// block 00490121
00490121  test       ecx, 0x400
00490127  je         0x49012c

// block 00490129
00490129  or         edx, 4

// block 0049012c
0049012c  test       ecx, 0x800
00490132  je         0x490137

// block 00490134
00490134  or         edx, 2

// block 00490137
00490137  test       ecx, 0x1000
0049013d  je         0x490142

// block 0049013f
0049013f  or         edx, 1

// block 00490142
00490142  mov        esi, 0x100
00490147  test       esi, ecx
00490149  je         0x490151

// block 0049014b
0049014b  or         edx, 0x80000

// block 00490151
00490151  mov        eax, ecx
00490153  and        eax, edi
00490155  je         0x49017b

// block 00490157
00490157  cmp        eax, 0x2000
0049015c  je         0x490179

// block 0049015e
0049015e  cmp        eax, 0x4000
00490163  je         0x490171

// block 00490165
00490165  cmp        eax, edi
00490167  jne        0x49017b

// block 00490169
00490169  or         edx, 0x300
0049016f  jmp        0x49017b

// block 00490171
00490171  or         edx, 0x200
00490177  jmp        0x49017b

// block 00490179
00490179  or         edx, esi

// block 0049017b
0049017b  and        ecx, ebx
0049017d  sub        ecx, 0x40
00490180  je         0x49019f

// block 00490182
00490182  sub        ecx, 0x7fc0
00490188  je         0x490197

// block 0049018a
0049018a  sub        ecx, 0x40
0049018d  jne        0x4901a5

// block 0049018f
0049018f  or         edx, 0x1000000
00490195  jmp        0x4901a5

// block 00490197
00490197  or         edx, 0x3000000
0049019d  jmp        0x4901a5

// block 0049019f
0049019f  or         edx, 0x2000000

// block 004901a5
004901a5  mov        eax, edx

// block 004901a7
004901a7  mov        ecx, dword ptr [ebp + 0x14]
004901aa  mov        dword ptr [ecx], eax
004901ac  jmp        0x4901b3

// block 004901ae
004901ae  mov        eax, dword ptr [ebp + 0x14]
004901b1  mov        dword ptr [eax], esi

// block 004901b3
004901b3  pop        edi
004901b4  xor        eax, eax
004901b6  pop        esi
004901b7  inc        eax
004901b8  pop        ebx
004901b9  leave      
004901ba  ret        
