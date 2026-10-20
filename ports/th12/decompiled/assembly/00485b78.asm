
// block 00485b78
00485b78  xor        eax, eax

// block 00485b7a
00485b7a  mov        cl, byte ptr [edx]
00485b7c  inc        edx
00485b7d  cmp        cl, 0x41
00485b80  jl         0x485b87

// block 00485b82
00485b82  cmp        cl, 0x5a
00485b85  jle        0x485b8f

// block 00485b87
00485b87  sub        cl, 0x61
00485b8a  cmp        cl, 0x19
00485b8d  ja         0x485b92

// block 00485b8f
00485b8f  inc        eax
00485b90  jmp        0x485b7a

// block 00485b92
00485b92  ret        
