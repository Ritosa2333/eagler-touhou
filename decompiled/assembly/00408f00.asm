
// block 00408f00
00408f00  mov        eax, dword ptr [edi + 0x40]
00408f03  mov        edx, dword ptr [0x4ce8cc]
00408f09  sub        esp, 0x14
00408f0c  push       ebx
00408f0d  push       eax
00408f0e  call       0x461920

// block 00408f13
00408f13  mov        ebx, eax
00408f15  test       ebx, ebx
00408f17  jne        0x408f1c

// block 00408f19
00408f19  mov        dword ptr [edi + 0x40], eax

// block 00408f1c
00408f1c  push       0x28
00408f1e  call       0x406f60

// block 00408f23
00408f23  test       ebx, ebx
00408f25  jne        0x408f44

// block 00408f27
00408f27  mov        ecx, dword ptr [edi + 0x48]
00408f2a  push       ecx
00408f2b  mov        ebx, 1
00408f30  call       0x461970

// block 00408f35
00408f35  mov        dword ptr [edi + 0x40], 0
00408f3c  or         eax, 0xffffffff
00408f3f  pop        ebx
00408f40  add        esp, 0x14
00408f43  ret        

// block 00408f44
00408f44  cmp        dword ptr [edi + 0x18], 0xb4
00408f4b  push       esi
00408f4c  jne        0x408f8e

// block 00408f4e
00408f4e  mov        edx, 6
00408f53  call       0x453d90

// block 00408f58
00408f58  push       0x44
00408f5a  call       0x46c9ea

// block 00408f5f
00408f5f  mov        esi, eax
00408f61  add        esp, 4
00408f64  test       esi, esi
00408f66  je         0x408f8e

// block 00408f68
00408f68  and        dword ptr [esi + 0x40], 0xfffffffe
00408f6c  push       0x44
00408f6e  push       0
00408f70  push       esi
00408f71  call       0x477420

// block 00408f76
00408f76  or         dword ptr [esi], 2
00408f79  add        esp, 0xc
00408f7c  push       0x46
00408f7e  push       0xa
00408f80  push       0x3c
00408f82  push       1
00408f84  push       4
00408f86  push       8
00408f88  push       esi
00408f89  call       0x452a60

// block 00408f8e
00408f8e  mov        eax, dword ptr [edi + 0x18]
00408f91  test       eax, eax
00408f93  jne        0x408fbb

// block 00408f95
00408f95  fld        dword ptr [0x4a4398]
00408f9b  push       0xd
00408f9d  push       0xb4
00408fa2  sub        esp, 8
00408fa5  fstp       dword ptr [esp + 4]
00408fa9  lea        edx, [edi + 0x508]
00408faf  fld        dword ptr [0x4a3e68]
00408fb5  fstp       dword ptr [esp]
00408fb8  push       edx
00408fb9  jmp        0x408fe3

// block 00408fbb
00408fbb  cmp        eax, 0xb4
00408fc0  jne        0x408fed

// block 00408fc2
00408fc2  fld        dword ptr [0x4a4130]
00408fc8  push       0x50
00408fca  push       0x64
00408fcc  sub        esp, 8
00408fcf  fstp       dword ptr [esp + 4]
00408fd3  lea        eax, [edi + 0x508]
00408fd9  fld        dword ptr [0x4a3f00]
00408fdf  fstp       dword ptr [esp]
00408fe2  push       eax

// block 00408fe3
00408fe3  mov        eax, dword ptr [0x4b4514]
00408fe8  call       0x4390f0

// block 00408fed
00408fed  mov        eax, dword ptr [edi + 0x18]
00408ff0  cmp        eax, 0xfa
00408ff5  jge        0x4090eb

// block 00408ffb
00408ffb  cmp        eax, 0x1e
00408ffe  jl         0x4090e0

// block 00409004
00409004  mov        esi, 0x4ce568
00409009  call       0x464440

// block 0040900e
0040900e  mov        dword ptr [esp + 0xc], eax
00409012  fild       dword ptr [esp + 0xc]
00409016  test       eax, eax
00409018  jge        0x409020

// block 0040901a
0040901a  fadd       dword ptr [0x4a3e10]

// block 00409020
00409020  fmul       qword ptr [0x4a3eb0]
00409026  push       ecx
00409027  fstp       dword ptr [esp + 0x10]
0040902b  fld        dword ptr [ebx + 0x40]
0040902e  fstp       dword ptr [esp + 0xc]
00409032  fld        dword ptr [esp + 0xc]
00409036  fmul       dword ptr [esp + 0x10]
0040903a  fstp       dword ptr [esp + 0x10]
0040903e  fld        dword ptr [esp + 0x10]
00409042  fstp       dword ptr [esp]
00409045  call       0x409310

// block 0040904a
0040904a  push       ecx
0040904b  lea        ecx, [esp + 0x18]
0040904f  fstp       dword ptr [esp]
00409052  call       0x4092f0

// block 00409057
00409057  fld        dword ptr [esp + 0x10]
0040905b  fadd       dword ptr [edi + 0x508]
00409061  lea        ecx, [esp + 0x10]
00409065  push       ecx
00409066  push       5
00409068  fstp       dword ptr [esp + 0x18]
0040906c  lea        edx, [esp + 0x14]
00409070  fld        dword ptr [edi + 0x50c]
00409076  push       edx
00409077  fadd       dword ptr [esp + 0x20]
0040907b  fstp       dword ptr [esp + 0x20]
0040907f  fld        dword ptr [edi + 0x510]
00409085  fadd       qword ptr [0x4a3d58]
0040908b  fstp       dword ptr [esp + 0x24]
0040908f  fld        dword ptr [esp + 0x1c]
00409093  fadd       qword ptr [0x4a3d70]
00409099  fadd       qword ptr [0x4a3d28]
0040909f  fstp       dword ptr [esp + 0x1c]
004090a3  fld        dword ptr [esp + 0x20]
004090a7  fadd       qword ptr [0x4a3d68]
004090ad  fstp       dword ptr [esp + 0x20]
004090b1  call       0x40fbe0

// block 004090b6
004090b6  mov        eax, dword ptr [esp + 0xc]
004090ba  mov        edx, dword ptr [0x4ce8cc]
004090c0  push       eax
004090c1  call       0x461920

// block 004090c6
004090c6  mov        ecx, dword ptr [eax + 0x47c]
004090cc  and        ecx, 0xffffff3f
004090d2  or         ecx, 0x20
004090d5  mov        dword ptr [eax + 0x47c], ecx
004090db  jmp        0x4091b7

// block 004090e0
004090e0  cmp        eax, 0xfa
004090e5  jl         0x4091b7

// block 004090eb
004090eb  mov        esi, 0x4ce568
004090f0  call       0x464440

// block 004090f5
004090f5  mov        dword ptr [esp + 0xc], eax
004090f9  fild       dword ptr [esp + 0xc]
004090fd  test       eax, eax
004090ff  jge        0x409107

// block 00409101
00409101  fadd       dword ptr [0x4a3e10]

// block 00409107
00409107  fmul       qword ptr [0x4a3ea8]
0040910d  mov        esi, 0x4ce568
00409112  fsub       qword ptr [0x4a3ce0]
00409118  fstp       dword ptr [esp + 0xc]
0040911c  fld        dword ptr [esp + 0xc]
00409120  fmul       qword ptr [0x4a3d28]
00409126  fstp       dword ptr [esp + 0x10]
0040912a  call       0x464440

// block 0040912f
0040912f  mov        dword ptr [esp + 0xc], eax
00409133  fild       dword ptr [esp + 0xc]
00409137  test       eax, eax
00409139  jge        0x409141

// block 0040913b
0040913b  fadd       dword ptr [0x4a3e10]

// block 00409141
00409141  fmul       qword ptr [0x4a3eb0]
00409147  lea        edx, [esp + 0x10]
0040914b  push       edx
0040914c  push       5
0040914e  fstp       dword ptr [esp + 0x14]
00409152  lea        eax, [esp + 0x14]
00409156  fld        dword ptr [esp + 0x14]
0040915a  push       eax
0040915b  fmul       qword ptr [0x4a3d50]
00409161  fstp       dword ptr [esp + 0x20]
00409165  fldz       
00409167  fstp       dword ptr [esp + 0x24]
0040916b  fld        dword ptr [esp + 0x1c]
0040916f  fadd       qword ptr [0x4a3d70]
00409175  fadd       qword ptr [0x4a3d28]
0040917b  fstp       dword ptr [esp + 0x1c]
0040917f  fld        dword ptr [esp + 0x20]
00409183  fadd       qword ptr [0x4a3d68]
00409189  fstp       dword ptr [esp + 0x20]
0040918d  call       0x40fbe0

// block 00409192
00409192  mov        ecx, dword ptr [esp + 0xc]
00409196  mov        edx, dword ptr [0x4ce8cc]
0040919c  push       ecx
0040919d  call       0x461920

// block 004091a2
004091a2  mov        edx, dword ptr [eax + 0x47c]
004091a8  and        edx, 0xffffff3f
004091ae  or         edx, 0x20
004091b1  mov        dword ptr [eax + 0x47c], edx

// block 004091b7
004091b7  cmp        dword ptr [edi + 0x18], 0x190
004091be  pop        esi
004091bf  jl         0x4091e6

// block 004091c1
004091c1  mov        eax, dword ptr [edi + 0x40]
004091c4  push       eax
004091c5  mov        ebx, 1
004091ca  call       0x461970

// block 004091cf
004091cf  mov        ecx, dword ptr [edi + 0x48]
004091d2  push       ecx
004091d3  call       0x461970

// block 004091d8
004091d8  mov        dword ptr [edi + 0x40], 0
004091df  xor        eax, eax
004091e1  pop        ebx
004091e2  add        esp, 0x14
004091e5  ret        

// block 004091e6
004091e6  mov        eax, edi
004091e8  call       0x409220

// block 004091ed
004091ed  fld        dword ptr [ebx + 0x44]
004091f0  fstp       dword ptr [edi + 0x514]
004091f6  xor        eax, eax
004091f8  pop        ebx
004091f9  add        esp, 0x14
004091fc  ret        
