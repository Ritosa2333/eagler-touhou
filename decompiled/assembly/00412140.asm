
// block 00412140
00412140  sub        esp, 0x1c
00412143  push       ebx
00412144  push       ebp
00412145  mov        ebp, dword ptr [esp + 0x28]
00412149  push       esi
0041214a  mov        esi, 0x4ce568
0041214f  call       0x464440

// block 00412154
00412154  mov        dword ptr [esp + 0x2c], eax
00412158  fild       dword ptr [esp + 0x2c]
0041215c  test       eax, eax
0041215e  jge        0x412166

// block 00412160
00412160  fadd       dword ptr [0x4a3e10]

// block 00412166
00412166  fmul       qword ptr [0x4a3ea8]
0041216c  lea        eax, [ebp + 4]
0041216f  mov        dword ptr [esp + 0x10], 0
00412177  mov        dword ptr [esp + 0x18], eax
0041217b  fsub       qword ptr [0x4a3ce0]
00412181  fstp       dword ptr [esp + 0x2c]
00412185  fld        dword ptr [esp + 0x2c]
00412189  mov        dword ptr [esp + 0x2c], eax
0041218d  fmul       qword ptr [0x4a3cd8]
00412193  fstp       dword ptr [esp + 0xc]

// block 00412197
00412197  mov        eax, dword ptr [esp + 0x2c]
0041219b  xor        ebx, ebx
0041219d  cmp        dword ptr [eax], ebx
0041219f  jle        0x4122d2

// block 004121a5
004121a5  fld        dword ptr [ebp + 0x54]
004121a8  sub        esp, 0xc
004121ab  fstp       dword ptr [esp + 8]
004121af  lea        ecx, [esp + 0x28]
004121b3  fld        dword ptr [ebp + 0x50]
004121b6  fstp       dword ptr [esp + 4]
004121ba  fld        dword ptr [esp + 0x18]
004121be  fstp       dword ptr [esp]
004121c1  call       0x41c9d0

// block 004121c6
004121c6  mov        esi, 0x4ce568
004121cb  call       0x464440

// block 004121d0
004121d0  mov        dword ptr [esp + 0x14], eax
004121d4  fild       dword ptr [esp + 0x14]
004121d8  test       eax, eax
004121da  jge        0x4121e2

// block 004121dc
004121dc  fadd       dword ptr [0x4a3e10]

// block 004121e2
004121e2  fmul       qword ptr [0x4a3eb0]
004121e8  mov        eax, dword ptr [esp + 0x10]
004121ec  sub        esp, 8
004121ef  lea        ecx, [esp + 0x24]
004121f3  fstp       dword ptr [esp + 0x1c]
004121f7  inc        eax
004121f8  fld        dword ptr [esp + 0x1c]
004121fc  fld        qword ptr [0x4a3cf8]
00412202  fmul       st(1), st(0)
00412204  faddp      st(1)
00412206  fstp       dword ptr [esp + 0x1c]
0041220a  fld        dword ptr [esp + 0x24]
0041220e  fld        dword ptr [esp + 0x1c]
00412212  fld        st(0)
00412214  fmulp      st(2)
00412216  fxch       st(1)
00412218  fstp       dword ptr [esp + 0x24]
0041221c  fmul       dword ptr [esp + 0x28]
00412220  fstp       dword ptr [esp + 0x28]
00412224  fld        dword ptr [edi]
00412226  fadd       dword ptr [esp + 0x24]
0041222a  fstp       dword ptr [esp + 0x24]
0041222e  fld        dword ptr [edi + 4]
00412231  fadd       dword ptr [esp + 0x28]
00412235  fstp       dword ptr [esp + 0x28]
00412239  fld        dword ptr [edi + 8]
0041223c  fadd       qword ptr [0x4a3d58]
00412242  fstp       dword ptr [esp + 0x2c]
00412246  fld        dword ptr [0x4a42c0]
0041224c  fstp       dword ptr [esp + 4]
00412250  fld        dword ptr [0x4a4060]
00412256  fstp       dword ptr [esp]
00412259  push       ecx
0041225a  push       eax
0041225b  call       0x4273f0

// block 00412260
00412260  mov        esi, 0x4ce568
00412265  call       0x464440

// block 0041226a
0041226a  mov        dword ptr [esp + 0x14], eax
0041226e  fild       dword ptr [esp + 0x14]
00412272  test       eax, eax
00412274  jge        0x41227c

// block 00412276
00412276  fadd       dword ptr [0x4a3e10]

// block 0041227c
0041227c  fmul       qword ptr [0x4a3ea8]
00412282  push       ecx
00412283  fsub       qword ptr [0x4a3ce0]
00412289  fstp       dword ptr [esp + 0x18]
0041228d  fld        dword ptr [esp + 0x18]
00412291  fmul       qword ptr [0x4a3cd8]
00412297  fstp       dword ptr [esp + 0x18]
0041229b  fld        dword ptr [esp + 0x18]
0041229f  fmul       qword ptr [0x4a42b8]
004122a5  fld        dword ptr [esp + 0x10]
004122a9  fadd       qword ptr [0x4a4058]
004122af  faddp      st(1)
004122b1  fstp       dword ptr [esp + 0x18]
004122b5  fld        dword ptr [esp + 0x18]
004122b9  fstp       dword ptr [esp]
004122bc  call       0x4646e0

// block 004122c1
004122c1  mov        edx, dword ptr [esp + 0x2c]
004122c5  fstp       dword ptr [esp + 0xc]
004122c9  inc        ebx
004122ca  cmp        ebx, dword ptr [edx]
004122cc  jl         0x4121a5

// block 004122d2
004122d2  mov        eax, dword ptr [esp + 0x10]
004122d6  add        dword ptr [esp + 0x2c], 4
004122db  inc        eax
004122dc  cmp        eax, 0x12
004122df  mov        dword ptr [esp + 0x10], eax
004122e3  jl         0x412197

// block 004122e9
004122e9  mov        eax, dword ptr [esp + 0x18]
004122ed  push       0x4c
004122ef  push       0
004122f1  push       eax
004122f2  call       0x477420

// block 004122f7
004122f7  add        esp, 0xc
004122fa  pop        esi
004122fb  pop        ebp
004122fc  pop        ebx
004122fd  add        esp, 0x1c
00412300  ret        4
