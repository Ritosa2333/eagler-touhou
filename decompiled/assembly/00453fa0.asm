
// block 00453fa0
00453fa0  sub        esp, 8
00453fa3  push       ebx
00453fa4  push       esi
00453fa5  mov        esi, 0x8000
00453faa  test       dword ptr [0x4cee78], esi
00453fb0  je         0x453fc3

// block 00453fb2
00453fb2  push       0x4cf200
00453fb7  call       dword ptr [0x498088]

// block 00453fbd
00453fbd  inc        byte ptr [0x4cf223]

// block 00453fc3
00453fc3  xor        ebx, ebx
00453fc5  cmp        dword ptr [0x4cf4f8], ebx
00453fcb  jne        0x453fee

// block 00453fcd
00453fcd  test       dword ptr [0x4cee78], esi
00453fd3  je         0x453fe6

// block 00453fd5
00453fd5  push       0x4cf200
00453fda  call       dword ptr [0x49808c]

// block 00453fe0
00453fe0  dec        byte ptr [0x4cf223]

// block 00453fe6
00453fe6  pop        esi
00453fe7  xor        eax, eax
00453fe9  pop        ebx
00453fea  add        esp, 8
00453fed  ret        

// block 00453fee
00453fee  push       ebp
00453fef  mov        ebp, 0x4d14d4
00453ff4  push       edi

// block 00453ff5
00453ff5  mov        eax, dword ptr [ebp]
00453ff8  dec        eax
00453ff9  mov        dword ptr [esp + 0x10], ebx
00453ffd  cmp        eax, 7
00454000  ja         0x4543a4

// block 00454006
00454006  jmp        dword ptr [eax*4 + 0x45448c]

// block 004543a4
004543a4  cmp        byte ptr [0x4ceacc], 0
004543ab  je         0x454462

// block 004543b1
004543b1  mov        ebp, 0x4cf568
004543b6  mov        edi, 0x4cf538
004543bb  jmp        0x4543c0

// block 004543c0
004543c0  mov        esi, dword ptr [edi - 0x30]
004543c3  test       esi, esi
004543c5  jl         0x454462

// block 004543cb
004543cb  mov        ebx, dword ptr [edi]
004543cd  mov        dword ptr [edi - 0x30], 0xffffffff
004543d4  test       ebx, ebx
004543d6  jge        0x45441d

// block 004543d8
004543d8  lea        esi, [esi + esi*2]
004543db  mov        eax, dword ptr [esi*8 + 0x4d0e70]
004543e2  lea        esi, [esi*8 + 0x4d0e70]
004543e9  mov        dword ptr [esi + 0x14], 0
004543f0  test       eax, eax
004543f2  je         0x454415

// block 004543f4
004543f4  mov        ecx, dword ptr [eax]
004543f6  lea        edx, [esp + 0x14]
004543fa  push       edx
004543fb  push       eax
004543fc  mov        eax, dword ptr [ecx + 0x24]
004543ff  call       eax

// block 00454401
00454401  mov        ecx, dword ptr [esp + 0x14]
00454405  and        ecx, 1
00454408  mov        dword ptr [esi + 0x14], ecx
0045440b  mov        esi, dword ptr [esi]
0045440d  mov        edx, dword ptr [esi]
0045440f  mov        eax, dword ptr [edx + 0x48]
00454412  push       esi
00454413  call       eax

// block 00454415
00454415  mov        dword ptr [edi], 0
0045441b  jmp        0x45444d

// block 0045441d
0045441d  xor        eax, eax
0045441f  test       ebx, ebx
00454421  jle        0x454438

// block 00454423
00454423  mov        ecx, ebp
00454425  mov        edx, ebx

// block 00454427
00454427  add        eax, dword ptr [ecx]
00454429  add        ecx, 4
0045442c  sub        edx, 1
0045442f  jne        0x454427

// block 00454431
00454431  test       ebx, ebx
00454433  jle        0x454438

// block 00454435
00454435  cdq        
00454436  idiv       ebx

// block 00454438
00454438  lea        esi, [esi + esi*2]
0045443b  lea        esi, [esi*8 + 0x4d0e70]
00454442  mov        dword ptr [edi], 0
00454448  call       0x4544b0

// block 0045444d
0045444d  add        edi, 4
00454450  add        ebp, 0x200
00454456  cmp        edi, 0x4cf568
0045445c  jl         0x4543c0

// block 00454462
00454462  test       dword ptr [0x4cee78], 0x8000
0045446c  je         0x45447f

// block 0045446e
0045446e  push       0x4cf200
00454473  call       dword ptr [0x49808c]

// block 00454479
00454479  dec        byte ptr [0x4cf223]

// block 0045447f
0045447f  mov        eax, dword ptr [0x4d14d4]
00454484  pop        edi
00454485  pop        ebp
00454486  pop        esi
00454487  pop        ebx
00454488  add        esp, 8
0045448b  ret        
