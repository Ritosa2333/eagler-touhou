
// block 0046a970
0046a970  push       ecx
0046a971  mov        eax, dword ptr [edi + 0x40]
0046a974  mov        edx, dword ptr [0x4ce8cc]
0046a97a  push       ebx
0046a97b  push       ebp
0046a97c  push       esi
0046a97d  push       eax
0046a97e  call       0x461920

// block 0046a983
0046a983  mov        esi, eax
0046a985  xor        ebp, ebp
0046a987  cmp        esi, ebp
0046a989  jne        0x46a98e

// block 0046a98b
0046a98b  mov        dword ptr [edi + 0x40], ebp

// block 0046a98e
0046a98e  push       0x28
0046a990  call       0x406f60

// block 0046a995
0046a995  cmp        esi, ebp
0046a997  jne        0x46a9db

// block 0046a999
0046a999  mov        esi, dword ptr [edi + 0x504]
0046a99f  cmp        esi, ebp
0046a9a1  je         0x46a9b1

// block 0046a9a3
0046a9a3  call       0x402870

// block 0046a9a8
0046a9a8  push       esi
0046a9a9  call       0x46ca4f

// block 0046a9ae
0046a9ae  add        esp, 4

// block 0046a9b1
0046a9b1  fld1       
0046a9b3  mov        ecx, dword ptr [0x4b4514]
0046a9b9  mov        dword ptr [edi + 0x504], ebp
0046a9bf  fstp       dword ptr [ecx + 0x825c]
0046a9c5  mov        edx, dword ptr [edi + 0x48]
0046a9c8  push       edx
0046a9c9  mov        ebx, 1
0046a9ce  call       0x461970

// block 0046a9d3
0046a9d3  pop        esi
0046a9d4  pop        ebp
0046a9d5  or         eax, 0xffffffff
0046a9d8  pop        ebx
0046a9d9  pop        ecx
0046a9da  ret        

// block 0046a9db
0046a9db  mov        eax, dword ptr [0x4b4514]
0046a9e0  fld        dword ptr [0x4a4088]
0046a9e6  mov        ecx, dword ptr [eax + 0x97c]
0046a9ec  fstp       dword ptr [eax + 0x825c]
0046a9f2  fld        dword ptr [0x4a434c]
0046a9f8  mov        dword ptr [edi + 0x508], ecx
0046a9fe  mov        edx, dword ptr [eax + 0x980]
0046aa04  mov        dword ptr [edi + 0x50c], edx
0046aa0a  mov        ecx, dword ptr [eax + 0x984]
0046aa10  mov        dword ptr [edi + 0x510], ecx
0046aa16  mov        edx, dword ptr [eax + 0x97c]
0046aa1c  sub        esp, 0x10
0046aa1f  fstp       dword ptr [esp + 0xc]
0046aa23  mov        dword ptr [esi + 0x430], edx
0046aa29  mov        ecx, dword ptr [eax + 0x980]
0046aa2f  fld        dword ptr [0x4a3db8]
0046aa35  fstp       dword ptr [esp + 8]
0046aa39  mov        dword ptr [esi + 0x434], ecx
0046aa3f  mov        edx, dword ptr [eax + 0x984]
0046aa45  fldz       
0046aa47  fstp       dword ptr [esp + 4]
0046aa4b  mov        dword ptr [esi + 0x438], edx
0046aa51  fld        dword ptr [edi + 0x508]
0046aa57  mov        eax, dword ptr [edi + 0x504]
0046aa5d  fsub       qword ptr [0x4a3d20]
0046aa63  fstp       dword ptr [esp + 0x1c]
0046aa67  fld        dword ptr [esp + 0x1c]
0046aa6b  fstp       dword ptr [esp]
0046aa6e  call       0x410020

// block 0046aa73
0046aa73  mov        eax, dword ptr [edi + 0x504]
0046aa79  cmp        dword ptr [eax + 4], ebp
0046aa7c  mov        ebx, dword ptr [eax + 0x10]
0046aa7f  jle        0x46abb0

// block 0046aa85
0046aa85  jmp        0x46aa90

// block 0046aa90
0046aa90  mov        eax, dword ptr [edi + 0x504]
0046aa96  mov        eax, dword ptr [eax + 4]
0046aa99  lea        edx, [eax*8]
0046aaa0  sub        edx, eax
0046aaa2  mov        ecx, 0xff7070a0
0046aaa7  mov        dword ptr [ebx + edx*4 + 0x10], ecx
0046aaab  mov        eax, dword ptr [edi + 0x504]
0046aab1  mov        eax, dword ptr [eax + 4]
0046aab4  lea        edx, [eax*8]
0046aabb  sub        edx, eax
0046aabd  mov        esi, 0x4ce568
0046aac2  mov        dword ptr [ebx + edx*8 + 0x10], ecx
0046aac6  call       0x464440

// block 0046aacb
0046aacb  mov        dword ptr [esp + 0xc], eax
0046aacf  fild       dword ptr [esp + 0xc]
0046aad3  test       eax, eax
0046aad5  jge        0x46aadd

// block 0046aad7
0046aad7  fadd       dword ptr [0x4a3e10]

// block 0046aadd
0046aadd  fmul       qword ptr [0x4a3ea8]
0046aae3  mov        ecx, dword ptr [edi + 0x504]
0046aae9  mov        eax, dword ptr [ecx + 4]
0046aaec  mov        ecx, dword ptr [ecx + 0x14]
0046aaef  fsub       qword ptr [0x4a3ce0]
0046aaf5  lea        edx, [eax + ebp]
0046aaf8  lea        edx, [edx + edx*2]
0046aafb  mov        esi, 0x4ce568
0046ab00  fstp       dword ptr [esp + 0xc]
0046ab04  fld        dword ptr [esp + 0xc]
0046ab08  fmul       qword ptr [0x4a4078]
0046ab0e  fstp       dword ptr [esp + 0xc]
0046ab12  fld        dword ptr [ecx + edx*4]
0046ab15  lea        edx, [eax*8]
0046ab1c  fsub       qword ptr [0x4a3d68]
0046ab22  sub        edx, eax
0046ab24  fadd       dword ptr [esp + 0xc]
0046ab28  fstp       dword ptr [esp + 0xc]
0046ab2c  fld        dword ptr [esp + 0xc]
0046ab30  fstp       dword ptr [ebx + edx*4]
0046ab33  call       0x464440

// block 0046ab38
0046ab38  mov        dword ptr [esp + 0xc], eax
0046ab3c  fild       dword ptr [esp + 0xc]
0046ab40  test       eax, eax
0046ab42  jge        0x46ab4a

// block 0046ab44
0046ab44  fadd       dword ptr [0x4a3e10]

// block 0046ab4a
0046ab4a  fmul       qword ptr [0x4a3ea8]
0046ab50  mov        ecx, dword ptr [edi + 0x504]
0046ab56  mov        eax, dword ptr [ecx + 4]
0046ab59  mov        ecx, dword ptr [ecx + 0x14]
0046ab5c  fsub       qword ptr [0x4a3ce0]
0046ab62  lea        edx, [ebp + eax*2]
0046ab66  lea        edx, [edx + edx*2]
0046ab69  inc        ebp
0046ab6a  fstp       dword ptr [esp + 0xc]
0046ab6e  add        ebx, 0x1c
0046ab71  fld        dword ptr [esp + 0xc]
0046ab75  fmul       qword ptr [0x4a4078]
0046ab7b  fstp       dword ptr [esp + 0xc]
0046ab7f  fld        dword ptr [ecx + edx*4]
0046ab82  lea        edx, [eax*8]
0046ab89  fadd       qword ptr [0x4a3d68]
0046ab8f  sub        edx, eax
0046ab91  fadd       dword ptr [esp + 0xc]
0046ab95  fstp       dword ptr [esp + 0xc]
0046ab99  fld        dword ptr [esp + 0xc]
0046ab9d  fstp       dword ptr [ebx + edx*8 - 0x1c]
0046aba1  mov        eax, dword ptr [edi + 0x504]
0046aba7  cmp        ebp, dword ptr [eax + 4]
0046abaa  jl         0x46aa90

// block 0046abb0
0046abb0  mov        eax, edi
0046abb2  call       0x46abe0

// block 0046abb7
0046abb7  pop        esi
0046abb8  pop        ebp
0046abb9  xor        eax, eax
0046abbb  pop        ebx
0046abbc  pop        ecx
0046abbd  ret        
