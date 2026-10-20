
// block 0046ea29
0046ea29  mov        edi, edi
0046ea2b  push       ebp
0046ea2c  mov        ebp, esp
0046ea2e  push       esi
0046ea2f  mov        esi, dword ptr [ebp + 8]
0046ea32  xor        eax, eax
0046ea34  cmp        esi, eax
0046ea36  jne        0x46ea4a

// block 0046ea38
0046ea38  push       eax
0046ea39  push       eax
0046ea3a  push       eax
0046ea3b  push       eax
0046ea3c  push       eax
0046ea3d  call       0x470f2d

// block 0046ea42
0046ea42  add        esp, 0x14
0046ea45  push       0x16
0046ea47  pop        eax
0046ea48  jmp        0x46ea55

// block 0046ea4a
0046ea4a  call       0x46e982

// block 0046ea4f
0046ea4f  mov        eax, dword ptr [eax]
0046ea51  mov        dword ptr [esi], eax
0046ea53  xor        eax, eax

// block 0046ea55
0046ea55  pop        esi
0046ea56  pop        ebp
0046ea57  ret        
