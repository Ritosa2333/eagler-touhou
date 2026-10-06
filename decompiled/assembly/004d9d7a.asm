
// block 004d9d7a
004d9d7a  pop        edx
004d9d7b  xchg       edx, eax
004d9d7c  mov        dl, ch

// block 004d9d7e
004d9d7e  cmp        dword ptr [esi - 0x6a5ad849], ebx
004d9d84  pushfd     
004d9d85  xor        dword ptr [ecx], eax
004d9d87  loop       0x4d9dc0

// block 004d9d89
004d9d89  mov        byte ptr [0x4912847a], al
004d9d8e  jno        0x4d9d22

// block 004d9d90
004d9d90  dec        ebp
004d9d91  dec        edi
