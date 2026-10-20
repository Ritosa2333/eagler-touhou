
// block 0041cb01
0041cb01  push       edi
0041cb02  mov        edi, esi
0041cb04  call       0x41ca10

// block 0041cb09
0041cb09  pop        edi
0041cb0a  test       eax, eax
0041cb0c  je         0x41cb26

// block 0041cb0e
0041cb0e  test       esi, esi
0041cb10  je         0x41cb22

// block 0041cb12
0041cb12  mov        eax, esi
0041cb14  call       0x41ca70

// block 0041cb19
0041cb19  push       esi
0041cb1a  call       0x46ca4f

// block 0041cb1f
0041cb1f  add        esp, 4

// block 0041cb22
0041cb22  xor        eax, eax
0041cb24  pop        esi
0041cb25  ret        

// block 0041cb26
0041cb26  mov        eax, esi
0041cb28  pop        esi
0041cb29  ret        
