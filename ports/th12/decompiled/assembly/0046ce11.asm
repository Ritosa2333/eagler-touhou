
// block 0046ce11
0046ce11  mov        edi, edi
0046ce13  push       ebp
0046ce14  mov        ebp, esp
0046ce16  mov        eax, dword ptr [ebp + 8]
0046ce19  add        ecx, 9
0046ce1c  push       ecx
0046ce1d  add        eax, 9
0046ce20  push       eax
0046ce21  call       0x472ac0

// block 0046ce26
0046ce26  pop        ecx
0046ce27  pop        ecx
0046ce28  xor        ecx, ecx
0046ce2a  test       eax, eax
0046ce2c  setg       cl
0046ce2f  mov        eax, ecx
0046ce31  pop        ebp
0046ce32  ret        4
