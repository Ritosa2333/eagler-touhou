
// block 0043ac30
0043ac30  push       ebp
0043ac31  mov        ebp, esp
0043ac33  and        esp, 0xfffffff8
0043ac36  sub        esp, 8
0043ac39  fld        dword ptr [eax + 4]
0043ac3c  fld        dword ptr [eax]
0043ac3e  fmul       st(0), st(0)
0043ac40  fld        st(1)
0043ac42  fmulp      st(2)
0043ac44  faddp      st(1)
0043ac46  fstp       dword ptr [esp + 4]
0043ac4a  fld        dword ptr [esp + 4]
0043ac4e  call       0x4937c0

// block 0043ac53
0043ac53  fstp       dword ptr [esp + 4]
0043ac57  fld        dword ptr [esp + 4]
0043ac5b  mov        esp, ebp
0043ac5d  pop        ebp
0043ac5e  ret        
