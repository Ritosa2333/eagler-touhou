
// block 00463fb0
00463fb0  push       ecx
00463fb1  test       dword ptr [0x4cee78], 0x8000
00463fbb  je         0x463fce

// block 00463fbd
00463fbd  push       0x4cf128
00463fc2  call       dword ptr [0x498088]

// block 00463fc8
00463fc8  inc        byte ptr [0x4cf21a]

// block 00463fce
00463fce  push       0
00463fd0  push       0x80
00463fd5  push       2
00463fd7  push       0
00463fd9  push       1
00463fdb  push       0x40000000
00463fe0  push       esi
00463fe1  call       dword ptr [0x49809c]

// block 00463fe7
00463fe7  mov        dword ptr [0x4ae590], eax
00463fec  cmp        eax, -1
00463fef  jne        0x464052

// block 00463ff1
00463ff1  push       0
00463ff3  push       0
00463ff5  lea        eax, [esp + 8]
00463ff9  push       eax
00463ffa  push       0x400
00463fff  call       dword ptr [0x4980e4]

// block 00464005
00464005  push       eax
00464006  push       0
00464008  push       0x1300
0046400d  call       dword ptr [0x4980fc]

// block 00464013
00464013  mov        ecx, dword ptr [esp]
00464016  push       ecx
00464017  push       esi
00464018  push       0x4a3680
0046401d  call       0x4654b0

// block 00464022
00464022  mov        edx, dword ptr [esp + 0xc]
00464026  add        esp, 0xc
00464029  push       edx
0046402a  call       dword ptr [0x498100]

// block 00464030
00464030  test       dword ptr [0x4cee78], 0x8000
0046403a  je         0x46404d

// block 0046403c
0046403c  push       0x4cf128
00464041  call       dword ptr [0x49808c]

// block 00464047
00464047  dec        byte ptr [0x4cf21a]

// block 0046404d
0046404d  or         eax, 0xffffffff
00464050  pop        ecx
00464051  ret        

// block 00464052
00464052  push       esi
00464053  push       0x4a36c8
00464058  call       0x4654b0

// block 0046405d
0046405d  add        esp, 8
00464060  xor        eax, eax
00464062  pop        ecx
00464063  ret        
