
// block 0044cb00
0044cb00  push       esi
0044cb01  mov        esi, ecx
0044cb03  lea        eax, [esi + esi*2]
0044cb06  add        eax, eax
0044cb08  add        eax, eax
0044cb0a  cmp        dword ptr [eax + 0x4b4540], 0
0044cb11  je         0x44cb28

// block 0044cb13
0044cb13  mov        edx, dword ptr [eax + 0x4b4548]
0044cb19  test       edx, edx
0044cb1b  jne        0x44cb2a

// block 0044cb1d
0044cb1d  mov        edx, dword ptr [eax + 0x4b4544]

// block 0044cb23
0044cb23  call       0x44cb50

// block 0044cb28
0044cb28  pop        esi
0044cb29  ret        

// block 0044cb2a
0044cb2a  cmp        dword ptr [eax + 0x4b4544], 0
0044cb31  je         0x44cb23

// block 0044cb33
0044cb33  push       edi
0044cb34  mov        eax, esi
0044cb36  call       0x44cc30

// block 0044cb3b
0044cb3b  mov        edi, eax
0044cb3d  mov        ecx, edi
0044cb3f  call       0x44cb00

// block 0044cb44
0044cb44  mov        edx, edi
0044cb46  pop        edi
0044cb47  mov        eax, esi
0044cb49  pop        esi
0044cb4a  jmp        0x44cba0
