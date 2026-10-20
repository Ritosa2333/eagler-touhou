
// block 0043e840
0043e840  push       esi
0043e841  push       0x2e4
0043e846  call       0x46c9ea

// block 0043e84b
0043e84b  add        esp, 4
0043e84e  test       eax, eax
0043e850  je         0x43e85d

// block 0043e852
0043e852  mov        esi, eax
0043e854  call       0x43e3e0

// block 0043e859
0043e859  mov        esi, eax
0043e85b  jmp        0x43e85f

// block 0043e85d
0043e85d  xor        esi, esi

// block 0043e85f
0043e85f  push       esi
0043e860  call       0x43e470

// block 0043e865
0043e865  test       eax, eax
0043e867  je         0x43e880

// block 0043e869
0043e869  test       esi, esi
0043e86b  je         0x43e87c

// block 0043e86d
0043e86d  push       esi
0043e86e  call       0x43e520

// block 0043e873
0043e873  push       esi
0043e874  call       0x46ca4f

// block 0043e879
0043e879  add        esp, 4

// block 0043e87c
0043e87c  xor        eax, eax
0043e87e  pop        esi
0043e87f  ret        

// block 0043e880
0043e880  mov        eax, esi
0043e882  pop        esi
0043e883  ret        
