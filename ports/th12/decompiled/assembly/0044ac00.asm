
// block 0044ac00
0044ac00  push       ebp
0044ac01  mov        ebp, esp
0044ac03  and        esp, 0xfffffff8
0044ac06  mov        edx, dword ptr [0x4b4534]
0044ac0c  mov        ecx, dword ptr [edx + 0x38]
0044ac0f  sub        esp, 0xc
0044ac12  push       edi
0044ac13  test       ecx, ecx
0044ac15  je         0x44ac34

// block 0044ac17
0044ac17  mov        eax, dword ptr [0x4b43dc]
0044ac1c  mov        eax, dword ptr [eax + 0x68]
0044ac1f  test       eax, eax
0044ac21  je         0x44ac34

// block 0044ac23
0044ac23  mov        edi, dword ptr [eax]
0044ac25  cmp        dword ptr [edi + 0x27b4], ecx
0044ac2b  je         0x44ac3b

// block 0044ac2d
0044ac2d  mov        eax, dword ptr [eax + 4]
0044ac30  test       eax, eax
0044ac32  jne        0x44ac23

// block 0044ac34
0044ac34  fldz       
0044ac36  pop        edi
0044ac37  mov        esp, ebp
0044ac39  pop        ebp
0044ac3a  ret        

// block 0044ac3b
0044ac3b  mov        eax, dword ptr [edx + 0x34]
0044ac3e  fld        dword ptr [eax + 0x1074]
0044ac44  fsub       dword ptr [esi]
0044ac46  fstp       dword ptr [esp + 0xc]
0044ac4a  fld        dword ptr [eax + 0x1078]
0044ac50  fsub       dword ptr [esi + 4]
0044ac53  fstp       dword ptr [esp + 8]
0044ac57  fldz       
0044ac59  fld        st(0)
0044ac5b  fld        dword ptr [esp + 8]
0044ac5f  fucom      st(1)
0044ac61  fnstsw     ax
0044ac63  fstp       st(1)
0044ac65  fld        dword ptr [esp + 0xc]
0044ac69  test       ah, 0x44
0044ac6c  jp         0x44ac88

// block 0044ac6e
0044ac6e  fucom      st(2)
0044ac70  fnstsw     ax
0044ac72  fstp       st(2)
0044ac74  test       ah, 0x44
0044ac77  jp         0x44ac8a

// block 0044ac79
0044ac79  fstp       st(0)
0044ac7b  fstp       st(0)
0044ac7d  fld        dword ptr [0x4a3e08]
0044ac83  pop        edi
0044ac84  mov        esp, ebp
0044ac86  pop        ebp
0044ac87  ret        

// block 0044ac88
0044ac88  fstp       st(2)

// block 0044ac8a
0044ac8a  fxch       st(1)
0044ac8c  call       0x4937aa

// block 0044ac91
0044ac91  fstp       dword ptr [esp + 0xc]
0044ac95  fld        dword ptr [esp + 0xc]
0044ac99  pop        edi
0044ac9a  mov        esp, ebp
0044ac9c  pop        ebp
0044ac9d  ret        
