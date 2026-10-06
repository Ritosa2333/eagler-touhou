
// block 0043ac60
0043ac60  push       ebp
0043ac61  mov        ebp, esp
0043ac63  and        esp, 0xfffffff8
0043ac66  sub        esp, 8
0043ac69  fld        dword ptr [eax + 0x10]
0043ac6c  fld        dword ptr [eax + 0xc]
0043ac6f  fmul       st(0), st(0)
0043ac71  fld        st(1)
0043ac73  fmulp      st(2)
0043ac75  faddp      st(1)
0043ac77  fstp       dword ptr [esp + 4]
0043ac7b  fld        dword ptr [esp + 4]
0043ac7f  call       0x4937c0

// block 0043ac84
0043ac84  fstp       dword ptr [esp + 4]
0043ac88  fld        dword ptr [esp + 4]
0043ac8c  mov        esp, ebp
0043ac8e  pop        ebp
0043ac8f  ret        
