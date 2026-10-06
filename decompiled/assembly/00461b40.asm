
// block 00461b40
00461b40  mov        eax, dword ptr [esp + 4]
00461b44  mov        edx, dword ptr [0x4ce8cc]
00461b4a  push       eax
00461b4b  call       0x461920

// block 00461b50
00461b50  test       eax, eax
00461b52  je         0x461b80

// block 00461b54
00461b54  fld        dword ptr [esi]
00461b56  fadd       qword ptr [0x4a3d70]
00461b5c  fadd       qword ptr [0x4a3d28]
00461b62  fstp       dword ptr [eax + 0x430]
00461b68  fld        dword ptr [esi + 4]
00461b6b  fadd       qword ptr [0x4a3d68]
00461b71  fstp       dword ptr [eax + 0x434]
00461b77  fld        dword ptr [esi + 8]
00461b7a  fstp       dword ptr [eax + 0x438]

// block 00461b80
00461b80  ret        4
