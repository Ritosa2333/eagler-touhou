
// block 00430500
00430500  test       dword ptr [0x4cee78], 0x8000
0043050a  je         0x43051d

// block 0043050c
0043050c  push       0x4cf188
00430511  call       dword ptr [0x498088]

// block 00430517
00430517  inc        byte ptr [0x4cf21e]

// block 0043051d
0043051d  push       esi
0043051e  mov        esi, 0x4cf0d8
00430523  call       0x464c40

// block 00430528
00430528  push       0x4cf0e0
0043052d  push       0
0043052f  push       0
00430531  push       0x422280
00430536  push       0
00430538  push       0
0043053a  mov        dword ptr [0x4cf0f0], 0x422280
00430544  mov        dword ptr [0x4cf0e8], 1
0043054e  mov        dword ptr [0x4cf0e4], 0
00430558  call       0x46e54b

// block 0043055d
0043055d  add        esp, 0x18
00430560  test       dword ptr [0x4cee78], 0x8000
0043056a  mov        dword ptr [0x4cf0dc], eax
0043056f  pop        esi
00430570  je         0x430583

// block 00430572
00430572  push       0x4cf188
00430577  call       dword ptr [0x49808c]

// block 0043057d
0043057d  dec        byte ptr [0x4cf21e]

// block 00430583
00430583  xor        eax, eax
00430585  ret        
