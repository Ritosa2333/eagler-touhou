
// block 004079d0
004079d0  mov        eax, dword ptr [edx + 0x18]
004079d3  sub        esp, 0xc
004079d6  xor        ecx, ecx
004079d8  cmp        eax, 0x1e
004079db  jge        0x4079e5

// block 004079dd
004079dd  xor        eax, eax
004079df  add        esp, 0xc
004079e2  ret        4

// block 004079e5
004079e5  cmp        eax, 0x5a
004079e8  jge        0x407a07

// block 004079ea
004079ea  fld        dword ptr [edx + 0x1c]
004079ed  fsub       qword ptr [0x4a3e78]
004079f3  fld        qword ptr [0x4a3d50]
004079f9  fmul       st(1), st(0)
004079fb  fxch       st(1)
004079fd  fdiv       qword ptr [0x4a3d20]
00407a03  fsubp      st(1)
00407a05  jmp        0x407a09

// block 00407a07
00407a07  fldz       

// block 00407a09
00407a09  mov        eax, dword ptr [esp + 0x10]
00407a0d  fstp       dword ptr [esp + 4]
00407a11  fld        dword ptr [eax + 4]
00407a14  fld        dword ptr [esp + 4]
00407a18  fcompp     
00407a1a  fnstsw     ax
00407a1c  test       ah, 0x41
00407a1f  jp         0x407a26

// block 00407a21
00407a21  mov        ecx, 0x10

// block 00407a26
00407a26  mov        eax, ecx
00407a28  add        esp, 0xc
00407a2b  ret        4
