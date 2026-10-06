
// block 004da0d9
004da0d9  and        ch, byte ptr [ebp + 0x5fb4c799]
004da0df  mov        ecx, 0x73c0cd08
004da0e4  push       edi

// block 004da0e5
004da0e5  inc        eax
004da0e6  call       dword ptr [ecx - 0x42]

// block 004da0e9
004da0e9  stosd      dword ptr es:[edi], eax
004da0ea  jbe        0x4da0e1

// block 004da0ec
004da0ec  mov        esp, 0x5a510889
004da0f1  sub        eax, 0x5a0eef10
004da0f6  or         al, 0xd7

// block 004da13c
004da13c  jp         0x31deb802

// block 004da142
004da142  dec        ebx
004da143  or         ebp, dword ptr [eax]
004da145  adc        al, 0xdf
004da147  mov        ch, 0xb2
004da149  test       al, 0x7e
004da14b  sbb        al, 0xdf
