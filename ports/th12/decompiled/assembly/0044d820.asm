
// block 0044d820
0044d820  mov        ecx, dword ptr [0x4aeb20]
0044d826  xor        eax, eax
0044d828  cmp        ecx, -1
0044d82b  je         0x44d844

// block 0044d82d
0044d82d  lea        ecx, [ecx]

// block 0044d830
0044d830  cmp        ecx, edx
0044d832  je         0x44d844

// block 0044d834
0044d834  inc        eax
0044d835  lea        ecx, [eax + eax*2]
0044d838  mov        ecx, dword ptr [ecx*8 + 0x4aeb20]
0044d83f  cmp        ecx, -1
0044d842  jne        0x44d830

// block 0044d844
0044d844  cmp        edx, -1
0044d847  jne        0x44d84c

// block 0044d849
0044d849  xor        eax, eax
0044d84b  ret        

// block 0044d84c
0044d84c  lea        eax, [eax + eax*2]
0044d84f  lea        eax, [eax*8 + 0x4aeb20]
0044d856  ret        
