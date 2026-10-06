
// block 0046cdd0
0046cdd0  mov        edi, edi
0046cdd2  push       ebp
0046cdd3  mov        ebp, esp
0046cdd5  mov        eax, dword ptr [ebp + 8]
0046cdd8  add        ecx, 9
0046cddb  push       ecx
0046cddc  add        eax, 9
0046cddf  push       eax
0046cde0  call       0x472ac0

// block 0046cde5
0046cde5  neg        eax
0046cde7  pop        ecx
0046cde8  sbb        eax, eax
0046cdea  pop        ecx
0046cdeb  inc        eax
0046cdec  pop        ebp
0046cded  ret        4
