
// block 0047315e
0047315e  push       3
00473160  call       0x4831b2

// block 00473165
00473165  pop        ecx
00473166  cmp        eax, 1
00473169  je         0x473180

// block 0047316b
0047316b  push       3
0047316d  call       0x4831b2

// block 00473172
00473172  pop        ecx
00473173  test       eax, eax
00473175  jne        0x473196

// block 00473177
00473177  cmp        dword ptr [0x4ad134], 1
0047317e  jne        0x473196

// block 00473180
00473180  push       0xfc
00473185  call       0x472f8d

// block 0047318a
0047318a  push       0xff
0047318f  call       0x472f8d

// block 00473194
00473194  pop        ecx
00473195  pop        ecx

// block 00473196
00473196  ret        
