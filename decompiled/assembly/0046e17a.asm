
// block 0046e17a
0046e17a  mov        edi, edi
0046e17c  push       ebp
0046e17d  mov        ebp, esp
0046e17f  push       esi
0046e180  mov        esi, ecx
0046e182  call       0x46e086

// block 0046e187
0046e187  test       byte ptr [ebp + 8], 1
0046e18b  je         0x46e194

// block 0046e18d
0046e18d  push       esi
0046e18e  call       0x46ca4f

// block 0046e193
0046e193  pop        ecx

// block 0046e194
0046e194  mov        eax, esi
0046e196  pop        esi
0046e197  pop        ebp
0046e198  ret        4
