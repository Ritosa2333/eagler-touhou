
// block 004645b0
004645b0  push       ecx
004645b1  call       0x464440

// block 004645b6
004645b6  mov        dword ptr [esp], eax
004645b9  fild       dword ptr [esp]
004645bc  test       eax, eax
004645be  jge        0x4645c6

// block 004645c0
004645c0  fadd       dword ptr [0x4a3e10]

// block 004645c6
004645c6  fmul       qword ptr [0x4a3eb0]
004645cc  fstp       dword ptr [esp]
004645cf  fld        dword ptr [esp]
004645d2  pop        ecx
004645d3  ret        
