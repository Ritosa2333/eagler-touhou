
// block 004da73c
004da73c  loope      0x4da7a2

// block 004da73e
004da73e  cdq        
004da73f  pop        edi
004da740  jns        0x4da745

// block 004da742
004da742  add        ebp, ebp
004da744  or         dword ptr [ebx - 0x29], esi

// block 004da747
004da747  mov        word ptr [ecx - 0x5e], ss
004da74a  mov        dword ptr [0x1e0fcc76], eax
004da74f  out        dx, eax
004da750  push       0x78aee6e0
004da755  jp         0x4da7a7

// block 004da7a7
004da7a7  popal      
004da7a8  mov        ch, 0x55
004da7aa  popal      
004da7ab  mov        dh, 0x7a
004da7ad  iretd      
