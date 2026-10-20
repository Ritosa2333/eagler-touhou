
// block 004088c0
004088c0  push       ebp
004088c1  mov        ebp, esp
004088c3  and        esp, 0xfffffff8
004088c6  sub        esp, 8
004088c9  fld        dword ptr [ebp + 8]
004088cc  fld        dword ptr [ebp + 0xc]
004088cf  call       0x4937aa

// block 004088d4
004088d4  fstp       dword ptr [esp + 4]
004088d8  fld        dword ptr [esp + 4]
004088dc  mov        esp, ebp
004088de  pop        ebp
004088df  ret        8
