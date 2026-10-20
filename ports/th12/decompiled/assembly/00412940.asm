
// block 00412940
00412940  mov        ecx, dword ptr [0x4b44f4]
00412946  mov        eax, dword ptr [ecx + 0x48c]
0041294c  test       eax, eax
0041294e  je         0x41295a

// block 00412950
00412950  mov        eax, dword ptr [eax + 8]
00412953  mov        dword ptr [ecx + 0x48c], eax
00412959  ret        

// block 0041295a
0041295a  xor        eax, eax
0041295c  ret        
