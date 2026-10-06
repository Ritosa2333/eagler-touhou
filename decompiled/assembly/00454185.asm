
// block 00454185
00454185  mov        eax, dword ptr [0x4d4754]
0045418a  cmp        eax, ebx
0045418c  je         0x454282

// block 00454192
00454192  mov        ecx, dword ptr [ebp + 8]
00454195  cmp        ecx, ebx
00454197  je         0x45439a

// block 0045419d
0045419d  cmp        ecx, 1
004541a0  jne        0x4541c4

// block 004541a2
004541a2  cmp        dword ptr [0x4cf500], ebx
004541a8  je         0x454282

// block 004541ae
004541ae  mov        edx, dword ptr [0x4cf4fc]
004541b4  push       ebx
004541b5  push       ebx
004541b6  push       0x12
004541b8  push       edx
004541b9  call       dword ptr [0x498220]

// block 004541bf
004541bf  jmp        0x4543a1

// block 004541c4
004541c4  cmp        ecx, 2
004541c7  je         0x454330

// block 004541cd
004541cd  cmp        ecx, 3
004541d0  je         0x454363

// block 004541d6
004541d6  cmp        ecx, 0xa
004541d9  jne        0x4543a1

// block 004541df
004541df  jmp        0x454282

// block 00454330
00454330  mov        eax, dword ptr [0x4cf500]
00454335  push       0x100
0045433a  push       eax
0045433b  call       dword ptr [0x4980f4]

// block 00454341
00454341  test       eax, eax
00454343  je         0x45435b

// block 00454345
00454345  mov        ecx, dword ptr [0x4cf4fc]
0045434b  push       ebx
0045434c  push       ebx
0045434d  push       0x12
0045434f  push       ecx
00454350  call       dword ptr [0x498220]

// block 00454356
00454356  dec        dword ptr [ebp + 8]
00454359  jmp        0x4543a1

// block 0045435b
0045435b  mov        dword ptr [0x4cf500], ebx
00454361  jmp        0x4543a1

// block 00454363
00454363  mov        edx, dword ptr [0x4cf500]
00454369  mov        esi, dword ptr [0x4980a4]
0045436f  push       edx
00454370  call       esi

// block 00454372
00454372  mov        eax, dword ptr [0x4d4758]
00454377  push       eax
00454378  call       esi

// block 0045437a
0045437a  mov        ecx, dword ptr [0x4d4754]
00454380  mov        dword ptr [0x4cf500], ebx
00454386  cmp        ecx, ebx
00454388  je         0x454392

// block 0045438a
0045438a  mov        edx, dword ptr [ecx]
0045438c  mov        eax, dword ptr [edx]
0045438e  push       1
00454390  call       eax

// block 00454392
00454392  mov        dword ptr [0x4d4754], ebx
00454398  jmp        0x4543a1

// block 0045439a
0045439a  push       1
