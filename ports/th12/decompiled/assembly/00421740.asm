
// block 00421740
00421740  push       ecx
00421741  cmp        dword ptr [0x4cee40], 8
00421748  je         0x421775

// block 0042174a
0042174a  test       byte ptr [0x4b0ce0], 0x20
00421751  jne        0x421775

// block 00421753
00421753  mov        ecx, dword ptr [0x4b43e4]
00421759  mov        edx, dword ptr [ecx + 0x6ce4]
0042175f  push       0
00421761  push       0
00421763  lea        eax, [esp + 8]
00421767  push       eax
00421768  push       edx
00421769  mov        eax, 0x17
0042176e  xor        ecx, ecx
00421770  call       0x4615a0

// block 00421775
00421775  pop        ecx
00421776  ret        
