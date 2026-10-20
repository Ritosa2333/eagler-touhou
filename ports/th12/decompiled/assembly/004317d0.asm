
// block 004317d0
004317d0  push       ebp
004317d1  mov        ebp, esp
004317d3  and        esp, 0xfffffff8
004317d6  sub        esp, 8
004317d9  fld        dword ptr [ebp + 8]
004317dc  call       0x4936b0

// block 004317e1
004317e1  fstp       dword ptr [esp + 4]
004317e5  fld        dword ptr [esp + 4]
004317e9  mov        esp, ebp
004317eb  pop        ebp
004317ec  ret        4
