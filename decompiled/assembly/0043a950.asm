
// block 0043a950
0043a950  sub        esp, 0x14
0043a953  push       ebx
0043a954  push       esi
0043a955  push       edi
0043a956  mov        esi, 0x4ce568
0043a95b  mov        edi, edx
0043a95d  call       0x464440

// block 0043a962
0043a962  mov        dword ptr [esp + 0x10], eax
0043a966  fild       dword ptr [esp + 0x10]
0043a96a  test       eax, eax
0043a96c  jge        0x43a974

// block 0043a96e
0043a96e  fadd       dword ptr [0x4a3e10]

// block 0043a974
0043a974  fmul       qword ptr [0x4a3ea8]
0043a97a  push       ecx
0043a97b  fsub       qword ptr [0x4a3ce0]
0043a981  fstp       dword ptr [esp + 0x10]
0043a985  fld        dword ptr [esp + 0x10]
0043a989  fmul       qword ptr [0x4a3cd8]
0043a98f  fstp       dword ptr [esp + 0x10]
0043a993  fld        dword ptr [esp + 0x10]
0043a997  fstp       dword ptr [esp]
0043a99a  call       0x4646e0

// block 0043a99f
0043a99f  fstp       dword ptr [esp + 0xc]
0043a9a3  mov        esi, 6
0043a9a8  jmp        0x43a9b0

// block 0043a9b0
0043a9b0  fld        dword ptr [esp + 0xc]
0043a9b4  sub        esp, 8
0043a9b7  fst        dword ptr [0x4b336c]
0043a9bd  lea        ecx, [esp + 0x1c]
0043a9c1  fldz       
0043a9c3  fstp       dword ptr [esp + 0x24]
0043a9c7  fld        dword ptr [0x4a404c]
0043a9cd  fstp       dword ptr [esp + 4]
0043a9d1  fstp       dword ptr [esp]
0043a9d4  call       0x43acc0

// block 0043a9d9
0043a9d9  fld        dword ptr [edi + 0x14]
0043a9dc  mov        ebx, dword ptr [0x4b4514]
0043a9e2  fadd       dword ptr [esp + 0x14]
0043a9e6  push       0
0043a9e8  push       0x4b3358
0043a9ed  lea        ecx, [esp + 0x1c]
0043a9f1  fstp       dword ptr [esp + 0x1c]
0043a9f5  fld        dword ptr [edi + 0x18]
0043a9f8  fadd       dword ptr [esp + 0x20]
0043a9fc  fstp       dword ptr [esp + 0x20]
0043aa00  fld        dword ptr [edi + 0x1c]
0043aa03  fadd       dword ptr [esp + 0x24]
0043aa07  fstp       dword ptr [esp + 0x24]
0043aa0b  call       0x439630

// block 0043aa10
0043aa10  fld        dword ptr [esp + 0xc]
0043aa14  push       ecx
0043aa15  fadd       qword ptr [0x4a4358]
0043aa1b  fstp       dword ptr [esp + 0x14]
0043aa1f  fld        dword ptr [esp + 0x14]
0043aa23  fstp       dword ptr [esp]
0043aa26  call       0x4646e0

// block 0043aa2b
0043aa2b  sub        esi, 1
0043aa2e  fstp       dword ptr [esp + 0xc]
0043aa32  jne        0x43a9b0

// block 0043aa38
0043aa38  pop        edi
0043aa39  pop        esi
0043aa3a  xor        eax, eax
0043aa3c  pop        ebx
0043aa3d  add        esp, 0x14
0043aa40  ret        4
