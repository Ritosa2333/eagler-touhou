
// block 0046ed39
0046ed39  mov        edi, edi
0046ed3b  push       ebp
0046ed3c  mov        ebp, esp
0046ed3e  mov        eax, dword ptr [ebp + 8]
0046ed41  push       esi
0046ed42  xor        esi, esi
0046ed44  cmp        eax, esi
0046ed46  jne        0x46ed65

// block 0046ed48
0046ed48  call       0x46e96f

// block 0046ed4d
0046ed4d  push       esi
0046ed4e  push       esi
0046ed4f  push       esi
0046ed50  push       esi
0046ed51  push       esi
0046ed52  mov        dword ptr [eax], 0x16
0046ed58  call       0x470f2d

// block 0046ed5d
0046ed5d  add        esp, 0x14
0046ed60  push       0x16
0046ed62  pop        eax
0046ed63  jmp        0x46ed77

// block 0046ed65
0046ed65  cmp        dword ptr [0x4b3c04], esi
0046ed6b  je         0x46ed48

// block 0046ed6d
0046ed6d  mov        ecx, dword ptr [0x4ad2b0]
0046ed73  mov        dword ptr [eax], ecx
0046ed75  xor        eax, eax

// block 0046ed77
0046ed77  pop        esi
0046ed78  pop        ebp
0046ed79  ret        
