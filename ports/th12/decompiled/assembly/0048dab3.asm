
// block 0048dab3
0048dab3  mov        edi, edi
0048dab5  push       ebp
0048dab6  mov        ebp, esp
0048dab8  xor        eax, eax
0048daba  cmp        dword ptr [ebp + 0x14], 0xa
0048dabe  jne        0x48dac6

// block 0048dac0
0048dac0  cmp        dword ptr [ebp + 8], eax
0048dac3  jge        0x48dac6

// block 0048dac5
0048dac5  inc        eax

// block 0048dac6
0048dac6  mov        ecx, dword ptr [ebp + 0xc]
0048dac9  push       eax
0048daca  push       dword ptr [ebp + 0x14]
0048dacd  mov        eax, dword ptr [ebp + 8]
0048dad0  push       dword ptr [ebp + 0x10]
0048dad3  call       0x48d9ac

// block 0048dad8
0048dad8  pop        ebp
0048dad9  ret        
