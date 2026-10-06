
// block 00485b20
00485b20  mov        edi, edi
00485b22  push       ebp
00485b23  mov        ebp, esp
00485b25  xor        eax, eax

// block 00485b27
00485b27  mov        cx, word ptr [ebp + 8]
00485b2b  cmp        cx, word ptr [eax + 0x49f2d8]
00485b32  je         0x485b40

// block 00485b34
00485b34  inc        eax
00485b35  inc        eax
00485b36  cmp        eax, 0x14
00485b39  jb         0x485b27

// block 00485b3b
00485b3b  xor        eax, eax
00485b3d  inc        eax
00485b3e  pop        ebp
00485b3f  ret        

// block 00485b40
00485b40  xor        eax, eax
00485b42  pop        ebp
00485b43  ret        
