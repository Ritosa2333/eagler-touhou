
// block 00410020
00410020  sub        esp, 0xc
00410023  push       ebx
00410024  mov        ebx, eax
00410026  push       esi
00410027  mov        esi, dword ptr [ebx]
00410029  test       esi, esi
0041002b  je         0x41015d

// block 00410031
00410031  fld        dword ptr [esp + 0x18]
00410035  lea        eax, [esi - 1]
00410038  fadd       qword ptr [0x4a3d28]
0041003e  mov        edx, dword ptr [ebx + 0x14]
00410041  fadd       qword ptr [0x4a3d70]
00410047  fstp       dword ptr [esp + 8]
0041004b  fld        dword ptr [esp + 0x1c]
0041004f  mov        dword ptr [esp + 0x1c], eax
00410053  fadd       qword ptr [0x4a3d68]
00410059  mov        eax, dword ptr [ebx + 4]
0041005c  lea        ecx, [eax - 1]
0041005f  fstp       dword ptr [esp + 0x18]
00410063  fld        dword ptr [esp + 0x18]
00410067  fstp       dword ptr [esp + 0xc]
0041006b  fldz       
0041006d  fst        dword ptr [esp + 0x10]
00410071  fld        dword ptr [esp + 0x20]
00410075  fidiv      dword ptr [esp + 0x1c]
00410079  mov        dword ptr [esp + 0x1c], ecx
0041007d  mov        ecx, dword ptr [ebx + 0x10]
00410080  fstp       dword ptr [esp + 0x20]
00410084  fld        dword ptr [esp + 0x24]
00410088  fidiv      dword ptr [esp + 0x1c]
0041008c  mov        dword ptr [esp + 0x1c], 0
00410094  fstp       dword ptr [esp + 0x24]
00410098  jle        0x410156

// block 0041009e
0041009e  fld        dword ptr [esp + 0x20]
004100a2  push       ebp
004100a3  fld        dword ptr [esp + 0x28]
004100a7  mov        ebp, dword ptr [esp + 0x14]
004100ab  fld        qword ptr [0x4a3d18]
004100b1  push       edi
004100b2  fld        qword ptr [0x4a3d10]
004100b8  fld1       

// block 004100ba
004100ba  xor        esi, esi
004100bc  test       eax, eax
004100be  jle        0x410127

// block 004100c0
004100c0  mov        edi, dword ptr [esp + 0x10]

// block 004100c4
004100c4  mov        eax, dword ptr [esp + 0x14]
004100c8  mov        dword ptr [edx], edi
004100ca  mov        dword ptr [edx + 4], eax
004100cd  mov        dword ptr [edx + 8], ebp
004100d0  mov        dword ptr [ecx], edi
004100d2  mov        dword ptr [ecx + 4], eax
004100d5  mov        dword ptr [ecx + 8], ebp
004100d8  fld        dword ptr [edx]
004100da  fdiv       st(3)
004100dc  fstp       dword ptr [ecx + 0x14]
004100df  fld        dword ptr [edx + 4]
004100e2  fdiv       st(2)
004100e4  fstp       dword ptr [ecx + 0x18]
004100e7  fxch       st(5)
004100e9  fcom       dword ptr [ecx + 0x14]
004100ec  fnstsw     ax
004100ee  test       ah, 0x41
004100f1  jne        0x4100f6

// block 004100f3
004100f3  fst        dword ptr [ecx + 0x14]

// block 004100f6
004100f6  fcom       dword ptr [ecx + 0x18]
004100f9  fnstsw     ax
004100fb  test       ah, 0x41
004100fe  jne        0x410103

// block 00410100
00410100  fst        dword ptr [ecx + 0x18]

// block 00410103
00410103  fxch       st(5)
00410105  mov        dword ptr [ecx + 0x10], 0xffffffff
0041010c  fst        dword ptr [ecx + 0xc]
0041010f  mov        eax, dword ptr [ebx + 4]
00410112  fld        dword ptr [esp + 0x14]
00410116  inc        esi
00410117  fadd       st(4)
00410119  add        edx, 0xc
0041011c  add        ecx, 0x1c
0041011f  cmp        esi, eax
00410121  fstp       dword ptr [esp + 0x14]
00410125  jl         0x4100c4

// block 00410127
00410127  fld        dword ptr [esp + 0x20]
0041012b  mov        esi, dword ptr [esp + 0x24]
0041012f  fstp       dword ptr [esp + 0x14]
00410133  inc        esi
00410134  cmp        esi, dword ptr [ebx]
00410136  fld        dword ptr [esp + 0x10]
0041013a  fadd       st(5)
0041013c  mov        dword ptr [esp + 0x24], esi
00410140  fstp       dword ptr [esp + 0x10]
00410144  jl         0x4100ba

// block 0041014a
0041014a  fstp       st(5)
0041014c  pop        edi
0041014d  fstp       st(1)
0041014f  pop        ebp
00410150  fstp       st(0)
00410152  fstp       st(0)
00410154  fstp       st(1)

// block 00410156
00410156  fstp       st(0)
00410158  call       0x4101f0

// block 0041015d
0041015d  pop        esi
0041015e  pop        ebx
0041015f  add        esp, 0xc
00410162  ret        0x10
