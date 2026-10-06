
// block 00475481
00475481  push       8
00475483  push       0x4aad50
00475488  call       0x46fcec

// block 0047548d
0047548d  mov        esi, dword ptr [ebp + 8]
00475490  test       esi, esi
00475492  je         0x475590

// block 00475498
00475498  mov        eax, dword ptr [esi + 0x24]
0047549b  test       eax, eax
0047549d  je         0x4754a6

// block 0047549f
0047549f  push       eax
004754a0  call       0x46c941

// block 004754a5
004754a5  pop        ecx

// block 004754a6
004754a6  mov        eax, dword ptr [esi + 0x2c]
004754a9  test       eax, eax
004754ab  je         0x4754b4

// block 004754ad
004754ad  push       eax
004754ae  call       0x46c941

// block 004754b3
004754b3  pop        ecx

// block 004754b4
004754b4  mov        eax, dword ptr [esi + 0x34]
004754b7  test       eax, eax
004754b9  je         0x4754c2

// block 004754bb
004754bb  push       eax
004754bc  call       0x46c941

// block 004754c1
004754c1  pop        ecx

// block 004754c2
004754c2  mov        eax, dword ptr [esi + 0x3c]
004754c5  test       eax, eax
004754c7  je         0x4754d0

// block 004754c9
004754c9  push       eax
004754ca  call       0x46c941

// block 004754cf
004754cf  pop        ecx

// block 004754d0
004754d0  mov        eax, dword ptr [esi + 0x40]
004754d3  test       eax, eax
004754d5  je         0x4754de

// block 004754d7
004754d7  push       eax
004754d8  call       0x46c941

// block 004754dd
004754dd  pop        ecx

// block 004754de
004754de  mov        eax, dword ptr [esi + 0x44]
004754e1  test       eax, eax
004754e3  je         0x4754ec

// block 004754e5
004754e5  push       eax
004754e6  call       0x46c941

// block 004754eb
004754eb  pop        ecx

// block 004754ec
004754ec  mov        eax, dword ptr [esi + 0x48]
004754ef  test       eax, eax
004754f1  je         0x4754fa

// block 004754f3
004754f3  push       eax
004754f4  call       0x46c941

// block 004754f9
004754f9  pop        ecx

// block 004754fa
004754fa  mov        eax, dword ptr [esi + 0x5c]
004754fd  cmp        eax, 0x49d5c8
00475502  je         0x47550b

// block 00475504
00475504  push       eax
00475505  call       0x46c941

// block 0047550a
0047550a  pop        ecx

// block 0047550b
0047550b  push       0xd
0047550d  call       0x46ec9a

// block 00475512
00475512  pop        ecx
00475513  and        dword ptr [ebp - 4], 0
00475517  mov        edi, dword ptr [esi + 0x68]
0047551a  test       edi, edi
0047551c  je         0x475538

// block 0047551e
0047551e  push       edi
0047551f  call       dword ptr [0x49815c]

// block 00475525
00475525  test       eax, eax
00475527  jne        0x475538

// block 00475529
00475529  cmp        edi, 0x4ad4b0
0047552f  je         0x475538

// block 00475531
00475531  push       edi
00475532  call       0x46c941

// block 00475537
00475537  pop        ecx

// block 00475538
00475538  mov        dword ptr [ebp - 4], 0xfffffffe
0047553f  call       0x47559b

// block 00475544
00475544  push       0xc
00475546  call       0x46ec9a

// block 0047554b
0047554b  pop        ecx
0047554c  mov        dword ptr [ebp - 4], 1
00475553  mov        edi, dword ptr [esi + 0x6c]
00475556  test       edi, edi
00475558  je         0x47557d

// block 0047555a
0047555a  push       edi
0047555b  call       0x474084

// block 00475560
00475560  pop        ecx
00475561  cmp        edi, dword ptr [0x4adab8]
00475567  je         0x47557d

// block 00475569
00475569  cmp        edi, 0x4ad9e0
0047556f  je         0x47557d

// block 00475571
00475571  cmp        dword ptr [edi], 0
00475574  jne        0x47557d

// block 00475576
00475576  push       edi
00475577  call       0x473eac

// block 0047557c
0047557c  pop        ecx

// block 0047557d
0047557d  mov        dword ptr [ebp - 4], 0xfffffffe
00475584  call       0x4755a7

// block 00475589
00475589  push       esi
0047558a  call       0x46c941

// block 0047558f
0047558f  pop        ecx

// block 00475590
00475590  call       0x46fd31

// block 00475595
00475595  ret        4
