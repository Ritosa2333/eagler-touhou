
// block 004d9ac1
004d9ac1  popfd      
004d9ac2  add        al, 0x93
004d9ac4  mov        al, 0x61
004d9ac6  mov        dword ptr [0x8b35f537], eax

// block 004d9af4
004d9af4  dec        ecx
004d9af5  jp         0x4d9b70

// block 004d9b05
004d9b05  add        bh, byte ptr [ebx - 0x248393cf]
004d9b0b  mov        dh, 0x37
004d9b0d  sub        byte ptr [ecx], ch
004d9b0f  pop        ecx
004d9b10  jl         0x4d9ac1

// block 004d9b12
004d9b12  ja         0x4d9af4

// block 004d9b14
004d9b14  and        byte ptr [eax - 0x3b8c8d6e], 0x2d
004d9b1b  or         al, 0x9d
004d9b1d  push       ss
004d9b1e  sub        dh, 0xd2
004d9b21  out        0x59, al
004d9b23  mov        esp, 0xaea91516
004d9b28  iretd      
