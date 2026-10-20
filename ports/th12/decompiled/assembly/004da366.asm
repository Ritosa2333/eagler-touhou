
// block 004da35d
004da35d  cmpsb      byte ptr [esi], byte ptr es:[edi]
004da35e  or         byte ptr [edx + 0x5756800c], dh

// block 004da366
004da366  loope      0x4da35d

// block 004da368
004da368  mov        esi, 0xa2716dcf
004da36d  pop        edx
004da36e  jle        0x4da339

// block 004da370
004da370  in         al, 0x25
004da372  and        dh, ah
004da374  inc        byte ptr [ecx + 0x9a2834c]
004da37b  mov        ecx, 0x1ab69614
