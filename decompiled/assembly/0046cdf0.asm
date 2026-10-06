
// block 0046cdf0
0046cdf0  mov        edi, edi
0046cdf2  push       ebp
0046cdf3  mov        ebp, esp
0046cdf5  mov        eax, dword ptr [ebp + 8]
0046cdf8  add        ecx, 9
0046cdfb  push       ecx
0046cdfc  add        eax, 9
0046cdff  push       eax
0046ce00  call       0x472ac0

// block 0046ce05
0046ce05  neg        eax
0046ce07  pop        ecx
0046ce08  sbb        eax, eax
0046ce0a  pop        ecx
0046ce0b  neg        eax
0046ce0d  pop        ebp
0046ce0e  ret        4
