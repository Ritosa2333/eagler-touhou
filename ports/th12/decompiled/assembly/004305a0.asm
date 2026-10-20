
// block 004305a0
004305a0  push       ecx
004305a1  push       esi
004305a2  mov        esi, eax
004305a4  cmp        dword ptr [esi + 0x93c], 0
004305ab  jne        0x4306b6

// block 004305b1
004305b1  mov        ecx, dword ptr [esi + 0x58c]
004305b7  push       0
004305b9  push       0
004305bb  lea        eax, [esp + 0xc]
004305bf  push       eax
004305c0  push       ecx
004305c1  mov        eax, 0x17
004305c6  xor        ecx, ecx
004305c8  call       0x4615a0

// block 004305cd
004305cd  mov        edx, dword ptr [esp + 4]
004305d1  push       0
004305d3  push       1
004305d5  lea        eax, [esp + 0xc]
004305d9  mov        dword ptr [0x4b44fc], edx
004305df  mov        ecx, dword ptr [esi + 0x58c]
004305e5  push       eax
004305e6  push       ecx
004305e7  mov        eax, 0x17
004305ec  xor        ecx, ecx
004305ee  call       0x4615a0

// block 004305f3
004305f3  mov        edx, dword ptr [esp + 4]
004305f7  push       0
004305f9  push       2
004305fb  lea        eax, [esp + 0xc]
004305ff  mov        dword ptr [0x4b4500], edx
00430605  mov        ecx, dword ptr [esi + 0x58c]
0043060b  push       eax
0043060c  push       ecx
0043060d  mov        eax, 0x17
00430612  xor        ecx, ecx
00430614  call       0x4615a0

// block 00430619
00430619  mov        edx, dword ptr [esp + 4]
0043061d  mov        dword ptr [0x4b4504], edx
00430623  mov        dword ptr [esi + 0x93c], 1
0043062d  mov        eax, dword ptr [0x4b44fc]
00430632  mov        esi, dword ptr [0x4ce8cc]
00430638  push       eax
00430639  mov        edx, esi
0043063b  call       0x461920

// block 00430640
00430640  test       eax, eax
00430642  je         0x43065e

// block 00430644
00430644  mov        ecx, dword ptr [edi]
00430646  mov        dword ptr [eax + 0x430], ecx
0043064c  mov        edx, dword ptr [edi + 4]
0043064f  mov        dword ptr [eax + 0x434], edx
00430655  mov        ecx, dword ptr [edi + 8]
00430658  mov        dword ptr [eax + 0x438], ecx

// block 0043065e
0043065e  mov        edx, dword ptr [0x4b4500]
00430664  push       edx
00430665  mov        edx, esi
00430667  call       0x461920

// block 0043066c
0043066c  test       eax, eax
0043066e  je         0x43068a

// block 00430670
00430670  mov        ecx, dword ptr [edi]
00430672  mov        dword ptr [eax + 0x430], ecx
00430678  mov        edx, dword ptr [edi + 4]
0043067b  mov        dword ptr [eax + 0x434], edx
00430681  mov        ecx, dword ptr [edi + 8]
00430684  mov        dword ptr [eax + 0x438], ecx

// block 0043068a
0043068a  mov        edx, dword ptr [0x4b4504]
00430690  push       edx
00430691  mov        edx, esi
00430693  call       0x461920

// block 00430698
00430698  test       eax, eax
0043069a  je         0x4306b6

// block 0043069c
0043069c  mov        ecx, dword ptr [edi]
0043069e  mov        dword ptr [eax + 0x430], ecx
004306a4  mov        edx, dword ptr [edi + 4]
004306a7  mov        dword ptr [eax + 0x434], edx
004306ad  mov        ecx, dword ptr [edi + 8]
004306b0  mov        dword ptr [eax + 0x438], ecx

// block 004306b6
004306b6  pop        esi
004306b7  pop        ecx
004306b8  ret        
