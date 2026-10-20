
// block 0047013f
0047013f  test       byte ptr [ecx + 0xc], 0x40
00470143  je         0x47014b

// block 00470145
00470145  cmp        dword ptr [ecx + 8], 0
00470149  je         0x47016f

// block 0047014b
0047014b  dec        dword ptr [ecx + 4]
0047014e  js         0x47015b

// block 00470150
00470150  mov        edx, dword ptr [ecx]
00470152  mov        byte ptr [edx], al
00470154  inc        dword ptr [ecx]
00470156  movzx      eax, al
00470159  jmp        0x470167

// block 0047015b
0047015b  movsx      eax, al
0047015e  push       ecx
0047015f  push       eax
00470160  call       0x46ffdb

// block 00470165
00470165  pop        ecx
00470166  pop        ecx

// block 00470167
00470167  cmp        eax, -1
0047016a  jne        0x47016f

// block 0047016c
0047016c  or         dword ptr [esi], eax
0047016e  ret        

// block 0047016f
0047016f  inc        dword ptr [esi]
00470171  ret        
