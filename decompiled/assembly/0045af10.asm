
// block 0045af10
0045af10  sub        esp, 0x34
0045af13  mov        ecx, dword ptr [eax + 0x3c]
0045af16  fld        dword ptr [eax + 0x2c]
0045af19  push       ebp
0045af1a  mov        ebp, dword ptr [esp + 0x3c]
0045af1e  fstp       dword ptr [esp + 0x3c]
0045af22  test       ecx, ecx
0045af24  je         0x45af31

// block 0045af26
0045af26  fld        dword ptr [ecx + 0x2c]
0045af29  fadd       dword ptr [esp + 0x3c]
0045af2d  fstp       dword ptr [esp + 0x3c]

// block 0045af31
0045af31  fld        dword ptr [esp + 0x3c]
0045af35  fsincos    
0045af37  fstp       dword ptr [esp + 0x10]
0045af3b  fstp       dword ptr [esp + 0x14]
0045af3f  fld        dword ptr [eax + 0x430]
0045af45  cmp        dword ptr [eax + 0x3c], 0
0045af49  fadd       dword ptr [eax + 0x424]
0045af4f  fadd       dword ptr [eax + 0x43c]
0045af55  fstp       dword ptr [esp + 8]
0045af59  fld        dword ptr [eax + 0x434]
0045af5f  fadd       dword ptr [eax + 0x428]
0045af65  fadd       dword ptr [eax + 0x440]
0045af6b  fstp       dword ptr [esp + 0xc]
0045af6f  fld        dword ptr [eax + 0x58]
0045af72  fmul       dword ptr [eax + 0x40]
0045af75  fstp       dword ptr [esp + 0x3c]
0045af79  fld        dword ptr [eax + 0x5c]
0045af7c  fmul       dword ptr [eax + 0x44]
0045af7f  fstp       dword ptr [esp + 4]
0045af83  je         0x45afd2

// block 0045af85
0045af85  mov        ecx, dword ptr [eax + 0x3c]
0045af88  fld        dword ptr [ecx + 0x430]
0045af8e  fadd       dword ptr [ecx + 0x424]
0045af94  fadd       dword ptr [ecx + 0x43c]
0045af9a  fadd       dword ptr [esp + 8]
0045af9e  fstp       dword ptr [esp + 8]
0045afa2  fld        dword ptr [ecx + 0x434]
0045afa8  fadd       dword ptr [ecx + 0x428]
0045afae  fadd       dword ptr [ecx + 0x440]
0045afb4  fadd       dword ptr [esp + 0xc]
0045afb8  fstp       dword ptr [esp + 0xc]
0045afbc  fld        dword ptr [ecx + 0x40]
0045afbf  fmul       dword ptr [esp + 0x3c]
0045afc3  fstp       dword ptr [esp + 0x3c]
0045afc7  fld        dword ptr [ecx + 0x44]
0045afca  fmul       dword ptr [esp + 4]
0045afce  fstp       dword ptr [esp + 4]

// block 0045afd2
0045afd2  mov        edx, dword ptr [eax + 0x47c]
0045afd8  fldz       
0045afda  fld        qword ptr [0x4a3cf8]
0045afe0  mov        ecx, edx
0045afe2  shr        ecx, 0x13
0045afe5  and        ecx, 3
0045afe8  sub        ecx, 0
0045afeb  je         0x45b068

// block 0045afed
0045afed  sub        ecx, 1
0045aff0  je         0x45b050

// block 0045aff2
0045aff2  sub        ecx, 1
0045aff5  jne        0x45b015

// block 0045aff7
0045aff7  fld        dword ptr [esp + 0x3c]
0045affb  fchs       
0045affd  fstp       dword ptr [esp + 0x30]
0045b001  fld        dword ptr [esp + 0x30]
0045b005  fstp       dword ptr [esp + 0x28]
0045b009  fxch       st(1)
0045b00b  fst        dword ptr [esp + 0x34]
0045b00f  fst        dword ptr [esp + 0x2c]

// block 0045b013
0045b013  fxch       st(1)

// block 0045b015
0045b015  shr        edx, 0x15
0045b018  and        edx, 3
0045b01b  sub        edx, 0
0045b01e  je         0x45b0a4

// block 0045b024
0045b024  sub        edx, 1
0045b027  fstp       st(0)
0045b029  je         0x45b08e

// block 0045b02b
0045b02b  sub        edx, 1
0045b02e  jne        0x45b0cc

// block 0045b034
0045b034  fld        dword ptr [esp + 4]
0045b038  fchs       
0045b03a  fstp       dword ptr [esp + 0x1c]
0045b03e  fld        dword ptr [esp + 0x1c]
0045b042  fstp       dword ptr [esp + 0x18]
0045b046  fst        dword ptr [esp + 0x24]
0045b04a  fstp       dword ptr [esp + 0x20]
0045b04e  jmp        0x45b0ce

// block 0045b050
0045b050  fxch       st(1)
0045b052  fst        dword ptr [esp + 0x30]
0045b056  fst        dword ptr [esp + 0x28]
0045b05a  fld        dword ptr [esp + 0x3c]
0045b05e  fst        dword ptr [esp + 0x34]
0045b062  fstp       dword ptr [esp + 0x2c]
0045b066  jmp        0x45b013

// block 0045b068
0045b068  fld        dword ptr [esp + 0x3c]
0045b06c  fld        st(0)
0045b06e  fchs       
0045b070  fmul       st(2)
0045b072  fstp       dword ptr [esp + 0x30]
0045b076  fld        dword ptr [esp + 0x30]
0045b07a  fstp       dword ptr [esp + 0x28]
0045b07e  fmul       st(1)
0045b080  fstp       dword ptr [esp + 0x34]
0045b084  fld        dword ptr [esp + 0x34]
0045b088  fstp       dword ptr [esp + 0x2c]
0045b08c  jmp        0x45b015

// block 0045b08e
0045b08e  fst        dword ptr [esp + 0x1c]
0045b092  fstp       dword ptr [esp + 0x18]
0045b096  fld        dword ptr [esp + 4]
0045b09a  fst        dword ptr [esp + 0x24]
0045b09e  fstp       dword ptr [esp + 0x20]
0045b0a2  jmp        0x45b0ce

// block 0045b0a4
0045b0a4  fstp       st(1)
0045b0a6  fld        dword ptr [esp + 4]
0045b0aa  fld        st(0)
0045b0ac  fchs       
0045b0ae  fmul       st(2)
0045b0b0  fstp       dword ptr [esp + 0x1c]
0045b0b4  fld        dword ptr [esp + 0x1c]
0045b0b8  fstp       dword ptr [esp + 0x18]
0045b0bc  fmulp      st(1)
0045b0be  fstp       dword ptr [esp + 0x24]
0045b0c2  fld        dword ptr [esp + 0x24]
0045b0c6  fstp       dword ptr [esp + 0x20]
0045b0ca  jmp        0x45b0ce

// block 0045b0cc
0045b0cc  fstp       st(0)

// block 0045b0ce
0045b0ce  fld        dword ptr [esp + 0x28]
0045b0d2  fld        st(0)
0045b0d4  fld        dword ptr [esp + 0x10]
0045b0d8  fld        st(0)
0045b0da  fmulp      st(2)
0045b0dc  fld        dword ptr [esp + 0x18]
0045b0e0  fld        st(0)
0045b0e2  fld        dword ptr [esp + 0x14]
0045b0e6  fld        st(0)
0045b0e8  fmulp      st(2)
0045b0ea  fxch       st(4)
0045b0ec  fsubrp     st(1)
0045b0ee  fld        dword ptr [esp + 8]
0045b0f2  fld        st(0)
0045b0f4  faddp      st(2)
0045b0f6  fxch       st(1)
0045b0f8  fstp       dword ptr [ebp]
0045b0fb  fld        st(2)
0045b0fd  fmulp      st(2)
0045b0ff  fld        st(3)
0045b101  fmulp      st(5)
0045b103  fxch       st(1)
0045b105  faddp      st(4)
0045b107  fld        dword ptr [esp + 0xc]
0045b10b  fld        st(0)
0045b10d  faddp      st(5)
0045b10f  fxch       st(4)
0045b111  fstp       dword ptr [ebp + 4]
0045b114  fld        dword ptr [esp + 0x2c]
0045b118  fld        st(0)
0045b11a  fmul       st(3)
0045b11c  fld        dword ptr [esp + 0x1c]
0045b120  fmul       st(5)
0045b122  fsubp      st(1)
0045b124  fadd       st(2)
0045b126  fstp       dword ptr [ebx]
0045b128  fmul       st(3)
0045b12a  fld        dword ptr [esp + 0x1c]
0045b12e  fmul       st(3)
0045b130  faddp      st(1)
0045b132  fadd       st(4)
0045b134  fstp       dword ptr [ebx + 4]
0045b137  fld        dword ptr [esp + 0x30]
0045b13b  fld        st(0)
0045b13d  fmul       st(3)
0045b13f  fld        dword ptr [esp + 0x20]
0045b143  fmul       st(5)
0045b145  fsubp      st(1)
0045b147  fadd       st(2)
0045b149  fstp       dword ptr [edi]
0045b14b  fld        dword ptr [esp + 0x20]
0045b14f  fmul       st(3)
0045b151  fld        st(4)
0045b153  fmulp      st(2)
0045b155  faddp      st(1)
0045b157  fadd       st(4)
0045b159  fstp       dword ptr [edi + 4]
0045b15c  fld        dword ptr [esp + 0x34]
0045b160  fmul       st(2)
0045b162  fld        dword ptr [esp + 0x24]
0045b166  fld        st(0)
0045b168  fmul       st(5)
0045b16a  fsubp      st(2)
0045b16c  fxch       st(1)
0045b16e  faddp      st(2)
0045b170  fxch       st(1)
0045b172  fstp       dword ptr [esi]
0045b174  fmulp      st(1)
0045b176  fld        dword ptr [esp + 0x34]
0045b17a  fmulp      st(2)
0045b17c  faddp      st(1)
0045b17e  faddp      st(1)
0045b180  fstp       dword ptr [esi + 4]
0045b183  fld        dword ptr [eax + 0x438]
0045b189  fadd       dword ptr [eax + 0x42c]
0045b18f  fadd       dword ptr [eax + 0x444]
0045b195  fstp       dword ptr [esp + 0x3c]
0045b199  fld        dword ptr [esp + 0x3c]
0045b19d  fst        dword ptr [esi + 8]
0045b1a0  fst        dword ptr [edi + 8]
0045b1a3  fst        dword ptr [ebx + 8]
0045b1a6  fstp       dword ptr [ebp + 8]
0045b1a9  pop        ebp
0045b1aa  add        esp, 0x34
0045b1ad  ret        4
