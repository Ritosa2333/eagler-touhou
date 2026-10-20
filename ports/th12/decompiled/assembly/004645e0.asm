
// block 004645e0
004645e0  push       ecx
004645e1  call       0x464440

// block 004645e6
004645e6  mov        dword ptr [esp], eax
004645e9  fild       dword ptr [esp]
004645ec  test       eax, eax
004645ee  jge        0x4645f6

// block 004645f0
004645f0  fadd       dword ptr [0x4a3e10]

// block 004645f6
004645f6  fmul       qword ptr [0x4a3ea8]
004645fc  fsub       qword ptr [0x4a3ce0]
00464602  fstp       dword ptr [esp]
00464605  fld        dword ptr [esp]
00464608  pop        ecx
00464609  ret        
