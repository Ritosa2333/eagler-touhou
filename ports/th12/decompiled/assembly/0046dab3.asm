
// block 0046dab3
0046dab3  mov        eax, dword ptr [ebp - 0x14]
0046dab6  mov        ecx, dword ptr [eax]
0046dab8  mov        ecx, dword ptr [ecx]
0046daba  mov        dword ptr [ebp - 0x1c], ecx
0046dabd  push       eax
0046dabe  push       ecx
0046dabf  call       0x477b33

// block 0046dac4
0046dac4  pop        ecx
0046dac5  pop        ecx
0046dac6  ret        
