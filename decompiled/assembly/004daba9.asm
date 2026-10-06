
// block 004daba9
004daba9  and        dword ptr [eax], esi
004dabab  sahf       
004dabac  jp         0x4dab57

// block 004dabae
004dabae  cmp        byte ptr [ebp + eax - 0x2d], bl
004dabb2  push       esp
004dabb3  push       ecx
004dabb4  jg         0x4dabe6

// block 004dabb7
004dabb7  xchg       edi, eax
004dabb8  jnp        0x4dab61

// block 004dabba
004dabba  out        0xa, al
004dabbc  mov        bl, 0x36
004dabbe  push       edi
004dabbf  cmpsb      byte ptr [esi], byte ptr es:[edi]
004dabc0  jp         0x4dab7d

// block 004dabc2
004dabc2  mov        al, byte ptr [0x2139116e]
