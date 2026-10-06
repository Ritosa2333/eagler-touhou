
// block 004933ca
004933ca  mov        edx, 0x4b3510
004933cf  jmp        0x493ec0

// block 00493ec0
00493ec0  push       ebp
00493ec1  mov        ebp, esp
00493ec3  add        esp, 0xfffffd30
00493ec9  push       ebx
00493eca  wait       
00493ecb  fnstcw     word ptr [ebp - 0xa4]
00493ed1  wait       
00493ed2  cmp        dword ptr [0x4b37a0], 0
00493ed9  je         0x493eef

// block 00493edb
00493edb  call       0x4941a7

// block 00493ee0
00493ee0  or         byte ptr [ebp - 0x2c8], 3
00493ee7  call       0x493f8a

// block 00493eec
00493eec  pop        ebx
00493eed  leave      
00493eee  ret        

// block 00493eef
00493eef  fxch       st(1)
00493ef1  fst        qword ptr [ebp - 0x86]
00493ef7  fxch       st(1)
00493ef9  fst        qword ptr [ebp - 0x7e]
00493efc  jmp        0x493edb
