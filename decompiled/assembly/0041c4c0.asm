
// block 0041c4c0
0041c4c0  push       ebp
0041c4c1  mov        ebp, esp
0041c4c3  and        esp, 0xfffffff8
0041c4c6  sub        esp, 8
0041c4c9  fld        dword ptr [ecx + 4]
0041c4cc  fsub       dword ptr [eax + 4]
0041c4cf  fld        dword ptr [ecx]
0041c4d1  fsub       dword ptr [eax]
0041c4d3  fmul       st(0), st(0)
0041c4d5  fld        st(1)
0041c4d7  fmulp      st(2)
0041c4d9  faddp      st(1)
0041c4db  fstp       dword ptr [esp + 4]
0041c4df  fld        dword ptr [esp + 4]
0041c4e3  call       0x4937c0

// block 0041c4e8
0041c4e8  fstp       dword ptr [esp + 4]
0041c4ec  fld        dword ptr [esp + 4]
0041c4f0  mov        esp, ebp
0041c4f2  pop        ebp
0041c4f3  ret        
