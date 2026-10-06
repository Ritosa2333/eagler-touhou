
// block 004d927e
004d927e  inc        dword ptr [eax]
004d9280  add        byte ptr [eax], bh
004d9286  add        byte ptr [eax], al
004d9288  add        byte ptr [eax], al
004d928b  add        byte ptr [eax], al
004d928d  add        byte ptr [eax], al
004d928f  add        byte ptr [eax], al
004d9291  add        byte ptr [eax], al
004d9293  add        byte ptr [eax], al
004d9295  add        byte ptr [eax], al
004d9297  add        byte ptr [eax], al
004d9299  add        byte ptr [eax], al
004d929b  add        byte ptr [eax], al
004d929d  add        byte ptr [eax], al
004d929f  add        byte ptr [eax], al
004d92a1  add        byte ptr [eax], al
004d92a3  add        byte ptr [eax], al
004d92a5  add        byte ptr [eax], al
004d92a7  add        byte ptr [eax], al
004d92a9  add        byte ptr [eax], al
004d92ab  add        byte ptr [eax], al
004d92ad  loopne     0x4d92af

// block 004d92af
004d92af  add        byte ptr [eax], al
004d92b1  push       cs
004d92b2  pop        ds
004d92b3  mov        edx, 0x9b4000e
004d92b8  int        0x21

// block 004d92ba
004d92ba  mov        eax, 0x21cd4c01
004d92bf  push       esp
004d92c0  push       0x70207369
004d92c5  jb         0x4d9336

// block 004d92c7
004d92c7  jb         0x4d932b

// block 004d932b
004d932b  lodsd      eax, dword ptr [esi]
004d932c  sal        dword ptr [esi], 1
