
// block 0046e7ca
0046e7ca  mov        eax, dword ptr [ebp - 0x14]
0046e7cd  mov        ecx, dword ptr [eax]
0046e7cf  mov        ecx, dword ptr [ecx]
0046e7d1  mov        dword ptr [ebp - 0x24], ecx
0046e7d4  push       eax
0046e7d5  push       ecx
0046e7d6  call       0x477b33

// block 0046e7db
0046e7db  pop        ecx
0046e7dc  pop        ecx
0046e7dd  ret        
