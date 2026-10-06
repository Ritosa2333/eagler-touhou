
// block 0043dec0
0043dec0  sub        esp, 0x18
0043dec3  test       byte ptr [0x4ceae8], 4
0043deca  push       ebx
0043decb  push       ebp
0043decc  mov        ebp, dword ptr [esp + 0x24]
0043ded0  push       esi
0043ded1  push       edi
0043ded2  lea        edi, [ebp + 0x4cc]
0043ded8  mov        dword ptr [esp + 0x18], edi
0043dedc  jne        0x43df10

// block 0043dede
0043dede  cmp        dword ptr [0x4cf278], 0
0043dee5  je         0x43df10

// block 0043dee7
0043dee7  mov        esi, dword ptr [0x4ce8cc]
0043deed  call       0x45a3c0

// block 0043def2
0043def2  mov        eax, dword ptr [0x4ce8f0]
0043def7  push       0
0043def9  mov        dword ptr [0x4cf278], 0
0043df03  mov        ecx, dword ptr [eax]
0043df05  mov        edx, dword ptr [ecx + 0xe4]
0043df0b  push       0x1c
0043df0d  push       eax
0043df0e  call       edx

// block 0043df10
0043df10  mov        esi, dword ptr [0x4b4514]
0043df16  mov        dword ptr [esp + 0x20], 0xd
0043df1e  mov        edi, edi

// block 0043df20
0043df20  cmp        byte ptr [edi + 0x38], 0
0043df24  je         0x43e207

// block 0043df2a
0043df2a  cmp        dword ptr [edi + 0x20], 8
0043df2e  jge        0x43df3b

// block 0043df30
0043df30  fld        dword ptr [edi + 0x24]
0043df33  fdivr      qword ptr [0x4a4078]
0043df39  jmp        0x43df41

// block 0043df3b
0043df3b  fld        dword ptr [0x4a3e54]

// block 0043df41
0043df41  movzx      eax, byte ptr [edi + 0x39]
0043df45  fstp       dword ptr [esp + 0x10]
0043df49  fld        dword ptr [edi + 0xc]
0043df4c  mov        dword ptr [esp + 0x2c], eax
0043df50  fild       dword ptr [esp + 0x2c]
0043df54  fmul       dword ptr [esp + 0x10]
0043df58  fmul       qword ptr [0x4a3cf8]
0043df5e  fsubp      st(1)
0043df60  fadd       qword ptr [0x4a3d70]
0043df66  fadd       qword ptr [0x4a3d28]
0043df6c  fstp       dword ptr [ebp + 0x448]
0043df72  fld        dword ptr [edi + 0x10]
0043df75  fadd       qword ptr [0x4a3d68]
0043df7b  fstp       dword ptr [ebp + 0x44c]
0043df81  mov        ecx, dword ptr [edi + 0x18]
0043df84  mov        dword ptr [ebp + 0x3d4], ecx
0043df8a  fld        dword ptr [esi + 0x97c]
0043df90  fsub       dword ptr [edi + 0xc]
0043df93  fstp       dword ptr [esp + 0x1c]
0043df97  fld        dword ptr [esi + 0x980]
0043df9d  fsub       dword ptr [edi + 0x10]
0043dfa0  fstp       dword ptr [esp + 0x2c]
0043dfa4  fld        dword ptr [esp + 0x2c]
0043dfa8  fld        dword ptr [esp + 0x1c]
0043dfac  fmul       st(0), st(0)
0043dfae  fld        st(1)
0043dfb0  fmulp      st(2)
0043dfb2  faddp      st(1)
0043dfb4  call       0x4931e0

// block 0043dfb9
0043dfb9  cmp        eax, 0x4000
0043dfbe  jle        0x43dfcb

// block 0043dfc0
0043dfc0  mov        ecx, 0xd0
0043dfc5  mov        dword ptr [esp + 0x2c], ecx
0043dfc9  jmp        0x43e001

// block 0043dfcb
0043dfcb  cmp        eax, 0x1000
0043dfd0  jle        0x43dff5

// block 0043dfd2
0043dfd2  lea        ecx, [eax + eax*4 - 0x5000]
0043dfd9  shl        ecx, 5
0043dfdc  mov        eax, 0x2aaaaaab
0043dfe1  imul       ecx
0043dfe3  sar        edx, 0xb
0043dfe6  mov        eax, edx
0043dfe8  shr        eax, 0x1f
0043dfeb  lea        ecx, [edx + eax + 0x30]
0043dfef  mov        dword ptr [esp + 0x2c], ecx
0043dff3  jmp        0x43dffd

// block 0043dff5
0043dff5  mov        dword ptr [esp + 0x2c], 0x30

// block 0043dffd
0043dffd  mov        ecx, dword ptr [esp + 0x2c]

// block 0043e001
0043e001  movzx      eax, byte ptr [edi + 0x39]
0043e005  lea        edx, [eax + edi - 1]
0043e009  mov        dword ptr [esp + 0x14], edx
0043e00d  mov        dword ptr [esp + 0x1c], eax
0043e011  test       eax, eax
0043e013  jle        0x43e207

// block 0043e019
0043e019  lea        esi, [ebp + 0x18]
0043e01c  jmp        0x43e028

// block 0043e020
0043e020  mov        ecx, dword ptr [esp + 0x2c]
0043e024  mov        edx, dword ptr [esp + 0x14]

// block 0043e028
0043e028  mov        edi, dword ptr [edi + 0x20]
0043e02b  cmp        edi, 0x34
0043e02e  jl         0x43e11f

// block 0043e034
0043e034  mov        al, byte ptr [edx]
0043e036  cmp        al, 0xa
0043e038  je         0x43e11f

// block 0043e03e
0043e03e  cmp        edi, 0x38
0043e041  movzx      eax, al
0043e044  jge        0x43e0b4

// block 0043e046
0043e046  add        eax, 0xcf
0043e04b  lea        edx, [eax + eax*8]
0043e04e  mov        eax, dword ptr [ebp + 0x10]
0043e051  mov        eax, dword ptr [eax + 0x118]
0043e057  lea        eax, [eax + edx*8]
0043e05a  mov        dword ptr [esi + 0x3f4], eax
0043e060  fld        dword ptr [eax + 0x24]
0043e063  fstp       dword ptr [esp + 0x24]
0043e067  mov        edx, eax
0043e069  fld        dword ptr [esp + 0x24]
0043e06d  fst        dword ptr [esi + 0x8c]
0043e073  fstp       dword ptr [esi + 0x7c]
0043e076  fld        dword ptr [edx + 0x2c]
0043e079  fstp       dword ptr [esp + 0x24]
0043e07d  fld        dword ptr [esp + 0x24]
0043e081  fst        dword ptr [esi + 0x94]
0043e087  fstp       dword ptr [esi + 0x84]
0043e08d  fld        dword ptr [eax + 0x28]
0043e090  fstp       dword ptr [esp + 0x24]
0043e094  fld        dword ptr [esp + 0x24]
0043e098  fst        dword ptr [esi + 0x88]
0043e09e  fstp       dword ptr [esi + 0x80]
0043e0a4  fld        dword ptr [edx + 0x30]
0043e0a7  fstp       dword ptr [esp + 0x24]
0043e0ab  fld        dword ptr [esp + 0x24]
0043e0af  jmp        0x43e18b

// block 0043e0b4
0043e0b4  mov        edx, dword ptr [ebp + 0x10]
0043e0b7  mov        edx, dword ptr [edx + 0x118]
0043e0bd  add        eax, 0xd9
0043e0c2  lea        eax, [eax + eax*8]
0043e0c5  lea        eax, [edx + eax*8]
0043e0c8  mov        dword ptr [esi + 0x3f4], eax
0043e0ce  fld        dword ptr [eax + 0x24]
0043e0d1  fstp       dword ptr [esp + 0x24]
0043e0d5  mov        edx, eax
0043e0d7  fld        dword ptr [esp + 0x24]
0043e0db  fst        dword ptr [esi + 0x8c]
0043e0e1  fstp       dword ptr [esi + 0x7c]
0043e0e4  fld        dword ptr [eax + 0x2c]
0043e0e7  fstp       dword ptr [esp + 0x24]
0043e0eb  fld        dword ptr [esp + 0x24]
0043e0ef  fst        dword ptr [esi + 0x94]
0043e0f5  fstp       dword ptr [esi + 0x84]
0043e0fb  fld        dword ptr [edx + 0x28]
0043e0fe  fstp       dword ptr [esp + 0x24]
0043e102  fld        dword ptr [esp + 0x24]
0043e106  fst        dword ptr [esi + 0x88]
0043e10c  fstp       dword ptr [esi + 0x80]
0043e112  fld        dword ptr [eax + 0x30]
0043e115  fstp       dword ptr [esp + 0x24]
0043e119  fld        dword ptr [esp + 0x24]
0043e11d  jmp        0x43e18b

// block 0043e11f
0043e11f  movzx      eax, byte ptr [edx]
0043e122  add        eax, 0xc4
0043e127  lea        edx, [eax + eax*8]
0043e12a  mov        eax, dword ptr [ebp + 0x10]
0043e12d  mov        eax, dword ptr [eax + 0x118]
0043e133  lea        eax, [eax + edx*8]
0043e136  mov        dword ptr [esi + 0x3f4], eax
0043e13c  mov        edx, eax
0043e13e  fld        dword ptr [eax + 0x24]
0043e141  fstp       dword ptr [esp + 0x24]
0043e145  fld        dword ptr [esp + 0x24]
0043e149  fst        dword ptr [esi + 0x8c]
0043e14f  fstp       dword ptr [esi + 0x7c]
0043e152  fld        dword ptr [edx + 0x2c]
0043e155  fstp       dword ptr [esp + 0x24]
0043e159  fld        dword ptr [esp + 0x24]
0043e15d  fst        dword ptr [esi + 0x94]
0043e163  fstp       dword ptr [esi + 0x84]
0043e169  fld        dword ptr [eax + 0x28]
0043e16c  fstp       dword ptr [esp + 0x24]
0043e170  fld        dword ptr [esp + 0x24]
0043e174  fst        dword ptr [esi + 0x88]
0043e17a  fstp       dword ptr [esi + 0x80]
0043e180  fld        dword ptr [edx + 0x30]
0043e183  fstp       dword ptr [esp + 0x24]
0043e187  fld        dword ptr [esp + 0x24]

// block 0043e18b
0043e18b  fst        dword ptr [esi + 0x98]
0043e191  push       0x4d4804
0043e196  fstp       dword ptr [esi + 0x90]
0043e19c  mov        eax, dword ptr [ebp + 0x40c]
0043e1a2  mov        byte ptr [ebp + 0x3d7], cl
0043e1a8  fld        dword ptr [eax + 0x38]
0043e1ab  or         dword ptr [esi + 0x47c], 8
0043e1b2  fstp       dword ptr [esi + 0x58]
0043e1b5  push       0x4d47e8
0043e1ba  mov        edi, 0x4d483c
0043e1bf  mov        ebx, 0x4d4820
0043e1c4  call       0x45a570

// block 0043e1c9
0043e1c9  mov        ecx, dword ptr [0x4ce8cc]
0043e1cf  push       1
0043e1d1  mov        eax, esi
0043e1d3  call       0x459e50

// block 0043e1d8
0043e1d8  fld        dword ptr [ebp + 0x448]
0043e1de  mov        eax, dword ptr [esp + 0x1c]
0043e1e2  fadd       dword ptr [esp + 0x10]
0043e1e6  dec        dword ptr [esp + 0x14]
0043e1ea  mov        edi, dword ptr [esp + 0x18]
0043e1ee  dec        eax
0043e1ef  fstp       dword ptr [ebp + 0x448]
0043e1f5  mov        dword ptr [esp + 0x1c], eax
0043e1f9  test       eax, eax
0043e1fb  jg         0x43e020

// block 0043e201
0043e201  mov        esi, dword ptr [0x4b4514]

// block 0043e207
0043e207  add        edi, 0x40
0043e20a  sub        dword ptr [esp + 0x20], 1
0043e20f  mov        dword ptr [esp + 0x18], edi
0043e213  jne        0x43df20

// block 0043e219
0043e219  pop        edi
0043e21a  pop        esi
0043e21b  pop        ebp
0043e21c  mov        eax, 1
0043e221  pop        ebx
0043e222  add        esp, 0x18
0043e225  ret        4
