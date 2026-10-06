
// block 0045d970
0045d970  push       ebp
0045d971  mov        ebp, esp
0045d973  and        esp, 0xffffffc0
0045d976  sub        esp, 0x40
0045d979  fld        dword ptr [ebp + 8]
0045d97c  sub        esp, 8
0045d97f  fstp       qword ptr [esp]
0045d982  call       0x493290

// block 0045d987
0045d987  fstp       dword ptr [esp + 0x44]
0045d98b  add        esp, 8
0045d98e  fld        dword ptr [esp + 0x3c]
0045d992  mov        esp, ebp
0045d994  pop        ebp
0045d995  ret        4
