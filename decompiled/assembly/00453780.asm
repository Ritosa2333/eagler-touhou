
// block 00453780
00453780  push       0
00453782  push       0
00453784  mov        eax, 0x4a07ac
00453789  call       0x463c10

// block 0045378e
0045378e  mov        ecx, dword ptr [esp + 4]
00453792  mov        dword ptr [ecx + 0x1984], eax
00453798  neg        eax
0045379a  sbb        eax, eax
0045379c  neg        eax
0045379e  dec        eax
0045379f  ret        4
