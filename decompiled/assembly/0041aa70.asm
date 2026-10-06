
// block 0041aa70
0041aa70  push       ebp
0041aa71  mov        ebp, esp
0041aa73  and        esp, 0xfffffff8
0041aa76  sub        esp, 0x18
0041aa79  push       ebx
0041aa7a  push       ebp
0041aa7b  push       esi
0041aa7c  push       edi
0041aa7d  mov        ebp, ecx
0041aa7f  mov        edi, dword ptr [0x4b43c8]
0041aa85  add        edi, 0x64
0041aa88  lea        esi, [edi + 0x4c0]
0041aa8e  mov        ebx, 0x7d0

// block 0041aa93
0041aa93  test       byte ptr [edi], 1
0041aa96  je         0x41ab74

// block 0041aa9c
0041aa9c  cmp        word ptr [esi + 0x72], 1
0041aaa1  jne        0x41ab74

// block 0041aaa7
0041aaa7  fld        dword ptr [esi]
0041aaa9  fsub       dword ptr [ebp + 0x38]
0041aaac  fld        dword ptr [esi - 4]
0041aaaf  fsub       dword ptr [ebp + 0x34]
0041aab2  fmul       st(0), st(0)
0041aab4  fld        st(1)
0041aab6  fmulp      st(2)
0041aab8  faddp      st(1)
0041aaba  fstp       dword ptr [esp + 0x10]
0041aabe  fld        dword ptr [esp + 0x10]
0041aac2  call       0x4937c0

// block 0041aac7
0041aac7  fstp       dword ptr [esp + 0x10]
0041aacb  fld        dword ptr [esp + 0x10]
0041aacf  fstp       dword ptr [esp + 0x10]
0041aad3  fld        dword ptr [0x4a3db8]
0041aad9  fcomp      dword ptr [esp + 0x10]
0041aadd  fnstsw     ax
0041aadf  test       ah, 0x41
0041aae2  jne        0x41ab74

// block 0041aae8
0041aae8  fld        dword ptr [ebp + 0x34]
0041aaeb  lea        eax, [esp + 0x14]
0041aaef  fsub       dword ptr [esi - 4]
0041aaf2  push       eax
0041aaf3  mov        ecx, eax
0041aaf5  push       ecx
0041aaf6  fstp       dword ptr [esp + 0x24]
0041aafa  fld        dword ptr [ebp + 0x38]
0041aafd  fsub       dword ptr [esi]
0041aaff  fstp       dword ptr [esp + 0x28]
0041ab03  fld        dword ptr [esp + 0x24]
0041ab07  fstp       dword ptr [esp + 0x1c]
0041ab0b  fld        dword ptr [esp + 0x28]
0041ab0f  fstp       dword ptr [esp + 0x20]
0041ab13  call       0x46c6ce

// block 0041ab18
0041ab18  fld        dword ptr [esp + 0x10]
0041ab1c  fsubr      qword ptr [0x4a3d48]
0041ab22  fdiv       qword ptr [0x4a4018]
0041ab28  fstp       dword ptr [esp + 0x10]
0041ab2c  fld        dword ptr [esp + 0x14]
0041ab30  fld        dword ptr [esp + 0x10]
0041ab34  fld        st(0)
0041ab36  fmulp      st(2)
0041ab38  fld        dword ptr [esi + 8]
0041ab3b  faddp      st(2)
0041ab3d  fxch       st(1)
0041ab3f  fstp       dword ptr [esp + 0x10]
0041ab43  fld        dword ptr [esp + 0x10]
0041ab47  fst        dword ptr [esi + 8]
0041ab4a  fld        dword ptr [esp + 0x18]
0041ab4e  fmulp      st(2)
0041ab50  fld        dword ptr [esi + 0xc]
0041ab53  faddp      st(2)
0041ab55  fxch       st(1)
0041ab57  fstp       dword ptr [esp + 0x10]
0041ab5b  fld        dword ptr [esp + 0x10]
0041ab5f  fst        dword ptr [esi + 0xc]
0041ab62  fxch       st(1)
0041ab64  call       0x4937aa

// block 0041ab69
0041ab69  fstp       dword ptr [esp + 0x10]
0041ab6d  fld        dword ptr [esp + 0x10]
0041ab71  fstp       dword ptr [esi + 0x18]

// block 0041ab74
0041ab74  add        edi, 0x9f8
0041ab7a  add        esi, 0x9f8
0041ab80  sub        ebx, 1
0041ab83  jne        0x41aa93

// block 0041ab89
0041ab89  pop        edi
0041ab8a  xor        eax, eax
0041ab8c  pop        esi
0041ab8d  pop        ebp
0041ab8e  pop        ebx
0041ab8f  mov        esp, ebp
0041ab91  pop        ebp
0041ab92  ret        
