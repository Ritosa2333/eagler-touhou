
// block 004dade3
004dade3  and        eax, 0x122b29f3
004dade8  pushal     
004dade9  inc        ebp
004dadea  xchg       dword ptr [eax], ecx
004dadec  mov        word ptr [ecx + 0x22], cs

// block 004dadf0
004dadf0  cmp        edi, dword ptr [0xa225a00]
004dadf6  jnp        0x4dadde

// block 004dadf8
004dadf8  jg         0x4dad8c

// block 004dadfa
004dadfa  jg         0x4dadf0

// block 004dadfc
004dadfc  mov        bh, 0x29
004dadfe  inc        ebx
