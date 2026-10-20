
// block 00451ed0
00451ed0  push       ecx
00451ed1  cmp        dword ptr [0x4ce55c], 0
00451ed8  push       esi
00451ed9  mov        esi, ecx
00451edb  je         0x451ee5

// block 00451edd
00451edd  mov        eax, 7
00451ee2  pop        esi
00451ee3  pop        ecx
00451ee4  ret        

// block 00451ee5
00451ee5  push       edi
00451ee6  mov        edi, dword ptr [esi + 0x1c]
00451ee9  mov        dword ptr [esp + 8], edi
00451eed  test       edi, edi
00451eef  je         0x451f17

// block 00451ef1
00451ef1  fld        dword ptr [esi + 0x38]
00451ef4  fld        qword ptr [0x4a3ed0]
00451efa  fmul       st(1), st(0)
00451efc  fild       dword ptr [esp + 8]
00451f00  fdivp      st(2)
00451f02  fsubrp     st(1)
00451f04  call       0x4931e0

// block 00451f09
00451f09  mov        dword ptr [esi + 0x18], eax
00451f0c  test       eax, eax
00451f0e  jge        0x451f17

// block 00451f10
00451f10  mov        dword ptr [esi + 0x18], 0

// block 00451f17
00451f17  cmp        dword ptr [esi + 0x34], edi
00451f1a  pop        edi
00451f1b  jge        0x451edd

// block 00451f1d
00451f1d  add        esi, 0x30
00451f20  call       0x464a80

// block 00451f25
00451f25  mov        eax, 1
00451f2a  pop        esi
00451f2b  pop        ecx
00451f2c  ret        
