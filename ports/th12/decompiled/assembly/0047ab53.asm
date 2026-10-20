
// block 0047ab53
0047ab53  mov        edi, edi
0047ab55  push       ebp
0047ab56  mov        ebp, esp
0047ab58  mov        eax, dword ptr [ebp + 8]
0047ab5b  mov        eax, dword ptr [eax]
0047ab5d  cmp        dword ptr [eax], 0xe06d7363
0047ab63  jne        0x47ab8f

// block 0047ab65
0047ab65  cmp        dword ptr [eax + 0x10], 3
0047ab69  jne        0x47ab8f

// block 0047ab6b
0047ab6b  mov        eax, dword ptr [eax + 0x14]
0047ab6e  cmp        eax, 0x19930520
0047ab73  je         0x47ab8a

// block 0047ab75
0047ab75  cmp        eax, 0x19930521
0047ab7a  je         0x47ab8a

// block 0047ab7c
0047ab7c  cmp        eax, 0x19930522
0047ab81  je         0x47ab8a

// block 0047ab83
0047ab83  cmp        eax, 0x1994000
0047ab88  jne        0x47ab8f

// block 0047ab8a
0047ab8a  call       0x472b48

// block 0047ab8f
0047ab8f  xor        eax, eax
0047ab91  pop        ebp
0047ab92  ret        4
