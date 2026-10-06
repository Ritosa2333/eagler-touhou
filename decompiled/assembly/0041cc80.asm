
// block 0041cc80
0041cc80  mov        eax, dword ptr [0x4cee40]
0041cc85  sub        esp, 0xc
0041cc88  cmp        eax, 0xf
0041cc8b  je         0x41cd4c

// block 0041cc91
0041cc91  cmp        eax, 4
0041cc94  je         0x41cd4c

// block 0041cc9a
0041cc9a  mov        eax, dword ptr [esp + 0x10]
0041cc9e  fld        dword ptr [eax + 0x34]
0041cca1  push       esi
0041cca2  mov        esi, dword ptr [0x4b43b8]
0041cca8  fstp       dword ptr [esp + 0x14]
0041ccac  test       esi, esi
0041ccae  je         0x41cd40

// block 0041ccb4
0041ccb4  fld        dword ptr [esp + 0x14]
0041ccb8  fcom       qword ptr [0x4a3e78]
0041ccbe  fnstsw     ax
0041ccc0  test       ah, 5
0041ccc3  jp         0x41cccc

// block 0041ccc5
0041ccc5  mov        eax, 0xff5050ff
0041ccca  jmp        0x41cce5

// block 0041cccc
0041cccc  fld        dword ptr [0x4a4010]
0041ccd2  fcomp      st(1)
0041ccd4  fnstsw     ax
0041ccd6  test       ah, 0x41
0041ccd9  jne        0x41cce2

// block 0041ccdb
0041ccdb  mov        eax, 0xffa0a0ff
0041cce0  jmp        0x41cce5

// block 0041cce2
0041cce2  or         eax, 0xffffffff

// block 0041cce5
0041cce5  fld        dword ptr [0x4a400c]
0041cceb  push       ebx
0041ccec  fstp       dword ptr [esp + 8]
0041ccf0  sub        esp, 8
0041ccf3  fld        dword ptr [0x4a4008]
0041ccf9  lea        ebx, [esp + 0x10]
0041ccfd  fstp       dword ptr [esp + 0x14]
0041cd01  mov        dword ptr [esi + 0x18f80], eax
0041cd07  fldz       
0041cd09  fstp       dword ptr [esp + 0x18]
0041cd0d  fadd       qword ptr [0x4a4000]
0041cd13  fstp       qword ptr [esp]
0041cd16  push       0x49fcc0
0041cd1b  call       0x401720

// block 0041cd20
0041cd20  mov        ecx, dword ptr [0x4b43b8]
0041cd26  add        esp, 0xc
0041cd29  pop        ebx
0041cd2a  mov        dword ptr [ecx + 0x18f80], 0xffffffff
0041cd34  mov        eax, 1
0041cd39  pop        esi
0041cd3a  add        esp, 0xc
0041cd3d  ret        4

// block 0041cd40
0041cd40  mov        eax, 1
0041cd45  pop        esi
0041cd46  add        esp, 0xc
0041cd49  ret        4

// block 0041cd4c
0041cd4c  mov        eax, 1
0041cd51  add        esp, 0xc
0041cd54  ret        4
