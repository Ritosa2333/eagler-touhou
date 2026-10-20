
// block 00430240
00430240  test       byte ptr [0x4ceae8], 0x10
00430247  push       edi
00430248  push       0
0043024a  mov        edi, 0x4a0a20
0043024f  mov        eax, 0x4cf4e8
00430254  je         0x430261

// block 00430256
00430256  push       4
00430258  call       0x454960

// block 0043025d
0043025d  xor        eax, eax
0043025f  pop        edi
00430260  ret        

// block 00430261
00430261  push       3
00430263  call       0x454960

// block 00430268
00430268  xor        eax, eax
0043026a  pop        edi
0043026b  ret        
