
// block 00485f5e
00485f5e  push       dword ptr [esi]
00485f60  call       0x475860

// block 00485f65
00485f65  push       dword ptr [esi + 4]
00485f68  sub        eax, 3
00485f6b  neg        eax
00485f6d  sbb        eax, eax
00485f6f  inc        eax
00485f70  mov        dword ptr [esi + 0x10], eax
00485f73  call       0x475860

// block 00485f78
00485f78  sub        eax, 3
00485f7b  neg        eax
00485f7d  sbb        eax, eax
00485f7f  and        dword ptr [esi + 0x18], 0
00485f83  inc        eax
00485f84  cmp        dword ptr [esi + 0x10], 0
00485f88  pop        ecx
00485f89  pop        ecx
00485f8a  mov        dword ptr [esi + 0x14], eax
00485f8d  je         0x485f94

// block 00485f8f
00485f8f  push       2
00485f91  pop        eax
00485f92  jmp        0x485f9b

// block 00485f94
00485f94  mov        edx, dword ptr [esi]
00485f96  call       0x485b78

// block 00485f9b
00485f9b  push       1
00485f9d  push       0x485c9f
00485fa2  mov        dword ptr [esi + 0xc], eax
00485fa5  call       dword ptr [0x498194]

// block 00485fab
00485fab  mov        eax, dword ptr [esi + 8]
00485fae  test       eax, 0x100
00485fb3  je         0x485fc0

// block 00485fb5
00485fb5  test       eax, 0x200
00485fba  je         0x485fc0

// block 00485fbc
00485fbc  test       al, 7
00485fbe  jne        0x485fc4

// block 00485fc0
00485fc0  and        dword ptr [esi + 8], 0

// block 00485fc4
00485fc4  ret        
