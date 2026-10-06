
// block 004d977e
004d977e  mov        al, 0x5e
004d9780  cmp        al, 0xe
004d9782  test       eax, 0x34630870

// block 004d978d
004d978d  xchg       edi, eax
004d978e  mov        edi, 0xcec326e1
004d9793  in         al, dx
004d9794  int3       
