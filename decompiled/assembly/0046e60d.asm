
// block 0046e60d
0046e60d  call       0x475467

// block 0046e612
0046e612  mov        ecx, dword ptr [eax + 0x14]
0046e615  imul       ecx, ecx, 0x343fd
0046e61b  add        ecx, 0x269ec3
0046e621  mov        dword ptr [eax + 0x14], ecx
0046e624  mov        eax, ecx
0046e626  shr        eax, 0x10
0046e629  and        eax, 0x7fff
0046e62e  ret        
