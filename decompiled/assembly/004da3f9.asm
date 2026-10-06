
// block 004da3f9
004da3f9  std        
004da3fa  push       0x601db36b
004da3ff  stosb      byte ptr es:[edi], al
004da400  jbe        0x4da423

// block 004da402
004da402  cmp        ch, cl
004da404  mov        esi, 0x21120e8e
004da409  jo         0x4da3bf

// block 004da40b
004da40b  cmp        eax, dword ptr [edi - 0x79fd99ca]
004da411  neg        byte ptr [edi - 0x12811873]
004da417  jb         0x4da491

// block 004da419
004da419  sub        cl, dh
004da41b  out        0xf6, al
004da41d  sbb        al, 0x9e

// block 004da491
004da491  lodsb      al, byte ptr [esi]
004da492  pushal     
004da493  test       dword ptr ss:[esi - 0x65], esi
