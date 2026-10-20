
// block 004306c0
004306c0  push       ebx
004306c1  push       esi
004306c2  mov        ebx, 1
004306c7  xor        esi, esi
004306c9  cmp        dword ptr [edi + 0x93c], ebx
004306cf  jne        0x43070c

// block 004306d1
004306d1  mov        eax, dword ptr [0x4b44fc]
004306d6  push       eax
004306d7  call       0x461970

// block 004306dc
004306dc  mov        ecx, dword ptr [0x4b4500]
004306e2  push       ecx
004306e3  call       0x461970

// block 004306e8
004306e8  mov        edx, dword ptr [0x4b4504]
004306ee  push       edx
004306ef  call       0x461970

// block 004306f4
004306f4  mov        dword ptr [0x4b44fc], esi
004306fa  mov        dword ptr [0x4b4500], esi
00430700  mov        dword ptr [0x4b4504], esi
00430706  mov        dword ptr [edi + 0x93c], esi

// block 0043070c
0043070c  cmp        dword ptr [0x4ce8a4], esi
00430712  je         0x43071a

// block 00430714
00430714  mov        dword ptr [0x4ce8a4], esi

// block 0043071a
0043071a  pop        esi
0043071b  pop        ebx
0043071c  ret        
