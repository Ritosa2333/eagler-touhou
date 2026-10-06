
// block 00406ed0
00406ed0  push       ecx
00406ed1  mov        ecx, dword ptr [0x4b43c4]
00406ed7  cmp        dword ptr [ecx + 0x3c], 0
00406edb  je         0x406efb

// block 00406edd
00406edd  mov        eax, dword ptr [0x4b0c94]
00406ee2  mov        edx, dword ptr [0x4b0c90]
00406ee8  lea        eax, [eax + edx*2]
00406eeb  sub        eax, 0
00406eee  je         0x406efb

// block 00406ef0
00406ef0  sub        eax, 1
00406ef3  jne        0x406efb

// block 00406ef5
00406ef5  push       ecx
00406ef6  call       0x408580

// block 00406efb
00406efb  pop        ecx
00406efc  ret        
