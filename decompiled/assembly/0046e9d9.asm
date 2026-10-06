
// block 0046e9d9
0046e9d9  mov        edi, edi
0046e9db  push       ebp
0046e9dc  mov        ebp, esp
0046e9de  push       esi
0046e9df  mov        esi, dword ptr [ebp + 8]
0046e9e2  xor        eax, eax
0046e9e4  cmp        esi, eax
0046e9e6  jne        0x46e9fa

// block 0046e9e8
0046e9e8  push       eax
0046e9e9  push       eax
0046e9ea  push       eax
0046e9eb  push       eax
0046e9ec  push       eax
0046e9ed  call       0x470f2d

// block 0046e9f2
0046e9f2  add        esp, 0x14
0046e9f5  push       0x16
0046e9f7  pop        eax
0046e9f8  jmp        0x46ea05

// block 0046e9fa
0046e9fa  call       0x46e96f

// block 0046e9ff
0046e9ff  mov        eax, dword ptr [eax]
0046ea01  mov        dword ptr [esi], eax
0046ea03  xor        eax, eax

// block 0046ea05
0046ea05  pop        esi
0046ea06  pop        ebp
0046ea07  ret        
