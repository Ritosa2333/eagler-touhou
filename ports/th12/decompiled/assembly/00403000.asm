
// block 00403000
00403000  test       esi, esi
00403002  je         0x403011

// block 00403004
00403004  push       esi
00403005  call       0x402cc0

// block 0040300a
0040300a  push       esi
0040300b  call       0x46ca4f

// block 00403010
00403010  pop        ecx

// block 00403011
00403011  ret        
