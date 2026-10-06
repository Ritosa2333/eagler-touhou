
// block 00473ff5
00473ff5  mov        edi, edi
00473ff7  push       ebp
00473ff8  mov        ebp, esp
00473ffa  push       ebx
00473ffb  push       esi
00473ffc  mov        esi, dword ptr [0x498160]
00474002  push       edi
00474003  mov        edi, dword ptr [ebp + 8]
00474006  push       edi
00474007  call       esi

// block 00474009
00474009  mov        eax, dword ptr [edi + 0xb0]
0047400f  test       eax, eax
00474011  je         0x474016

// block 00474013
00474013  push       eax
00474014  call       esi

// block 00474016
00474016  mov        eax, dword ptr [edi + 0xb8]
0047401c  test       eax, eax
0047401e  je         0x474023

// block 00474020
00474020  push       eax
00474021  call       esi

// block 00474023
00474023  mov        eax, dword ptr [edi + 0xb4]
00474029  test       eax, eax
0047402b  je         0x474030

// block 0047402d
0047402d  push       eax
0047402e  call       esi

// block 00474030
00474030  mov        eax, dword ptr [edi + 0xc0]
00474036  test       eax, eax
00474038  je         0x47403d

// block 0047403a
0047403a  push       eax
0047403b  call       esi

// block 0047403d
0047403d  lea        ebx, [edi + 0x50]
00474040  mov        dword ptr [ebp + 8], 6

// block 00474047
00474047  cmp        dword ptr [ebx - 8], 0x4ad9d8
0047404e  je         0x474059

// block 00474050
00474050  mov        eax, dword ptr [ebx]
00474052  test       eax, eax
00474054  je         0x474059

// block 00474056
00474056  push       eax
00474057  call       esi

// block 00474059
00474059  cmp        dword ptr [ebx - 4], 0
0047405d  je         0x474069

// block 0047405f
0047405f  mov        eax, dword ptr [ebx + 4]
00474062  test       eax, eax
00474064  je         0x474069

// block 00474066
00474066  push       eax
00474067  call       esi

// block 00474069
00474069  add        ebx, 0x10
0047406c  dec        dword ptr [ebp + 8]
0047406f  jne        0x474047

// block 00474071
00474071  mov        eax, dword ptr [edi + 0xd4]
00474077  add        eax, 0xb4
0047407c  push       eax
0047407d  call       esi

// block 0047407f
0047407f  pop        edi
00474080  pop        esi
00474081  pop        ebx
00474082  pop        ebp
00474083  ret        
