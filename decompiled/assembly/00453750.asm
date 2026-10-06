
// block 00453750
00453750  push       0
00453752  push       0
00453754  call       0x463c10

// block 00453759
00453759  mov        ecx, dword ptr [esp + 4]
0045375d  mov        edx, dword ptr [esp + 8]
00453761  mov        dword ptr [ecx + edx*4 + 0x1f28], eax
00453768  neg        eax
0045376a  sbb        eax, eax
0045376c  neg        eax
0045376e  dec        eax
0045376f  ret        8
