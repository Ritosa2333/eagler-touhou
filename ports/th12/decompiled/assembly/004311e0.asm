
// block 004311e0
004311e0  push       ebx
004311e1  push       esi
004311e2  push       edi
004311e3  mov        edi, eax
004311e5  mov        eax, dword ptr [edi + 0x554]
004311eb  mov        ebx, 1
004311f0  cmp        eax, dword ptr [edi + 0x558]
004311f6  je         0x4312dc

// block 004311fc
004311fc  test       dword ptr [edi + 0x590], 0x8000
00431206  je         0x43121b

// block 00431208
00431208  lea        ecx, [edi + 0x888]
0043120e  push       ecx
0043120f  call       dword ptr [0x498088]

// block 00431215
00431215  add        byte ptr [edi + 0x935], bl

// block 0043121b
0043121b  mov        edx, dword ptr [edi + 0x558]
00431221  mov        eax, dword ptr [edi + 0x554]
00431227  push       edx
00431228  push       eax
00431229  push       0x4a0d58
0043122e  mov        dword ptr [edi + 0x55c], eax
00431234  call       0x4319e0

// block 00431239
00431239  mov        eax, dword ptr [edi + 0x558]
0043123f  add        esp, 0xc
00431242  mov        dword ptr [edi + 0x9c0], 0xff000000
0043124c  cmp        eax, 0x11
0043124f  ja         0x4312b1

// block 00431251
00431251  jmp        dword ptr [eax*4 + 0x43145c]

// block 00431258
00431258  mov        dword ptr [edi + 0x558], ebx
0043125e  call       0x42ede0

// block 00431263
00431263  mov        dword ptr [edi + 0x9a8], eax
00431269  test       eax, eax
0043126b  jne        0x4312b1

// block 0043126d
0043126d  mov        dword ptr [edi + 0x558], 3

// block 00431277
00431277  call       0x42f830

// block 0043127c
0043127c  mov        esi, 5
00431281  call       0x431800

// block 00431286
00431286  pop        edi
00431287  lea        eax, [esi - 1]
0043128a  pop        esi
0043128b  pop        ebx
0043128c  ret        

// block 0043128d
0043128d  mov        eax, dword ptr [edi + 0x554]
00431293  dec        eax
00431294  cmp        eax, 0xe
00431297  ja         0x4312b1

// block 00431299
00431299  movzx      eax, byte ptr [eax + 0x4314b4]
004312a0  jmp        dword ptr [eax*4 + 0x4314a4]

// block 004312a7
004312a7  call       0x422770

// block 004312ac
004312ac  call       0x43f6b0

// block 004312b1
004312b1  test       dword ptr [edi + 0x590], 0x8000
004312bb  mov        edx, dword ptr [edi + 0x558]
004312c1  mov        dword ptr [edi + 0x554], edx
004312c7  je         0x4312dc

// block 004312c9
004312c9  lea        eax, [edi + 0x888]
004312cf  push       eax
004312d0  call       dword ptr [0x49808c]

// block 004312d6
004312d6  dec        byte ptr [edi + 0x935]

// block 004312dc
004312dc  pop        edi
004312dd  pop        esi
004312de  mov        eax, ebx
004312e0  pop        ebx
004312e1  ret        

// block 004312e2
004312e2  call       0x4110e0

// block 004312e7
004312e7  jmp        0x4312ac

// block 004312e9
004312e9  mov        eax, dword ptr [edi + 0x554]
004312ef  sub        eax, 2
004312f2  je         0x43130a

// block 004312f4
004312f4  sub        eax, 5
004312f7  je         0x431305

// block 004312f9
004312f9  sub        eax, 8
004312fc  jne        0x4312b1

// block 004312fe
004312fe  call       0x4110e0

// block 00431303
00431303  jmp        0x43130a

// block 00431305
00431305  call       0x422770

// block 0043130a
0043130a  mov        dword ptr [edi + 0x558], 4
00431314  mov        dword ptr [0x4ce8b0], 3
0043131e  jmp        0x4312ac

// block 00431320
00431320  cmp        dword ptr [edi + 0x554], 4
00431327  jne        0x43132e

// block 00431329
00431329  call       0x43f720

// block 0043132e
0043132e  push       0
00431330  mov        dword ptr [edi + 0x564], ebx
00431336  call       0x422700

// block 0043133b
0043133b  jmp        0x4312b1

// block 00431340
00431340  cmp        dword ptr [edi + 0x554], 4
00431347  jne        0x43134e

// block 00431349
00431349  call       0x43f720

// block 0043134e
0043134e  push       ebx
0043134f  mov        dword ptr [edi + 0x558], 7
00431359  mov        dword ptr [edi + 0x564], ebx
0043135f  call       0x422700

// block 00431364
00431364  jmp        0x4312b1

// block 00431369
00431369  cmp        dword ptr [edi + 0x554], 7
00431370  mov        ecx, dword ptr [0x4b44e8]
00431376  mov        esi, dword ptr [ecx + 0x74]
00431379  mov        dword ptr [edi + 0x564], 0
00431383  jne        0x43138a

// block 00431385
00431385  call       0x422770

// block 0043138a
0043138a  push       esi
0043138b  mov        dword ptr [edi + 0x558], 7
00431395  call       0x422700

// block 0043139a
0043139a  jmp        0x4312b1

// block 0043139f
0043139f  call       0x422770

// block 004313a4
004313a4  mov        dword ptr [edi + 0x564], ebx
004313aa  mov        dword ptr [edi + 0x558], 7
004313b4  mov        eax, dword ptr [0x4b0cb4]
004313b9  mov        dword ptr [0x4b0cb0], eax
004313be  shl        eax, 6
004313c1  add        eax, 0x4aebf0
004313c6  push       0
004313c8  mov        dword ptr [0x4b452c], eax
004313cd  call       0x422700

// block 004313d2
004313d2  jmp        0x4312b1

// block 004313d7
004313d7  call       0x422770

// block 004313dc
004313dc  mov        dword ptr [edi + 0x564], ebx
004313e2  mov        dword ptr [edi + 0x558], 7
004313ec  mov        eax, dword ptr [0x4b0cb4]
004313f1  mov        dword ptr [0x4b0cb0], eax
004313f6  shl        eax, 6
004313f9  add        eax, 0x4aebf0
004313fe  push       ebx
004313ff  mov        dword ptr [0x4b452c], eax
00431404  call       0x422700

// block 00431409
00431409  jmp        0x4312b1

// block 0043140e
0043140e  call       0x422770

// block 00431413
00431413  push       0
00431415  mov        dword ptr [edi + 0x564], ebx
0043141b  mov        dword ptr [edi + 0x558], 7
00431425  call       0x422700

// block 0043142a
0043142a  jmp        0x4312b1

// block 0043142f
0043142f  cmp        dword ptr [edi + 0x554], 7
00431436  jne        0x43143d

// block 00431438
00431438  call       0x422770

// block 0043143d
0043143d  call       0x411060

// block 00431442
00431442  jmp        0x4312b1

// block 00431447
00431447  call       0x42f830

// block 0043144c
0043144c  mov        esi, 5
00431451  call       0x431800

// block 00431456
00431456  pop        edi
00431457  mov        eax, esi
00431459  pop        esi
0043145a  pop        ebx
0043145b  ret        
