
// block 004db190
004db190  movsd      dword ptr es:[edi], dword ptr [esi]
004db191  dec        edx
004db192  and        al, 0x90
004db194  jge        0x4db1bc

// block 004db196
004db196  cmp        byte ptr [esi + 0xca6d7af], bl
004db19c  jo         0x4db218

// block 004db19e
004db19e  push       ds
004db19f  dec        esp
004db1a0  xor        dh, byte ptr [0x1bc3d107]
004db1a6  dec        ecx
004db1a7  mov        ch, 0x45
004db1a9  xchg       esp, eax
004db1aa  push       edx
004db1ab  add        al, 0x4d
004db1ad  xchg       esp, eax
004db1ae  mov        ah, 0x41
004db1b0  mov        edi, 0xfe58466f

// block 004db1bc
004db1bc  mov        dword ptr [0xeac6bb9f], eax
004db1c1  jge        0x4db22e

// block 004db1c3
004db1c3  mov        al, cl
004db1c5  inc        ecx
004db1c6  call       0x36a3e97

// block 004db1cb
004db1cb  xor        bh, byte ptr [edi]
004db1cd  mov        eax, dword ptr [0xc8e1da8b]
004db1d2  lodsd      eax, dword ptr [esi]
004db1d3  das        
004db1d4  fistp      qword ptr [edi + 0x3b76ff42]

// block 004db1da
004db1da  inc        eax
004db1db  movups     xmm1, xmmword ptr [ebp + 0x4f]
004db1df  sal        dword ptr [edi + 0x4eef918], 1
004db1e5  iretd      

// block 004db22e
004db22e  aas        
004db22f  ljmp       0xa37e:0xdb9f446f
