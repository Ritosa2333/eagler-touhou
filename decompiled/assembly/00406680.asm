
// block 00406680
00406680  push       ebp
00406681  mov        ebp, esp
00406683  and        esp, 0xfffffff8
00406686  sub        esp, 8
00406689  fld        dword ptr [ebp + 8]
0040668c  call       0x4938c0

// block 00406691
00406691  fstp       dword ptr [esp + 4]
00406695  fld        dword ptr [esp + 4]
00406699  mov        esp, ebp
0040669b  pop        ebp
0040669c  ret        4
