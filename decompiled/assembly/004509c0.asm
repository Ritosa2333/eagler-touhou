
// block 004509c0
004509c0  mov        eax, dword ptr [0x4cf408]
004509c5  sub        esp, 8
004509c8  or         eax, dword ptr [0x4cf40c]
004509ce  je         0x450a01

// block 004509d0
004509d0  lea        ecx, [esp]
004509d3  push       ecx
004509d4  call       dword ptr [0x4980d4]

// block 004509da
004509da  mov        edx, dword ptr [esp]
004509dd  sub        edx, dword ptr [0x4cf410]
004509e3  mov        eax, dword ptr [esp + 4]
004509e7  sbb        eax, dword ptr [0x4cf414]
004509ed  mov        dword ptr [esp], edx
004509f0  mov        dword ptr [esp + 4], eax
004509f4  fild       qword ptr [esp]
004509f7  fild       qword ptr [0x4cf408]
004509fd  fdivp      st(1)
004509ff  jmp        0x450a1d

// block 00450a01
00450a01  call       dword ptr [0x498298]

// block 00450a07
00450a07  mov        dword ptr [esp], eax
00450a0a  fild       dword ptr [esp]
00450a0d  test       eax, eax
00450a0f  jge        0x450a17

// block 00450a11
00450a11  fadd       qword ptr [0x4a3d38]

// block 00450a17
00450a17  fdiv       qword ptr [0x4a3d30]

// block 00450a1d
00450a1d  fld        qword ptr [0x4cf448]
00450a23  fcom       st(1)
00450a25  fnstsw     ax
00450a27  test       ah, 0x41
00450a2a  jne        0x450a36

// block 00450a2c
00450a2c  fstp       st(0)
00450a2e  fld        st(0)
00450a30  fst        qword ptr [0x4cf448]

// block 00450a36
00450a36  fsubp      st(1)
00450a38  add        esp, 8
00450a3b  ret        
