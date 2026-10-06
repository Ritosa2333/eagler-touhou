
// block 00411400
00411400  push       esi
00411401  mov        esi, eax
00411403  push       esi
00411404  call       0x411420

// block 00411409
00411409  test       eax, eax
0041140b  je         0x411414

// block 0041140d
0041140d  mov        eax, 1
00411412  pop        esi
00411413  ret        

// block 00411414
00411414  add        esi, 4
00411417  call       0x464a80

// block 0041141c
0041141c  xor        eax, eax
0041141e  pop        esi
0041141f  ret        
