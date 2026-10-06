
// block 0045dcd0
0045dcd0  sub        esp, 0x18
0045dcd3  fld        dword ptr [0x4a3dc8]
0045dcd9  mov        edx, ecx
0045dcdb  fstp       dword ptr [esp]
0045dcde  push       ebx
0045dcdf  fld        dword ptr [edx + 0x430]
0045dce5  push       ebp
0045dce6  fadd       dword ptr [edx + 0x424]
0045dcec  mov        ebp, dword ptr [edx + 0x478]
0045dcf2  push       esi
0045dcf3  push       edi
0045dcf4  fstp       dword ptr [esp + 0x1c]
0045dcf8  mov        eax, dword ptr [esp + 0x1c]
0045dcfc  fld        dword ptr [edx + 0x434]
0045dd02  fadd       dword ptr [edx + 0x428]
0045dd08  fstp       dword ptr [esp + 0x20]
0045dd0c  mov        ecx, dword ptr [esp + 0x20]
0045dd10  fld        dword ptr [edx + 0x438]
0045dd16  fadd       dword ptr [edx + 0x42c]
0045dd1c  mov        dword ptr [ebp], eax
0045dd1f  mov        dword ptr [ebp + 4], ecx
0045dd22  fstp       dword ptr [esp + 0x24]
0045dd26  mov        eax, dword ptr [esp + 0x24]
0045dd2a  fld        dword ptr [ebp + 0x4a4]
0045dd30  mov        dword ptr [ebp + 8], eax
0045dd33  fadd       dword ptr [ebp + 0x14]
0045dd36  fstp       dword ptr [esp + 0x14]
0045dd3a  fld        dword ptr [esp + 0x14]
0045dd3e  fst        dword ptr [ebp + 0x14]
0045dd41  fldz       
0045dd43  fcom       st(1)
0045dd45  fnstsw     ax
0045dd47  fstp       st(1)
0045dd49  fld1       
0045dd4b  test       ah, 0x41
0045dd4e  jne        0x45dd78

// block 0045dd50
0045dd50  lea        eax, [ebp + 0x30]
0045dd53  mov        ecx, 0xb

// block 0045dd58
0045dd58  fld        dword ptr [eax - 0x1c]
0045dd5b  add        eax, 0x54
0045dd5e  sub        ecx, 1
0045dd61  fadd       st(1)
0045dd63  fstp       dword ptr [eax - 0x70]
0045dd66  fld        dword ptr [eax - 0x54]
0045dd69  fadd       st(1)
0045dd6b  fstp       dword ptr [eax - 0x54]
0045dd6e  fld        dword ptr [eax - 0x38]
0045dd71  fadd       st(1)
0045dd73  fstp       dword ptr [eax - 0x38]
0045dd76  jne        0x45dd58

// block 0045dd78
0045dd78  fld        dword ptr [ebp + 0x4a4]
0045dd7e  fadd       dword ptr [ebp + 0x18]
0045dd81  fstp       dword ptr [esp + 0x14]
0045dd85  fld        dword ptr [esp + 0x14]
0045dd89  fst        dword ptr [ebp + 0x18]
0045dd8c  fcomp      st(2)
0045dd8e  fnstsw     ax
0045dd90  test       ah, 5
0045dd93  jp         0x45ddbd

// block 0045dd95
0045dd95  lea        eax, [ebp + 0x34]
0045dd98  mov        ecx, 0xb

// block 0045dd9d
0045dd9d  fld        dword ptr [eax - 0x1c]
0045dda0  add        eax, 0x54
0045dda3  sub        ecx, 1
0045dda6  fadd       st(1)
0045dda8  fstp       dword ptr [eax - 0x70]
0045ddab  fld        dword ptr [eax - 0x54]
0045ddae  fadd       st(1)
0045ddb0  fstp       dword ptr [eax - 0x54]
0045ddb3  fld        dword ptr [eax - 0x38]
0045ddb6  fadd       st(1)
0045ddb8  fstp       dword ptr [eax - 0x38]
0045ddbb  jne        0x45dd9d

// block 0045ddbd
0045ddbd  mov        ecx, dword ptr [edx + 0x3bc]
0045ddc3  lea        edi, [ebp + 0x1c]
0045ddc6  mov        dword ptr [ebp + 0x10], ecx
0045ddc9  mov        dword ptr [esp + 0x18], edi
0045ddcd  lea        esi, [ebp + 0x3a0]
0045ddd3  mov        ebx, 0x1f
0045ddd8  jmp        0x45dde4

// block 0045dde0
0045dde0  fldz       
0045dde2  fld1       

// block 0045dde4
0045dde4  fld        dword ptr [ebp + 0x4a4]
0045ddea  fadd       dword ptr [edi + 0x14]
0045dded  fstp       dword ptr [esp + 0x14]
0045ddf1  fld        dword ptr [esp + 0x14]
0045ddf5  fst        dword ptr [edi + 0x14]
0045ddf8  fcomp      st(2)
0045ddfa  fnstsw     ax
0045ddfc  test       ah, 5
0045ddff  jp         0x45de29

// block 0045de01
0045de01  lea        eax, [ebp + 0x30]
0045de04  mov        ecx, 0xb

// block 0045de09
0045de09  fld        dword ptr [eax - 0x1c]
0045de0c  add        eax, 0x54
0045de0f  sub        ecx, 1
0045de12  fadd       st(1)
0045de14  fstp       dword ptr [eax - 0x70]
0045de17  fld        dword ptr [eax - 0x54]
0045de1a  fadd       st(1)
0045de1c  fstp       dword ptr [eax - 0x54]
0045de1f  fld        dword ptr [eax - 0x38]
0045de22  fadd       st(1)
0045de24  fstp       dword ptr [eax - 0x38]
0045de27  jne        0x45de09

// block 0045de29
0045de29  fld        dword ptr [ebp + 0x4a4]
0045de2f  fadd       dword ptr [edi + 0x18]
0045de32  fstp       dword ptr [esp + 0x14]
0045de36  fld        dword ptr [esp + 0x14]
0045de3a  fst        dword ptr [edi + 0x18]
0045de3d  fcomp      st(2)
0045de3f  fnstsw     ax
0045de41  fstp       st(1)
0045de43  test       ah, 5
0045de46  jp         0x45de70

// block 0045de48
0045de48  lea        eax, [ebp + 0x34]
0045de4b  mov        ecx, 0xb

// block 0045de50
0045de50  fld        dword ptr [eax - 0x1c]
0045de53  add        eax, 0x54
0045de56  sub        ecx, 1
0045de59  fadd       st(1)
0045de5b  fstp       dword ptr [eax - 0x70]
0045de5e  fld        dword ptr [eax - 0x54]
0045de61  fadd       st(1)
0045de63  fstp       dword ptr [eax - 0x54]
0045de66  fld        dword ptr [eax - 0x38]
0045de69  fadd       st(1)
0045de6b  fstp       dword ptr [eax - 0x38]
0045de6e  jne        0x45de50

// block 0045de70
0045de70  mov        eax, dword ptr [edx + 0x3bc]
0045de76  fstp       st(0)
0045de78  mov        dword ptr [edi + 0x10], eax
0045de7b  mov        byte ptr [edi + 0x13], 0
0045de7f  fld        dword ptr [esi + 0x84]
0045de85  fadd       dword ptr [esi]
0045de87  sub        esp, 8
0045de8a  mov        ecx, edi
0045de8c  fstp       dword ptr [esp + 0x1c]
0045de90  fld        dword ptr [esp + 0x1c]
0045de94  fst        dword ptr [esi]
0045de96  fstp       dword ptr [esp + 4]
0045de9a  fld        dword ptr [esp + 0x18]
0045de9e  fstp       dword ptr [esp]
0045dea1  call       0x45df50

// block 0045dea6
0045dea6  fld        dword ptr [edx + 0x430]
0045deac  add        edi, 0x1c
0045deaf  fadd       dword ptr [edx + 0x424]
0045deb5  add        esi, 4
0045deb8  sub        ebx, 1
0045debb  fstp       dword ptr [esp + 0x1c]
0045debf  fld        dword ptr [edx + 0x434]
0045dec5  fadd       dword ptr [edx + 0x428]
0045decb  fstp       dword ptr [esp + 0x20]
0045decf  fld        dword ptr [edx + 0x438]
0045ded5  fadd       dword ptr [edx + 0x42c]
0045dedb  fstp       dword ptr [esp + 0x24]
0045dedf  fld        dword ptr [edi - 0x1c]
0045dee2  fadd       dword ptr [esp + 0x1c]
0045dee6  fstp       dword ptr [edi - 0x1c]
0045dee9  fld        dword ptr [edi - 0x18]
0045deec  fadd       dword ptr [esp + 0x20]
0045def0  fstp       dword ptr [edi - 0x18]
0045def3  fld        dword ptr [edi - 0x14]
0045def6  fadd       dword ptr [esp + 0x24]
0045defa  fstp       dword ptr [edi - 0x14]
0045defd  fld        dword ptr [esp + 0x10]
0045df01  fadd       qword ptr [0x4a3dc0]
0045df07  fstp       dword ptr [esp + 0x10]
0045df0b  jne        0x45dde0

// block 0045df11
0045df11  mov        esi, dword ptr [esp + 0x18]
0045df15  mov        ecx, 7

// block 0045df1a
0045df1a  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045df1c
0045df1c  pop        edi
0045df1d  pop        esi
0045df1e  pop        ebp
0045df1f  xor        eax, eax
0045df21  pop        ebx
0045df22  add        esp, 0x18
0045df25  ret        
