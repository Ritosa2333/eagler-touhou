
// block 004742b4
004742b4  push       8
004742b6  push       0x4aaca0
004742bb  call       0x46fcec

// block 004742c0
004742c0  mov        esi, dword ptr [ebp + 8]
004742c3  test       esi, esi
004742c5  je         0x474358

// block 004742cb
004742cb  push       0xd
004742cd  call       0x46ec9a

// block 004742d2
004742d2  pop        ecx
004742d3  and        dword ptr [ebp - 4], 0
004742d7  mov        eax, dword ptr [esi + 4]
004742da  test       eax, eax
004742dc  je         0x4742fa

// block 004742de
004742de  push       eax
004742df  call       dword ptr [0x49815c]

// block 004742e5
004742e5  test       eax, eax
004742e7  jne        0x4742fa

// block 004742e9
004742e9  mov        eax, dword ptr [esi + 4]
004742ec  cmp        eax, 0x4ad4b0
004742f1  je         0x4742fa

// block 004742f3
004742f3  push       eax
004742f4  call       0x46c941

// block 004742f9
004742f9  pop        ecx

// block 004742fa
004742fa  mov        dword ptr [ebp - 4], 0xfffffffe
00474301  call       0x474361

// block 00474306
00474306  cmp        dword ptr [esi], 0
00474309  je         0x474347

// block 0047430b
0047430b  push       0xc
0047430d  call       0x46ec9a

// block 00474312
00474312  pop        ecx
00474313  mov        dword ptr [ebp - 4], 1
0047431a  push       dword ptr [esi]
0047431c  call       0x474084

// block 00474321
00474321  pop        ecx
00474322  mov        eax, dword ptr [esi]
00474324  test       eax, eax
00474326  je         0x47433b

// block 00474328
00474328  cmp        dword ptr [eax], 0
0047432b  jne        0x47433b

// block 0047432d
0047432d  cmp        eax, 0x4ad9e0
00474332  je         0x47433b

// block 00474334
00474334  push       eax
00474335  call       0x473eac

// block 0047433a
0047433a  pop        ecx

// block 0047433b
0047433b  mov        dword ptr [ebp - 4], 0xfffffffe
00474342  call       0x47436d

// block 00474347
00474347  mov        eax, 0xbaadf00d
0047434c  mov        dword ptr [esi], eax
0047434e  mov        dword ptr [esi + 4], eax
00474351  push       esi
00474352  call       0x46c941

// block 00474357
00474357  pop        ecx

// block 00474358
00474358  call       0x46fd31

// block 0047435d
0047435d  ret        

// block 00474376
00474376  mov        edi, edi
00474378  push       ebp
00474379  mov        ebp, esp
0047437b  pop        ebp
0047437c  jmp        0x4742b4
