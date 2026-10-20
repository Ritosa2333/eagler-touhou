
// block 0046ff67
0046ff67  mov        edi, edi
0046ff69  push       ebp
0046ff6a  mov        ebp, esp
0046ff6c  push       dword ptr [0x4b3d5c]
0046ff72  call       0x4751de

// block 0046ff77
0046ff77  pop        ecx
0046ff78  test       eax, eax
0046ff7a  je         0x46ff8b

// block 0046ff7c
0046ff7c  push       dword ptr [ebp + 8]
0046ff7f  call       eax

// block 0046ff81
0046ff81  pop        ecx
0046ff82  test       eax, eax
0046ff84  je         0x46ff8b

// block 0046ff86
0046ff86  xor        eax, eax
0046ff88  inc        eax
0046ff89  pop        ebp
0046ff8a  ret        

// block 0046ff8b
0046ff8b  xor        eax, eax
0046ff8d  pop        ebp
0046ff8e  ret        
