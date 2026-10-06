
// block 004db529
004db529  mov        bh, 0x79
004db52b  stc        
004db52c  pop        ecx
004db52d  cmp        esi, ebx
004db52f  out        dx, al
004db530  salc       
004db531  movsb      byte ptr es:[edi], byte ptr [esi]
004db532  jnp        0x4db4d7

// block 004db534
004db534  mov        bh, 0x68
004db536  sub        al, 0xe9
004db538  dec        ebx
004db539  ror        dword ptr [ecx], 0x8c
004db53c  stosd      dword ptr es:[edi], eax
004db53d  movsd      dword ptr es:[edi], dword ptr [esi]
004db53e  mov        cl, 0x76
004db540  push       ss
004db541  xlatb      
004db542  cmp        dword ptr [ebx], edi
004db544  xor        bh, bl
004db546  hlt        
