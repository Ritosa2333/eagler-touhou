
// block 004d9937
004d9937  mov        ebx, 0xf7ecf9d4
004d993c  or         al, 0x2d
004d993e  loop       0x4d99b0

// block 004d9940
004d9940  ret        0x7b99

// block 004d99b0
004d99b0  dec        ebp
004d99b1  cmp        byte ptr [edx], ah
004d99b3  or         al, 0xd0
004d99b5  dec        eax
