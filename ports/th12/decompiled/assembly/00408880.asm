
// block 00408880
00408880  push       ebp
00408881  mov        ebp, esp
00408883  and        esp, 0xfffffff8
00408886  sub        esp, 8
00408889  fld        dword ptr [eax + 4]
0040888c  fld        dword ptr [eax]
0040888e  fmul       st(0), st(0)
00408890  fld        st(1)
00408892  fmulp      st(2)
00408894  faddp      st(1)
00408896  fstp       dword ptr [esp + 4]
0040889a  fld        dword ptr [esp + 4]
0040889e  call       0x4937c0

// block 004088a3
004088a3  fstp       dword ptr [esp + 4]
004088a7  fld        dword ptr [esp + 4]
004088ab  mov        esp, ebp
004088ad  pop        ebp
004088ae  ret        
