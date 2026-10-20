
// block 0043f130
0043f130  push       esi
0043f131  mov        esi, dword ptr [0x4ce8cc]
0043f137  add        esi, 0x4b50c4
0043f13d  push       edi
0043f13e  mov        edi, dword ptr [esi]
0043f140  test       edi, edi
0043f142  je         0x43f15a

// block 0043f144
0043f144  call       0x4604e0

// block 0043f149
0043f149  mov        eax, dword ptr [esi]
0043f14b  push       eax
0043f14c  call       0x46ca4f

// block 0043f151
0043f151  add        esp, 4
0043f154  mov        dword ptr [esi], 0

// block 0043f15a
0043f15a  pop        edi
0043f15b  pop        esi
0043f15c  ret        
