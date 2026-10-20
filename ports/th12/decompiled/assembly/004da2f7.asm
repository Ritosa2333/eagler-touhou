
// block 004da2f7
004da2f7  sbb        eax, 0xf3924e85
004da2fc  test       byte ptr [ebx + 0x50], bh
004da2ff  loopne     0x4da2b5

// block 004da301
004da301  push       esp
004da302  salc       
004da303  ror        byte ptr [ebx], 1
004da306  add        byte ptr [edi - 0x57], 0x28
004da30a  push       esi
004da30b  cmovnp     eax, dword ptr [ecx]
004da30e  out        0xaf, eax
004da310  loop       0x4da34b

// block 004da312
004da312  sub        byte ptr [esi + esi + 0x222877b], ah
004da319  mov        dh, 0x2c
004da31b  push       esp

// block 004da34b
004da34b  in         eax, 0xfa
