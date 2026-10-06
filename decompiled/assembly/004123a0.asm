
// block 004123a0
004123a0  push       esi
004123a1  mov        esi, ecx
004123a3  push       esi
004123a4  call       0x413570

// block 004123a9
004123a9  test       byte ptr [esp + 8], 1
004123ae  je         0x4123b9

// block 004123b0
004123b0  push       esi
004123b1  call       0x46ca4f

// block 004123b6
004123b6  add        esp, 4

// block 004123b9
004123b9  mov        eax, esi
004123bb  pop        esi
004123bc  ret        4
