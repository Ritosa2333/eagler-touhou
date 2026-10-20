
// block 004d9836
004d9836  or         byte ptr [eax], bl
004d9838  xor        ch, byte ptr [esi + 0x25]
004d983b  pop        eax
004d983c  dec        eax
004d983d  cmc        
004d983e  mov        cl, 0xd4
004d9840  mov        eax, 0x5c7e2644
004d9845  sub        byte ptr [ecx - 0x37006fc7], ch
004d984b  sub        byte ptr [edi], 0xed
004d984e  test       al, 0x41
004d9850  sub        bh, byte ptr [esi]
004d9852  xchg       edx, eax
004d9853  jl         0x4d981b

// block 004d9855
004d9855  ret        0x7c63
