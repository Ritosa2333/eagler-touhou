
// block 00485fc5
00485fc5  push       dword ptr [esi]
00485fc7  call       0x475860

// block 00485fcc
00485fcc  sub        eax, 3
00485fcf  neg        eax
00485fd1  sbb        eax, eax
00485fd3  inc        eax
00485fd4  pop        ecx
00485fd5  mov        dword ptr [esi + 0x10], eax
00485fd8  je         0x485fdf

// block 00485fda
00485fda  push       2
00485fdc  pop        eax
00485fdd  jmp        0x485fe6

// block 00485fdf
00485fdf  mov        edx, dword ptr [esi]
00485fe1  call       0x485b78

// block 00485fe6
00485fe6  push       1
00485fe8  push       0x485e71
00485fed  mov        dword ptr [esi + 0xc], eax
00485ff0  call       dword ptr [0x498194]

// block 00485ff6
00485ff6  test       byte ptr [esi + 8], 4
00485ffa  jne        0x486000

// block 00485ffc
00485ffc  and        dword ptr [esi + 8], 0

// block 00486000
00486000  ret        
