
// block 00428750
00428750  mov        eax, dword ptr [0x4b44f4]
00428755  mov        ecx, dword ptr [eax + 0x18]
00428758  test       ecx, ecx
0042875a  je         0x428779

// block 0042875c
0042875c  push       esi
0042875d  lea        ecx, [ecx]

// block 00428760
00428760  cmp        dword ptr [ecx + 0xc], 1
00428764  mov        esi, dword ptr [ecx + 8]
00428767  je         0x428772

// block 00428769
00428769  mov        edx, dword ptr [ecx]
0042876b  mov        eax, dword ptr [edx + 0x14]
0042876e  push       edi
0042876f  push       ebx
00428770  call       eax

// block 00428772
00428772  mov        ecx, esi
00428774  test       esi, esi
00428776  jne        0x428760

// block 00428778
00428778  pop        esi

// block 00428779
00428779  mov        eax, 1
0042877e  ret        
