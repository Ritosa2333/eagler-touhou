
// block 00461e30
00461e30  mov        eax, dword ptr [eax]
00461e32  mov        edx, dword ptr [0x4ce8cc]
00461e38  push       eax
00461e39  call       0x461920

// block 00461e3e
00461e3e  test       eax, eax
00461e40  je         0x461e6e

// block 00461e42
00461e42  fld        dword ptr [esi]
00461e44  fadd       qword ptr [0x4a3d70]
00461e4a  fadd       qword ptr [0x4a3d28]
00461e50  fstp       dword ptr [eax + 0x430]
00461e56  fld        dword ptr [esi + 4]
00461e59  fadd       qword ptr [0x4a3d68]
00461e5f  fstp       dword ptr [eax + 0x434]
00461e65  fld        dword ptr [esi + 8]
00461e68  fstp       dword ptr [eax + 0x438]

// block 00461e6e
00461e6e  ret        
