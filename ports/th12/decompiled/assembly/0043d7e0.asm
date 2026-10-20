
// block 0043d7e0
0043d7e0  push       esi
0043d7e1  push       0x2bc
0043d7e6  call       0x46c9ea

// block 0043d7eb
0043d7eb  add        esp, 4
0043d7ee  test       eax, eax
0043d7f0  je         0x43d7fd

// block 0043d7f2
0043d7f2  mov        esi, eax
0043d7f4  call       0x43d570

// block 0043d7f9
0043d7f9  mov        esi, eax
0043d7fb  jmp        0x43d7ff

// block 0043d7fd
0043d7fd  xor        esi, esi

// block 0043d7ff
0043d7ff  push       esi
0043d800  call       0x43d600

// block 0043d805
0043d805  test       eax, eax
0043d807  je         0x43d820

// block 0043d809
0043d809  test       esi, esi
0043d80b  je         0x43d81c

// block 0043d80d
0043d80d  push       esi
0043d80e  call       0x43d6d0

// block 0043d813
0043d813  push       esi
0043d814  call       0x46ca4f

// block 0043d819
0043d819  add        esp, 4

// block 0043d81c
0043d81c  xor        eax, eax
0043d81e  pop        esi
0043d81f  ret        

// block 0043d820
0043d820  mov        eax, esi
0043d822  pop        esi
0043d823  ret        
