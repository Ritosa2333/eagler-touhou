
// block 00485b44
00485b44  mov        edi, edi
00485b46  push       esi
00485b47  xor        esi, esi
00485b49  jmp        0x485b6e

// block 00485b4b
00485b4b  mov        al, cl
00485b4d  sub        al, 0x61
00485b4f  inc        edx
00485b50  cmp        al, 5
00485b52  ja         0x485b59

// block 00485b54
00485b54  add        cl, 0xd9
00485b57  jmp        0x485b64

// block 00485b59
00485b59  mov        al, cl
00485b5b  sub        al, 0x41
00485b5d  cmp        al, 5
00485b5f  ja         0x485b64

// block 00485b61
00485b61  add        cl, 0xf9

// block 00485b64
00485b64  movsx      eax, cl
00485b67  shl        esi, 4
00485b6a  lea        esi, [esi + eax - 0x30]

// block 00485b6e
00485b6e  mov        cl, byte ptr [edx]
00485b70  test       cl, cl
00485b72  jne        0x485b4b

// block 00485b74
00485b74  mov        eax, esi
00485b76  pop        esi
00485b77  ret        
