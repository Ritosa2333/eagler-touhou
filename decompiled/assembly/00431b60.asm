
// block 00431b60
00431b60  push       ecx
00431b61  mov        eax, dword ptr [0x4b43e0]
00431b66  fld        qword ptr [eax + 0x24]
00431b69  fdiv       qword ptr [eax + 0x2c]
00431b6c  fstp       dword ptr [esp]
00431b6f  fld        dword ptr [esp]
00431b72  fld        qword ptr [0x4a3ce8]
00431b78  fmul       st(1), st(0)
00431b7a  fsubrp     st(1)
00431b7c  fstp       dword ptr [esp]
00431b7f  fld        dword ptr [esp]
00431b82  pop        ecx
00431b83  ret        
