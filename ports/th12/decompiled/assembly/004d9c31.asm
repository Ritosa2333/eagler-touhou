
// block 004d9c31
004d9c31  sbb        al, 0x13
004d9c33  loopne     0x4d9c6d

// block 004d9c6d
004d9c6d  add        eax, 0xfbdfbaec
004d9c72  jb         0x4d9c0c

// block 004d9c74
004d9c74  mov        ch, 0xe7
004d9c76  push       ecx
004d9c77  aad        0x1e
004d9c79  mov        ch, 4
004d9c7b  mov        bl, 0xb2
004d9c7d  jo         0x4d9cfd

// block 004d9c7f
004d9c7f  mov        al, 0xe8
004d9c81  movsb      byte ptr es:[edi], byte ptr [esi]
004d9c82  ja         0x4d9c04

// block 004d9c84
004d9c84  push       esp

// block 004d9cfd
004d9cfd  xor        byte ptr [ebx + 0x61], 0x8b
004d9d01  jl         0x4d9d58

// block 004d9d58
004d9d58  fmul       st(2)

// block 004d9d5a
004d9d5a  xor        eax, 0xd7475bb6
004d9d5f  mov        edx, ss
004d9d61  adc        al, 0xe7
004d9d64  jmp        0x61122b7a
