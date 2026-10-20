
// block 00401220
00401220  push       -1
00401222  push       0x496f49
00401227  mov        eax, dword ptr fs:[0]
0040122d  push       eax
0040122e  push       ebx
0040122f  push       ebp
00401230  push       esi
00401231  push       edi
00401232  mov        eax, dword ptr [0x4ad138]
00401237  xor        eax, esp
00401239  push       eax
0040123a  lea        eax, [esp + 0x14]
0040123e  mov        dword ptr fs:[0], eax
00401244  mov        ebx, dword ptr [esp + 0x24]
00401248  mov        dword ptr [ebx], 0x49f4b8
0040124e  mov        dword ptr [esp + 0x1c], 1
00401256  mov        esi, dword ptr [ebx + 0xc]
00401259  mov        edi, dword ptr [0x4ce89c]
0040125f  mov        ebp, dword ptr [0x49808c]
00401265  test       esi, esi
00401267  je         0x4012a8

// block 00401269
00401269  test       dword ptr [0x4cee78], 0x8000
00401273  je         0x401286

// block 00401275
00401275  push       0x4cf0f8
0040127a  call       dword ptr [0x498088]

// block 00401280
00401280  inc        byte ptr [0x4cf218]

// block 00401286
00401286  mov        ecx, esi
00401288  mov        edx, edi
0040128a  call       0x462890

// block 0040128f
0040128f  test       dword ptr [0x4cee78], 0x8000
00401299  je         0x4012a8

// block 0040129b
0040129b  push       0x4cf0f8
004012a0  call       ebp

// block 004012a2
004012a2  dec        byte ptr [0x4cf218]

// block 004012a8
004012a8  mov        esi, dword ptr [ebx + 0x10]
004012ab  mov        edi, dword ptr [0x4ce89c]
004012b1  test       esi, esi
004012b3  je         0x4012f4

// block 004012b5
004012b5  test       dword ptr [0x4cee78], 0x8000
004012bf  je         0x4012d2

// block 004012c1
004012c1  push       0x4cf0f8
004012c6  call       dword ptr [0x498088]

// block 004012cc
004012cc  inc        byte ptr [0x4cf218]

// block 004012d2
004012d2  mov        ecx, esi
004012d4  mov        edx, edi
004012d6  call       0x462890

// block 004012db
004012db  test       dword ptr [0x4cee78], 0x8000
004012e5  je         0x4012f4

// block 004012e7
004012e7  push       0x4cf0f8
004012ec  call       ebp

// block 004012ee
004012ee  dec        byte ptr [0x4cf218]

// block 004012f4
004012f4  mov        esi, dword ptr [ebx + 0x18fc0]
004012fa  mov        edi, dword ptr [0x4ce89c]
00401300  test       esi, esi
00401302  je         0x401343

// block 00401304
00401304  test       dword ptr [0x4cee78], 0x8000
0040130e  je         0x401321

// block 00401310
00401310  push       0x4cf0f8
00401315  call       dword ptr [0x498088]

// block 0040131b
0040131b  inc        byte ptr [0x4cf218]

// block 00401321
00401321  mov        ecx, esi
00401323  mov        edx, edi
00401325  call       0x462890

// block 0040132a
0040132a  test       dword ptr [0x4cee78], 0x8000
00401334  je         0x401343

// block 00401336
00401336  push       0x4cf0f8
0040133b  call       ebp

// block 0040133d
0040133d  dec        byte ptr [0x4cf218]

// block 00401343
00401343  mov        esi, dword ptr [0x4ce8cc]
00401349  mov        edi, dword ptr [esi + 0x4b50c8]
0040134f  add        esi, 0x4b50c8
00401355  test       edi, edi
00401357  je         0x40136f

// block 00401359
00401359  call       0x4604e0

// block 0040135e
0040135e  mov        eax, dword ptr [esi]
00401360  push       eax
00401361  call       0x46ca4f

// block 00401366
00401366  add        esp, 4
00401369  mov        dword ptr [esi], 0

// block 0040136f
0040136f  mov        esi, dword ptr [0x4ce8cc]
00401375  mov        edi, dword ptr [esi + 0x4b50c0]
0040137b  add        esi, 0x4b50c0
00401381  test       edi, edi
00401383  je         0x40139b

// block 00401385
00401385  call       0x4604e0

// block 0040138a
0040138a  mov        ecx, dword ptr [esi]
0040138c  push       ecx
0040138d  call       0x46ca4f

// block 00401392
00401392  add        esp, 4
00401395  mov        dword ptr [esi], 0

// block 0040139b
0040139b  mov        eax, dword ptr [ebx + 0x940]
004013a1  mov        dword ptr [0x4b43b8], 0
004013ab  test       eax, eax
004013ad  je         0x4013b8

// block 004013af
004013af  push       eax
004013b0  call       0x46c941

// block 004013b5
004013b5  add        esp, 4

// block 004013b8
004013b8  xor        ecx, ecx
004013ba  mov        dword ptr [ebx + 0x940], ecx
004013c0  mov        eax, dword ptr [ebx + 0x48c]
004013c6  cmp        eax, ecx
004013c8  je         0x4013df

// block 004013ca
004013ca  push       eax
004013cb  call       0x46c941

// block 004013d0
004013d0  add        esp, 4
004013d3  mov        dword ptr [ebx + 0x48c], 0
004013dd  jmp        0x4013e5

// block 004013df
004013df  mov        dword ptr [ebx + 0x48c], ecx

// block 004013e5
004013e5  mov        ecx, dword ptr [esp + 0x14]
004013e9  mov        dword ptr fs:[0], ecx
004013f0  pop        ecx
004013f1  pop        edi
004013f2  pop        esi
004013f3  pop        ebp
004013f4  pop        ebx
004013f5  add        esp, 0xc
004013f8  ret        4
