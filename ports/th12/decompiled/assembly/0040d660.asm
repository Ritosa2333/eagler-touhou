
// block 0040d660
0040d660  push       ecx
0040d661  call       0x464440

// block 0040d666
0040d666  mov        dword ptr [esp], eax
0040d669  fild       dword ptr [esp]
0040d66c  test       eax, eax
0040d66e  jge        0x40d676

// block 0040d670
0040d670  fadd       dword ptr [0x4a3e10]

// block 0040d676
0040d676  fmul       qword ptr [0x4a3eb0]
0040d67c  fstp       dword ptr [esp]
0040d67f  fld        dword ptr [esp]
0040d682  fmul       dword ptr [esp + 8]
0040d686  fstp       dword ptr [esp + 8]
0040d68a  fld        dword ptr [esp + 8]
0040d68e  pop        ecx
0040d68f  ret        4
