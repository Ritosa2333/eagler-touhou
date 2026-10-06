
// block 004d9915
004d9915  mov        cl, byte ptr [ebx - 0x3f]
004d9918  jecxz      0x4d997b

// block 004d992a
004d992a  jbe        0x4d992f

// block 004d992c
004d992c  mov        al, 0x6b
004d992e  push       esi

// block 004d992f
004d992f  mov        al, byte ptr [0x836f1f62]
004d9934  cmc        
004d9935  iretd      

// block 004d997b
004d997b  jg         0x4d992a

// block 004d997d
004d997d  daa        
004d997e  pop        esp
