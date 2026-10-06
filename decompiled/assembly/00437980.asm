
// block 00437980
00437980  push       ebp
00437981  mov        ebp, esp
00437983  and        esp, 0xfffffff8
00437986  push       ecx
00437987  push       esi
00437988  mov        esi, dword ptr [0x4b4514]
0043798e  fld        dword ptr [esi + 0x980]
00437994  fsub       dword ptr [eax + 4]
00437997  fld        dword ptr [esi + 0x97c]
0043799d  fsub       dword ptr [eax]
0043799f  mov        eax, dword ptr [esi + 0xa2c]
004379a5  fmul       st(0), st(0)
004379a7  fld        st(1)
004379a9  fmulp      st(2)
004379ab  faddp      st(1)
004379ad  fstp       dword ptr [esp + 4]
004379b1  fld        dword ptr [eax + 4]
004379b4  fld        dword ptr [ebp + 8]
004379b7  fld        st(0)
004379b9  fmul       st(0), st(0)
004379bb  fld        dword ptr [esp + 4]
004379bf  fld        st(3)
004379c1  fmulp      st(4)
004379c3  fxch       st(3)
004379c5  fadd       st(1)
004379c7  fcomp      st(3)
004379c9  fnstsw     ax
004379cb  test       ah, 0x41
004379ce  jp         0x437a24

// block 004379d0
004379d0  fxch       st(1)
004379d2  fdiv       qword ptr [0x4a4238]
004379d8  fstp       dword ptr [esp + 4]
004379dc  fld        dword ptr [0x4a4010]
004379e2  fcom       dword ptr [esp + 4]
004379e6  fnstsw     ax
004379e8  test       ah, 0x41
004379eb  jne        0x4379f3

// block 004379ed
004379ed  fstp       dword ptr [esp + 4]
004379f1  jmp        0x4379f5

// block 004379f3
004379f3  fstp       st(0)

// block 004379f5
004379f5  mov        ecx, dword ptr [esi + 0xa2c]
004379fb  fld        dword ptr [ecx + 4]
004379fe  fadd       dword ptr [esp + 4]
00437a02  fmul       st(0), st(0)
00437a04  faddp      st(1)
00437a06  fcompp     
00437a08  fnstsw     ax
00437a0a  test       ah, 0x41
00437a0d  jp         0x437a18

// block 00437a0f
00437a0f  xor        eax, eax
00437a11  pop        esi
00437a12  mov        esp, ebp
00437a14  pop        ebp
00437a15  ret        4

// block 00437a18
00437a18  mov        eax, 2
00437a1d  pop        esi
00437a1e  mov        esp, ebp
00437a20  pop        ebp
00437a21  ret        4

// block 00437a24
00437a24  mov        eax, dword ptr [0x4b43e4]
00437a29  fstp       st(0)
00437a2b  fstp       st(0)
00437a2d  fstp       st(0)
00437a2f  test       eax, eax
00437a31  je         0x437a3c

// block 00437a33
00437a33  cmp        dword ptr [eax + 0x6d30], 0
00437a3a  jne        0x437a0f

// block 00437a3c
00437a3c  mov        eax, dword ptr [esi + 0xa28]
00437a42  cmp        eax, 2
00437a45  je         0x437a0f

// block 00437a47
00437a47  cmp        eax, 4
00437a4a  je         0x437a0f

// block 00437a4c
00437a4c  cmp        eax, 3
00437a4f  je         0x437a0f

// block 00437a51
00437a51  test       byte ptr [esi + 0xc414], 2
00437a58  jne        0x437a0f

// block 00437a5a
00437a5a  cmp        dword ptr [esi + 0xc404], 0
00437a61  jg         0x437a68

// block 00437a63
00437a63  call       0x438370

// block 00437a68
00437a68  mov        eax, 1
00437a6d  pop        esi
00437a6e  mov        esp, ebp
00437a70  pop        ebp
00437a71  ret        4
