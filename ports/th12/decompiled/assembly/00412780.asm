
// block 00412780
00412780  mov        eax, dword ptr [0x4b43cc]
00412785  mov        eax, dword ptr [eax + 0x7c]
00412788  test       al, 1
0041278a  je         0x412796

// block 0041278c
0041278c  test       al, 0x20
0041278e  je         0x412796

// block 00412790
00412790  mov        eax, 1
00412795  ret        

// block 00412796
00412796  xor        eax, eax
00412798  ret        
