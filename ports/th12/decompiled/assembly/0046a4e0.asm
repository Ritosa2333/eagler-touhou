
// block 0046a4e0
0046a4e0  push       ebp
0046a4e1  mov        ebp, esp
0046a4e3  and        esp, 0xfffffff8
0046a4e6  sub        esp, 8
0046a4e9  fld        dword ptr [eax + 4]
0046a4ec  fsub       dword ptr [ecx + 4]
0046a4ef  fstp       dword ptr [esp + 4]
0046a4f3  fld        dword ptr [esp + 4]
0046a4f7  fld        dword ptr [eax]
0046a4f9  fsub       dword ptr [ecx]
0046a4fb  fstp       dword ptr [esp + 4]
0046a4ff  fld        dword ptr [esp + 4]
0046a503  call       0x4937aa

// block 0046a508
0046a508  fstp       dword ptr [esp + 4]
0046a50c  fld        dword ptr [esp + 4]
0046a510  mov        esp, ebp
0046a512  pop        ebp
0046a513  ret        
