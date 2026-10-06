
// block 004dac71
004dac71  aaa        
004dac72  cmp        byte ptr [ebx + edx*8 + 0x546dd10], bl
004dac79  idiv       ah
004dac7b  sub        byte ptr [ecx], dl
004dac7d  hlt        

// block 004dacd8
004dacd8  jnp        0x4dac71

// block 004dacda
004dacda  pop        ebp
004dacdb  rcr        edx, cl
004dacdd  mov        byte ptr [ecx + edx*8 - 0x4a537e58], al
004dace4  pop        eax

// block 004dacf5
004dacf5  dec        edi
004dacf6  movsb      byte ptr es:[edi], byte ptr [esi]
004dacf7  loopne     0x4dacd8

// block 004dacf9
004dacf9  cmp        bh, byte ptr [edi]
004dacfb  pop        edx
004dacfc  xchg       esp, eax
004dacfd  xchg       byte ptr [ebx + ebp], dl
004dad00  xchg       edi, eax
