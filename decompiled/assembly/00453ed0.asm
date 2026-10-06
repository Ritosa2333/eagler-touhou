
// block 00453ed0
00453ed0  fld        dword ptr [esp + 4]
00453ed4  lea        eax, [eax + eax*2]
00453ed7  fmul       qword ptr [0x4a3d30]
00453edd  lea        eax, [eax*8 + 0x4d0e70]
00453ee4  push       esi
00453ee5  mov        esi, dword ptr [eax]
00453ee7  fdiv       qword ptr [0x4a3d28]
00453eed  push       edi
00453eee  mov        edi, dword ptr [esi]
00453ef0  call       0x4931e0

// block 00453ef5
00453ef5  push       eax
00453ef6  mov        eax, dword ptr [edi + 0x40]
00453ef9  push       esi
00453efa  call       eax

// block 00453efc
00453efc  pop        edi
00453efd  pop        esi
00453efe  ret        4
