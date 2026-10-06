
// block 0048d830
0048d830  push       ecx
0048d831  lea        ecx, [esp + 8]
0048d835  sub        ecx, eax
0048d837  and        ecx, 0xf
0048d83a  add        eax, ecx
0048d83c  sbb        ecx, ecx
0048d83e  or         eax, ecx
0048d840  pop        ecx
0048d841  jmp        0x48d060
