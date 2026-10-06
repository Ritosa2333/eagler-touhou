
// block 0042a800
0042a800  sub        esp, 0x2c
0042a803  cmp        dword ptr [esp + 0x34], 0
0042a808  push       ebp
0042a809  mov        ebp, ecx
0042a80b  je         0x42a81f

// block 0042a80d
0042a80d  cmp        dword ptr [ebp + 0x44c], 0
0042a814  je         0x42a81f

// block 0042a816
0042a816  xor        eax, eax
0042a818  pop        ebp
0042a819  add        esp, 0x2c
0042a81c  ret        8

// block 0042a81f
0042a81f  fld        dword ptr [0x4a3e54]
0042a825  sub        esp, 8
0042a828  fst        dword ptr [esp + 0x40]
0042a82c  lea        ecx, [esp + 0x2c]
0042a830  fstp       dword ptr [esp + 4]
0042a834  mov        dword ptr [esp + 0xc], 0
0042a83c  fld        dword ptr [ebp + 0x68]
0042a83f  fstp       dword ptr [esp]
0042a842  call       0x42e880

// block 0042a847
0042a847  fld        dword ptr [ebp + 0x50]
0042a84a  fld        dword ptr [esp + 0x24]
0042a84e  fld        st(0)
0042a850  faddp      st(2)
0042a852  fxch       st(1)
0042a854  fstp       dword ptr [esp + 0xc]
0042a858  mov        eax, dword ptr [esp + 0xc]
0042a85c  fld        dword ptr [ebp + 0x54]
0042a85f  mov        dword ptr [esp + 0x18], eax
0042a863  fld        dword ptr [esp + 0x28]
0042a867  fld        st(0)
0042a869  faddp      st(2)
0042a86b  fxch       st(1)
0042a86d  fstp       dword ptr [esp + 0x10]
0042a871  mov        ecx, dword ptr [esp + 0x10]
0042a875  fld        dword ptr [ebp + 0x58]
0042a878  mov        dword ptr [esp + 0x1c], ecx
0042a87c  fldz       
0042a87e  fadd       st(1), st(0)
0042a880  fxch       st(1)
0042a882  fstp       dword ptr [esp + 0x14]
0042a886  mov        edx, dword ptr [esp + 0x14]
0042a88a  fld        st(2)
0042a88c  mov        dword ptr [esp + 0x20], edx
0042a890  faddp      st(3)
0042a892  fxch       st(2)
0042a894  fstp       dword ptr [esp + 0x24]
0042a898  fadd       st(0), st(0)
0042a89a  fstp       dword ptr [esp + 0x28]
0042a89e  fadd       st(0), st(0)
0042a8a0  fstp       dword ptr [esp + 0x2c]
0042a8a4  fld        dword ptr [ebp + 0x6c]
0042a8a7  fcomp      qword ptr [0x4a3d68]
0042a8ad  fnstsw     ax
0042a8af  test       ah, 0x41
0042a8b2  jne        0x42aa4a

// block 0042a8b8
0042a8b8  push       ebx
0042a8b9  push       esi
0042a8ba  push       edi
0042a8bb  jmp        0x42a8c0

// block 0042a8c0
0042a8c0  movsx      ebx, word ptr [ebp + 0x47a]
0042a8c7  mov        eax, dword ptr [0x4b44f4]
0042a8cc  mov        edi, dword ptr [eax + 0x488]
0042a8d2  mov        esi, 1
0042a8d7  add        dword ptr [esp + 0x10], esi
0042a8db  test       dword ptr [0x4cee78], 0x8000
0042a8e5  lea        ebx, [ebx + ebx + 4]
0042a8e9  je         0x42a8fc

// block 0042a8eb
0042a8eb  push       0x4cf1d0
0042a8f0  call       dword ptr [0x498088]

// block 0042a8f6
0042a8f6  inc        byte ptr [0x4cf221]

// block 0042a8fc
0042a8fc  add        dword ptr [edi + 0x130], esi
0042a902  call       0x4621c0

// block 0042a907
0042a907  fld        dword ptr [esp + 0x24]
0042a90b  fadd       qword ptr [0x4a3d70]
0042a911  mov        esi, eax
0042a913  or         dword ptr [esi + 0x480], 1
0042a91a  mov        dword ptr [esi + 0x20], 0x17
0042a921  fst        qword ptr [esp + 0x18]
0042a925  push       ebx
0042a926  fadd       qword ptr [0x4a3d28]
0042a92c  push       esi
0042a92d  mov        ecx, edi
0042a92f  fstp       dword ptr [esi + 0x430]
0042a935  fld        dword ptr [esp + 0x30]
0042a939  fadd       qword ptr [0x4a3d68]
0042a93f  fstp       dword ptr [esi + 0x434]
0042a945  fld        dword ptr [esp + 0x34]
0042a949  fstp       dword ptr [esi + 0x438]
0042a94f  call       0x454d10

// block 0042a954
0042a954  mov        ebx, esi
0042a956  lea        eax, [esp + 0x14]
0042a95a  call       0x461250

// block 0042a95f
0042a95f  test       dword ptr [0x4cee78], 0x8000
0042a969  je         0x42a97c

// block 0042a96b
0042a96b  push       0x4cf1d0
0042a970  call       dword ptr [0x49808c]

// block 0042a976
0042a976  dec        byte ptr [0x4cf221]

// block 0042a97c
0042a97c  cmp        dword ptr [esp + 0x40], 0
0042a981  je         0x42a9fb

// block 0042a983
0042a983  fld        qword ptr [0x4a3d60]
0042a989  fcomp      qword ptr [esp + 0x18]
0042a98d  fnstsw     ax
0042a98f  test       ah, 1
0042a992  je         0x42a9fb

// block 0042a994
0042a994  fld        dword ptr [esp + 0x24]
0042a998  fld        qword ptr [0x4a3d70]
0042a99e  fsub       st(1), st(0)
0042a9a0  fxch       st(1)
0042a9a2  fcomp      qword ptr [0x4a3d28]
0042a9a8  fnstsw     ax
0042a9aa  test       ah, 1
0042a9ad  je         0x42a9f9

// block 0042a9af
0042a9af  fld        dword ptr [esp + 0x28]
0042a9b3  fld        st(0)
0042a9b5  fadd       st(2)
0042a9b7  fcomp      qword ptr [0x4a3d58]
0042a9bd  fnstsw     ax
0042a9bf  test       ah, 0x41
0042a9c2  jnp        0x42a9f7

// block 0042a9c4
0042a9c4  fsubrp     st(1)
0042a9c6  fcomp      qword ptr [0x4a3d50]
0042a9cc  fnstsw     ax
0042a9ce  test       ah, 1
0042a9d1  je         0x42a9fb

// block 0042a9d3
0042a9d3  fld        dword ptr [0x4a41f4]
0042a9d9  sub        esp, 8
0042a9dc  fstp       dword ptr [esp + 4]
0042a9e0  lea        ecx, [esp + 0x2c]
0042a9e4  fld        dword ptr [0x4a4060]
0042a9ea  fstp       dword ptr [esp]
0042a9ed  push       ecx
0042a9ee  push       9
0042a9f0  call       0x4273f0

// block 0042a9f5
0042a9f5  jmp        0x42a9fb

// block 0042a9f7
0042a9f7  fstp       st(1)

// block 0042a9f9
0042a9f9  fstp       st(0)

// block 0042a9fb
0042a9fb  fld        dword ptr [esp + 0x24]
0042a9ff  fadd       dword ptr [esp + 0x30]
0042aa03  fstp       dword ptr [esp + 0x24]
0042aa07  fld        dword ptr [esp + 0x28]
0042aa0b  fadd       dword ptr [esp + 0x34]
0042aa0f  fstp       dword ptr [esp + 0x28]
0042aa13  fld        dword ptr [esp + 0x2c]
0042aa17  fadd       dword ptr [esp + 0x38]
0042aa1b  fstp       dword ptr [esp + 0x2c]
0042aa1f  fld        dword ptr [esp + 0x44]
0042aa23  fadd       qword ptr [0x4a3d68]
0042aa29  fstp       dword ptr [esp + 0x44]
0042aa2d  fld        dword ptr [esp + 0x44]
0042aa31  fadd       qword ptr [0x4a4078]
0042aa37  fld        dword ptr [ebp + 0x6c]
0042aa3a  fcompp     
0042aa3c  fnstsw     ax
0042aa3e  test       ah, 0x41
0042aa41  je         0x42a8c0

// block 0042aa47
0042aa47  pop        edi
0042aa48  pop        esi
0042aa49  pop        ebx

// block 0042aa4a
0042aa4a  mov        eax, dword ptr [esp + 4]
0042aa4e  mov        dword ptr [ebp + 0xc], 1
0042aa55  pop        ebp
0042aa56  add        esp, 0x2c
0042aa59  ret        8
