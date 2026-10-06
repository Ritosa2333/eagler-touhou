
// block 0040db40
0040db40  mov        eax, 1
0040db45  sub        esp, 0x18
0040db48  test       byte ptr [edi + 0x7c], al
0040db4b  je         0x40dd3e

// block 0040db51
0040db51  add        dword ptr [edi + 0x90], eax
0040db57  cmp        dword ptr [edi + 0x28], 0x3c
0040db5b  jl         0x40db69

// block 0040db5d
0040db5d  mov        eax, dword ptr [0x4b43c0]
0040db62  and        dword ptr [eax + 0x35bc], 0xfffffffe

// block 0040db69
0040db69  cmp        dword ptr [edi + 0x28], 0x12c
0040db70  push       esi
0040db71  jl         0x40dbb7

// block 0040db73
0040db73  test       byte ptr [edi + 0x7c], 8
0040db77  jne        0x40dbb7

// block 0040db79
0040db79  mov        ecx, dword ptr [edi + 0x84]
0040db7f  mov        eax, ecx
0040db81  cdq        
0040db82  and        edx, 3
0040db85  add        edx, eax
0040db87  sar        edx, 2
0040db8a  mov        eax, ecx
0040db8c  mov        ecx, dword ptr [edi + 0x88]
0040db92  sub        eax, edx
0040db94  cdq        
0040db95  sub        ecx, 0x12c
0040db9b  idiv       ecx
0040db9d  mov        ecx, dword ptr [edi + 0x80]
0040dba3  mov        esi, 0xa
0040dba8  sub        ecx, eax
0040dbaa  mov        eax, ecx
0040dbac  cdq        
0040dbad  idiv       esi
0040dbaf  sub        ecx, edx
0040dbb1  mov        dword ptr [edi + 0x80], ecx

// block 0040dbb7
0040dbb7  lea        esi, [edi + 0x24]
0040dbba  call       0x464a80

// block 0040dbbf
0040dbbf  cmp        dword ptr [edi + 0x28], 0x78
0040dbc3  jl         0x40dc4b

// block 0040dbc9
0040dbc9  test       byte ptr [edi + 0x7c], 4
0040dbcd  push       ebx
0040dbce  push       ebp
0040dbcf  jne        0x40dc0d

// block 0040dbd1
0040dbd1  fld        dword ptr [0x4a40c0]
0040dbd7  mov        edx, dword ptr [0x4b4514]
0040dbdd  fcomp      dword ptr [edx + 0x980]
0040dbe3  fnstsw     ax
0040dbe5  test       ah, 0x41
0040dbe8  jne        0x40dc49

// block 0040dbea
0040dbea  lea        esi, [edi + 0x14]
0040dbed  mov        ebp, 3

// block 0040dbf2
0040dbf2  mov        eax, dword ptr [esi]
0040dbf4  push       eax
0040dbf5  mov        ebx, 3
0040dbfa  call       0x461970

// block 0040dbff
0040dbff  add        esi, 4
0040dc02  sub        ebp, 1
0040dc05  jne        0x40dbf2

// block 0040dc07
0040dc07  or         dword ptr [edi + 0x7c], 4
0040dc0b  jmp        0x40dc49

// block 0040dc0d
0040dc0d  fld        dword ptr [0x4a3db8]
0040dc13  mov        ecx, dword ptr [0x4b4514]
0040dc19  fcomp      dword ptr [ecx + 0x980]
0040dc1f  fnstsw     ax
0040dc21  test       ah, 5
0040dc24  jp         0x40dc49

// block 0040dc26
0040dc26  lea        esi, [edi + 0x14]
0040dc29  mov        ebp, 3
0040dc2e  mov        edi, edi

// block 0040dc30
0040dc30  mov        edx, dword ptr [esi]
0040dc32  push       edx
0040dc33  mov        ebx, 2
0040dc38  call       0x461970

// block 0040dc3d
0040dc3d  add        esi, 4
0040dc40  sub        ebp, 1
0040dc43  jne        0x40dc30

// block 0040dc45
0040dc45  and        dword ptr [edi + 0x7c], 0xfffffffb

// block 0040dc49
0040dc49  pop        ebp
0040dc4a  pop        ebx

// block 0040dc4b
0040dc4b  mov        eax, dword ptr [0x4b43dc]
0040dc50  mov        eax, dword ptr [eax + 0x1c]
0040dc53  fld        dword ptr [eax + 0x1074]
0040dc59  add        eax, 0x1074
0040dc5e  fsub       dword ptr [edi + 0xac]
0040dc64  mov        edx, dword ptr [0x4ce8cc]
0040dc6a  fstp       dword ptr [esp + 4]
0040dc6e  fld        dword ptr [eax + 4]
0040dc71  fsub       dword ptr [edi + 0xb0]
0040dc77  fstp       dword ptr [esp + 8]
0040dc7b  fld        dword ptr [eax + 8]
0040dc7e  fsub       dword ptr [edi + 0xb4]
0040dc84  fstp       dword ptr [esp + 0xc]
0040dc88  fld        dword ptr [esp + 4]
0040dc8c  fld        qword ptr [0x4a40e0]
0040dc92  fmul       st(1), st(0)
0040dc94  fxch       st(1)
0040dc96  fstp       dword ptr [esp + 0x10]
0040dc9a  fld        dword ptr [esp + 8]
0040dc9e  fmul       st(1)
0040dca0  fstp       dword ptr [esp + 0x14]
0040dca4  fmul       dword ptr [esp + 0xc]
0040dca8  fstp       dword ptr [esp + 0x18]
0040dcac  fld        dword ptr [edi + 0xac]
0040dcb2  fadd       dword ptr [esp + 0x10]
0040dcb6  fstp       dword ptr [edi + 0xac]
0040dcbc  fld        dword ptr [edi + 0xb0]
0040dcc2  fadd       dword ptr [esp + 0x14]
0040dcc6  fstp       dword ptr [edi + 0xb0]
0040dccc  fld        dword ptr [edi + 0xb4]
0040dcd2  fadd       dword ptr [esp + 0x18]
0040dcd6  fstp       dword ptr [edi + 0xb4]
0040dcdc  mov        eax, dword ptr [edi + 0x20]
0040dcdf  push       eax
0040dce0  call       0x461920

// block 0040dce5
0040dce5  pop        esi
0040dce6  test       eax, eax
0040dce8  je         0x40dd20

// block 0040dcea
0040dcea  fld        dword ptr [edi + 0xac]
0040dcf0  fadd       qword ptr [0x4a3d70]
0040dcf6  fadd       qword ptr [0x4a3d28]
0040dcfc  fstp       dword ptr [eax + 0x430]
0040dd02  fld        dword ptr [edi + 0xb0]
0040dd08  fadd       qword ptr [0x4a3d68]
0040dd0e  fstp       dword ptr [eax + 0x434]
0040dd14  fld        dword ptr [edi + 0xb4]
0040dd1a  fstp       dword ptr [eax + 0x438]

// block 0040dd20
0040dd20  mov        eax, dword ptr [edi + 0x7c]
0040dd23  test       al, 0x20
0040dd25  je         0x40dd39

// block 0040dd27
0040dd27  mov        ecx, dword ptr [0x4b43c4]
0040dd2d  cmp        dword ptr [ecx + 0x3c], 0
0040dd31  jne        0x40dd39

// block 0040dd33
0040dd33  and        eax, 0xffffffdf
0040dd36  mov        dword ptr [edi + 0x7c], eax

// block 0040dd39
0040dd39  mov        eax, 1

// block 0040dd3e
0040dd3e  add        esp, 0x18
0040dd41  ret        
