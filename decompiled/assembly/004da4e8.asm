
// block 004da4e8
004da4e8  cli        

// block 004da4e9
004da4e9  cmp        dl, bh
004da4eb  out        0xa1, eax
004da4ed  ror        dword ptr [ebp - 0x5f], 0x81
004da4f1  not        dword ptr [esi]
004da4f3  test       al, 0xea
004da4f5  jnp        0x4da506

// block 004da4f7
004da4f7  mov        edx, 0x94793987
004da4fc  mov        cl, 0xcf
004da4fe  inc        eax
004da4ff  xchg       esp, eax
004da500  sal        dword ptr [edi + 0x7abff0f4], 1

// block 004da506
004da506  das        
004da507  sub        byte ptr [edi + 0x6a], bl
004da50a  xlatb      
004da50b  xchg       edi, eax
004da50c  push       ds
004da50d  pop        ebp
