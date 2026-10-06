
// block 00438d70
00438d70  mov        eax, dword ptr [0x4b4514]
00438d75  sub        esp, 0x10
00438d78  cmp        dword ptr [eax + 0xc598], 0
00438d7f  jne        0x438d89

// block 00438d81
00438d81  fld        dword ptr [0x4a3e04]
00438d87  jmp        0x438d8f

// block 00438d89
00438d89  fld        dword ptr [0x4a3e00]

// block 00438d8f
00438d8f  fstp       dword ptr [esp]
00438d92  push       edi
00438d93  fld        dword ptr [esp + 4]
00438d97  sub        esp, 8
00438d9a  fstp       dword ptr [esp + 4]
00438d9e  lea        ecx, [esp + 0x10]
00438da2  fld        dword ptr [esi + 0xa8]
00438da8  fstp       dword ptr [esp]
00438dab  call       0x4393d0

// block 00438db0
00438db0  fld        dword ptr [esp + 8]
00438db4  fld        qword ptr [0x4a3df8]
00438dba  fmul       st(1), st(0)
00438dbc  fxch       st(1)
00438dbe  call       0x4931e0

// block 00438dc3
00438dc3  fmul       dword ptr [esp + 0xc]
00438dc7  mov        edi, dword ptr [0x4b4514]
00438dcd  mov        ecx, dword ptr [edi + 0x988]
00438dd3  sub        ecx, eax
00438dd5  mov        dword ptr [esi + 0x54], ecx
00438dd8  call       0x4931e0

// block 00438ddd
00438ddd  fld        dword ptr [esi + 0xa8]
00438de3  fadd       qword ptr [0x4a3df0]
00438de9  mov        edx, dword ptr [edi + 0x98c]
00438def  sub        edx, eax
00438df1  push       ecx
00438df2  fstp       dword ptr [esp + 8]
00438df6  mov        dword ptr [esi + 0x58], edx
00438df9  fld        dword ptr [esp + 8]
00438dfd  fstp       dword ptr [esp]
00438e00  call       0x4646e0

// block 00438e05
00438e05  mov        eax, dword ptr [esi + 0x54]
00438e08  fstp       dword ptr [esi + 0xa8]
00438e0e  cmp        eax, 0xffff9a00
00438e13  pop        edi
00438e14  mov        ecx, 1
00438e19  jge        0x438e33

// block 00438e1b
00438e1b  or         dword ptr [esi + 0xd8], ecx
00438e21  add        eax, 0xcc00
00438e26  mov        dword ptr [esi + 0xd4], ecx
00438e2c  mov        dword ptr [esi + 0x54], eax
00438e2f  add        esp, 0x10
00438e32  ret        

// block 00438e33
00438e33  cmp        eax, 0x6600
00438e38  jle        0x438e52

// block 00438e3a
00438e3a  add        eax, 0xffff3400
00438e3f  or         dword ptr [esi + 0xd8], ecx
00438e45  mov        dword ptr [esi + 0xd4], ecx
00438e4b  mov        dword ptr [esi + 0x54], eax
00438e4e  add        esp, 0x10
00438e51  ret        

// block 00438e52
00438e52  mov        eax, dword ptr [esi + 0xd8]
00438e58  test       cl, al
00438e5a  je         0x438e62

// block 00438e5c
00438e5c  mov        dword ptr [esi + 0xd4], ecx

// block 00438e62
00438e62  and        eax, 0xfffffffe
00438e65  mov        dword ptr [esi + 0xd8], eax
00438e6b  add        esp, 0x10
00438e6e  ret        
