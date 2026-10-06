
// block 0043ab10
0043ab10  push       ecx
0043ab11  push       esi
0043ab12  push       edi
0043ab13  mov        esi, 0x4ce568
0043ab18  mov        edi, edx
0043ab1a  call       0x464440

// block 0043ab1f
0043ab1f  mov        dword ptr [esp + 8], eax
0043ab23  fild       dword ptr [esp + 8]
0043ab27  test       eax, eax
0043ab29  jge        0x43ab31

// block 0043ab2b
0043ab2b  fadd       dword ptr [0x4a3e10]

// block 0043ab31
0043ab31  fmul       qword ptr [0x4a3ea8]
0043ab37  xor        eax, eax
0043ab39  fsub       qword ptr [0x4a3ce0]
0043ab3f  fstp       dword ptr [esp + 8]
0043ab43  fld        dword ptr [esp + 8]
0043ab47  fmul       qword ptr [0x4a3fa0]
0043ab4d  fstp       dword ptr [edi + 0x70]
0043ab50  pop        edi
0043ab51  pop        esi
0043ab52  pop        ecx
0043ab53  ret        4
