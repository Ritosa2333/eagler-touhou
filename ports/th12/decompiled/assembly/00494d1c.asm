
// block 00494d1c
00494d1c  push       edx
00494d1d  sub        esp, 0x30
00494d20  fstp       xword ptr [esp + 0x18]
00494d24  fstp       xword ptr [esp]
00494d27  xor        edx, edx
00494d29  mov        eax, dword ptr [esp + 6]
00494d2d  test       eax, 0x7fff0000
00494d32  je         0x494d3e

// block 00494d34
00494d34  call       0x494b16

// block 00494d39
00494d39  add        esp, 0x30
00494d3c  pop        edx
00494d3d  ret        

// block 00494d3e
00494d3e  fld        xword ptr [esp]
00494d41  fld        xword ptr [esp + 0x18]
00494d45  mov        eax, dword ptr [esp]
00494d48  or         eax, dword ptr [esp + 4]
00494d4c  je         0x494dc7

// block 00494d4e
00494d4e  fxch       st(1)
00494d50  fstp       xword ptr [esp + 0xc]
00494d54  fld        xword ptr [esp]
00494d57  fxch       st(1)
00494d59  or         edx, 2
00494d5c  fnstcw     word ptr [esp + 0x24]
00494d60  mov        eax, dword ptr [esp + 0x24]
00494d64  or         eax, 0x33f
00494d69  mov        dword ptr [esp + 0x28], eax
00494d6d  fldcw      word ptr [esp + 0x28]
00494d71  mov        eax, dword ptr [esp + 0x20]
00494d75  and        eax, 0x7fff
00494d7a  cmp        eax, 0x7fbe
00494d7f  ja         0x494d99

// block 00494d81
00494d81  or         edx, 1
00494d84  fmul       qword ptr [0x4b3624]
00494d8a  fstp       xword ptr [esp + 0x18]
00494d8e  fmul       qword ptr [0x4b3624]
00494d94  fstp       xword ptr [esp]
00494d97  jmp        0x494db9

// block 00494d99
00494d99  fnstcw     word ptr [esp + 0x24]
00494d9d  mov        eax, dword ptr [esp + 0x24]
00494da1  or         eax, 0x300
00494da6  mov        dword ptr [esp + 0x28], eax
00494daa  fldcw      word ptr [esp + 0x28]
00494dae  fstp       st(0)
00494db0  fmul       qword ptr [0x4b3624]
00494db6  fstp       xword ptr [esp]

// block 00494db9
00494db9  fldcw      word ptr [esp + 0x24]
00494dbd  call       0x494b16

// block 00494dc2
00494dc2  add        esp, 0x30
00494dc5  pop        edx
00494dc6  ret        

// block 00494dc7
00494dc7  fprem      
00494dc9  add        esp, 0x30
00494dcc  pop        edx
00494dcd  ret        
