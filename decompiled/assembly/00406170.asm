
// block 00406170
00406170  push       ebp
00406171  mov        ebp, esp
00406173  and        esp, 0xfffffff8
00406176  sub        esp, 8
00406179  fld        dword ptr [ebp + 8]
0040617c  call       0x4939f0

// block 00406181
00406181  fstp       dword ptr [esp + 4]
00406185  fld        dword ptr [esp + 4]
00406189  mov        esp, ebp
0040618b  pop        ebp
0040618c  ret        4
