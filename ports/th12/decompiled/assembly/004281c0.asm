
// block 004281c0
004281c0  push       edi
004281c1  push       0x490
004281c6  call       0x46c9ea

// block 004281cb
004281cb  mov        edi, eax
004281cd  add        esp, 4
004281d0  test       edi, edi
004281d2  je         0x428200

// block 004281d4
004281d4  push       esi
004281d5  lea        esi, [edi + 0x10]
004281d8  call       0x427ec0

// block 004281dd
004281dd  push       0x490
004281e2  push       0
004281e4  push       edi
004281e5  call       0x477420

// block 004281ea
004281ea  add        esp, 0xc
004281ed  mov        dword ptr [edi + 0x46c], 0x10000
004281f7  mov        dword ptr [0x4b44f4], edi
004281fd  pop        esi
004281fe  jmp        0x428202

// block 00428200
00428200  xor        edi, edi

// block 00428202
00428202  push       edi
00428203  call       0x427fd0

// block 00428208
00428208  test       eax, eax
0042820a  je         0x428226

// block 0042820c
0042820c  test       edi, edi
0042820e  je         0x428222

// block 00428210
00428210  push       ebx
00428211  mov        ebx, edi
00428213  call       0x4280d0

// block 00428218
00428218  push       edi
00428219  call       0x46ca4f

// block 0042821e
0042821e  add        esp, 4
00428221  pop        ebx

// block 00428222
00428222  xor        eax, eax
00428224  pop        edi
00428225  ret        

// block 00428226
00428226  mov        eax, edi
00428228  pop        edi
00428229  ret        
