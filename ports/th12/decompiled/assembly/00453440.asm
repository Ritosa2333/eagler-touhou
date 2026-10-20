
// block 00453440
00453440  mov        eax, dword ptr [esi]
00453442  test       eax, eax
00453444  je         0x453454

// block 00453446
00453446  mov        ecx, dword ptr [eax]
00453448  mov        edx, dword ptr [ecx + 8]
0045344b  push       eax
0045344c  call       edx

// block 0045344e
0045344e  mov        dword ptr [esi], 0

// block 00453454
00453454  push       esi
00453455  call       0x46ca4f

// block 0045345a
0045345a  add        esp, 4
0045345d  mov        eax, esi
0045345f  ret        
