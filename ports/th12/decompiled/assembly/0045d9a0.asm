
// block 0045d9a0
0045d9a0  mov        eax, dword ptr [ebx + 0x478]
0045d9a6  sub        esp, 0x28
0045d9a9  test       eax, eax
0045d9ab  je         0x45d9c0

// block 0045d9ad
0045d9ad  push       eax
0045d9ae  call       0x46c941

// block 0045d9b3
0045d9b3  add        esp, 4
0045d9b6  mov        dword ptr [ebx + 0x478], 0

// block 0045d9c0
0045d9c0  push       ebp
0045d9c1  push       esi
0045d9c2  push       edi
0045d9c3  push       0x4b0
0045d9c8  call       0x46d04a

// block 0045d9cd
0045d9cd  mov        ebp, eax
0045d9cf  add        esp, 4
0045d9d2  mov        esi, 0x4ce568
0045d9d7  mov        dword ptr [ebx + 0x478], ebp
0045d9dd  mov        dword ptr [ebx + 0x488], 0x45dcd0
0045d9e7  mov        dword ptr [ebx + 0x48c], 0x45df30
0045d9f1  call       0x464440

// block 0045d9f6
0045d9f6  mov        dword ptr [esp + 0x18], eax
0045d9fa  fild       dword ptr [esp + 0x18]
0045d9fe  test       eax, eax
0045da00  jge        0x45da08

// block 0045da02
0045da02  fadd       dword ptr [0x4a3e10]

// block 0045da08
0045da08  fmul       qword ptr [0x4a3ea8]
0045da0e  mov        esi, 0x4ce568
0045da13  fsub       qword ptr [0x4a3ce0]
0045da19  fstp       dword ptr [esp + 0x14]
0045da1d  fld        dword ptr [esp + 0x14]
0045da21  fmul       qword ptr [0x4a4090]
0045da27  fstp       dword ptr [ebp + 0x4a4]
0045da2d  call       0x464440

// block 0045da32
0045da32  mov        dword ptr [esp + 0x18], eax
0045da36  fild       dword ptr [esp + 0x18]
0045da3a  test       eax, eax
0045da3c  jge        0x45da44

// block 0045da3e
0045da3e  fadd       dword ptr [0x4a3e10]

// block 0045da44
0045da44  fmul       qword ptr [0x4a3ea8]
0045da4a  mov        esi, 0x4ce568
0045da4f  lea        edi, [ebp + 0x1c]
0045da52  fsub       qword ptr [0x4a3ce0]
0045da58  fstp       dword ptr [esp + 0x14]
0045da5c  fld        dword ptr [esp + 0x14]
0045da60  fmul       qword ptr [0x4a4090]
0045da66  fstp       dword ptr [ebp + 0x4a8]
0045da6c  fld        dword ptr [0x4a3dc8]
0045da72  fstp       dword ptr [esp + 0x10]
0045da76  fld        dword ptr [ebx + 0x424]
0045da7c  fadd       dword ptr [ebx + 0x430]
0045da82  fstp       dword ptr [esp + 0x1c]
0045da86  mov        eax, dword ptr [esp + 0x1c]
0045da8a  fld        dword ptr [ebx + 0x434]
0045da90  fadd       dword ptr [ebx + 0x428]
0045da96  fstp       dword ptr [esp + 0x20]
0045da9a  mov        ecx, dword ptr [esp + 0x20]
0045da9e  fld        dword ptr [ebx + 0x438]
0045daa4  fadd       dword ptr [ebx + 0x42c]
0045daaa  mov        dword ptr [ebp], eax
0045daad  mov        dword ptr [ebp + 4], ecx
0045dab0  fstp       dword ptr [esp + 0x24]
0045dab4  mov        edx, dword ptr [esp + 0x24]
0045dab8  fld1       
0045daba  mov        dword ptr [ebp + 8], edx
0045dabd  fstp       dword ptr [ebp + 0xc]
0045dac0  fld        dword ptr [0x4a4088]
0045dac6  fst        dword ptr [ebp + 0x14]
0045dac9  fstp       dword ptr [ebp + 0x18]
0045dacc  call       0x464440

// block 0045dad1
0045dad1  mov        dword ptr [esp + 0x18], eax
0045dad5  fild       dword ptr [esp + 0x18]
0045dad9  test       eax, eax
0045dadb  jge        0x45dae3

// block 0045dadd
0045dadd  fadd       dword ptr [0x4a3e10]

// block 0045dae3
0045dae3  fmul       qword ptr [0x4a3ea8]
0045dae9  add        ebp, 0x3a0
0045daef  fsub       qword ptr [0x4a3ce0]
0045daf5  fstp       dword ptr [esp + 0x14]
0045daf9  fld        dword ptr [esp + 0x14]
0045dafd  mov        dword ptr [esp + 0x14], 0x1f
0045db05  fmul       qword ptr [0x4a4080]
0045db0b  fstp       dword ptr [esp + 0xc]
0045db0f  nop        

// block 0045db10
0045db10  fld        dword ptr [esp + 0x10]
0045db14  fcom       qword ptr [0x4a3cd8]
0045db1a  fnstsw     ax
0045db1c  test       ah, 1
0045db1f  jne        0x45db2f

// block 0045db21
0045db21  fsub       qword ptr [0x4a3cd0]
0045db27  fstp       dword ptr [esp + 0x10]
0045db2b  fld        dword ptr [esp + 0x10]

// block 0045db2f
0045db2f  fld1       
0045db31  sub        esp, 8
0045db34  fstp       dword ptr [edi + 0xc]
0045db37  lea        ecx, [esp + 0x30]
0045db3b  fld        dword ptr [0x4a4088]
0045db41  fstp       dword ptr [esp + 4]
0045db45  fstp       dword ptr [esp]
0045db48  call       0x45df50

// block 0045db4d
0045db4d  fld        dword ptr [esp + 0x28]
0045db51  mov        esi, 0x4ce568
0045db56  fld        qword ptr [0x4a3cf8]
0045db5c  fadd       st(1), st(0)
0045db5e  fxch       st(1)
0045db60  fstp       dword ptr [edi + 0x14]
0045db63  fadd       dword ptr [esp + 0x2c]
0045db67  fstp       dword ptr [edi + 0x18]
0045db6a  fldz       
0045db6c  fstp       dword ptr [edi + 8]
0045db6f  call       0x464440

// block 0045db74
0045db74  mov        dword ptr [esp + 0x18], eax
0045db78  fild       dword ptr [esp + 0x18]
0045db7c  test       eax, eax
0045db7e  jge        0x45db86

// block 0045db80
0045db80  fadd       dword ptr [0x4a3e10]

// block 0045db86
0045db86  fmul       qword ptr [0x4a3ea8]
0045db8c  mov        esi, 0x4ce568
0045db91  fsub       qword ptr [0x4a3ce0]
0045db97  fstp       dword ptr [esp + 0x18]
0045db9b  fld        dword ptr [esp + 0x18]
0045db9f  fmul       qword ptr [0x4a4078]
0045dba5  fstp       dword ptr [esp + 0x18]
0045dba9  fld        dword ptr [esp + 0x18]
0045dbad  fadd       qword ptr [0x4a3f68]
0045dbb3  fstp       dword ptr [ebp]
0045dbb6  fld        dword ptr [esp + 0xc]
0045dbba  fstp       dword ptr [ebp + 0x84]
0045dbc0  call       0x464440

// block 0045dbc5
0045dbc5  mov        dword ptr [esp + 0x18], eax
0045dbc9  fild       dword ptr [esp + 0x18]
0045dbcd  test       eax, eax
0045dbcf  jge        0x45dbd7

// block 0045dbd1
0045dbd1  fadd       dword ptr [0x4a3e10]

// block 0045dbd7
0045dbd7  fmul       qword ptr [0x4a3ea8]
0045dbdd  fsub       qword ptr [0x4a3ce0]
0045dbe3  fstp       dword ptr [esp + 0x18]
0045dbe7  fld        dword ptr [esp + 0x18]
0045dbeb  fmul       qword ptr [0x4a4070]
0045dbf1  fstp       dword ptr [esp + 0x18]
0045dbf5  fld        dword ptr [esp + 0x18]
0045dbf9  fadd       dword ptr [esp + 0xc]
0045dbfd  fstp       dword ptr [esp + 0xc]
0045dc01  fld        dword ptr [0x4a4068]
0045dc07  fld        dword ptr [esp + 0xc]
0045dc0b  fcom       st(1)
0045dc0d  fnstsw     ax
0045dc0f  test       ah, 5
0045dc12  jp         0x45dc1c

// block 0045dc14
0045dc14  fstp       st(0)
0045dc16  fstp       dword ptr [esp + 0xc]
0045dc1a  jmp        0x45dc37

// block 0045dc1c
0045dc1c  fstp       st(1)
0045dc1e  fld        dword ptr [0x4a4064]
0045dc24  fcom       st(1)
0045dc26  fnstsw     ax
0045dc28  fstp       st(1)
0045dc2a  test       ah, 5
0045dc2d  jp         0x45dc35

// block 0045dc2f
0045dc2f  fstp       dword ptr [esp + 0xc]
0045dc33  jmp        0x45dc37

// block 0045dc35
0045dc35  fstp       st(0)

// block 0045dc37
0045dc37  fld        dword ptr [ebp]
0045dc3a  sub        esp, 8
0045dc3d  fstp       dword ptr [esp + 4]
0045dc41  mov        ecx, edi
0045dc43  fld        dword ptr [esp + 0x18]
0045dc47  fstp       dword ptr [esp]
0045dc4a  call       0x45df50

// block 0045dc4f
0045dc4f  fld        dword ptr [ebx + 0x430]
0045dc55  add        edi, 0x1c
0045dc58  fadd       dword ptr [ebx + 0x424]
0045dc5e  add        ebp, 4
0045dc61  sub        dword ptr [esp + 0x14], 1
0045dc66  fstp       dword ptr [esp + 0x1c]
0045dc6a  fld        dword ptr [ebx + 0x434]
0045dc70  fadd       dword ptr [ebx + 0x428]
0045dc76  fstp       dword ptr [esp + 0x20]
0045dc7a  fld        dword ptr [ebx + 0x438]
0045dc80  fadd       dword ptr [ebx + 0x42c]
0045dc86  fstp       dword ptr [esp + 0x24]
0045dc8a  fld        dword ptr [edi - 0x1c]
0045dc8d  fadd       dword ptr [esp + 0x1c]
0045dc91  fstp       dword ptr [edi - 0x1c]
0045dc94  fld        dword ptr [edi - 0x18]
0045dc97  fadd       dword ptr [esp + 0x20]
0045dc9b  fstp       dword ptr [edi - 0x18]
0045dc9e  fld        dword ptr [edi - 0x14]
0045dca1  fadd       dword ptr [esp + 0x24]
0045dca5  fstp       dword ptr [edi - 0x14]
0045dca8  fld        dword ptr [esp + 0x10]
0045dcac  fadd       qword ptr [0x4a3dc0]
0045dcb2  fstp       dword ptr [esp + 0x10]
0045dcb6  jne        0x45db10

// block 0045dcbc
0045dcbc  pop        edi
0045dcbd  pop        esi
0045dcbe  xor        eax, eax
0045dcc0  pop        ebp
0045dcc1  add        esp, 0x28
0045dcc4  ret        
