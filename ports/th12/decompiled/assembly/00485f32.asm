
// block 00485f32
00485f32  push       dword ptr [esi + 4]
00485f35  call       0x475860

// block 00485f3a
00485f3a  sub        eax, 3
00485f3d  neg        eax
00485f3f  pop        ecx
00485f40  sbb        eax, eax
00485f42  push       1
00485f44  inc        eax
00485f45  push       0x485b93
00485f4a  mov        dword ptr [esi + 0x14], eax
00485f4d  call       dword ptr [0x498194]

// block 00485f53
00485f53  test       byte ptr [esi + 8], 4
00485f57  jne        0x485f5d

// block 00485f59
00485f59  and        dword ptr [esi + 8], 0

// block 00485f5d
00485f5d  ret        
