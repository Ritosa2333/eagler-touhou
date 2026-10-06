
// block 00459cf0
00459cf0  push       esi
00459cf1  mov        esi, eax
00459cf3  mov        eax, dword ptr [edi + 0x47c]
00459cf9  shr        eax, 5
00459cfc  and        eax, 7
00459cff  cmp        byte ptr [esi + 0x4b5640], al
00459d05  je         0x459dc0

// block 00459d0b
00459d0b  call       0x45a3c0

// block 00459d10
00459d10  mov        eax, dword ptr [edi + 0x47c]
00459d16  shr        eax, 5
00459d19  and        al, 7
00459d1b  mov        byte ptr [esi + 0x4b5640], al
00459d21  movzx      eax, al
00459d24  cmp        eax, 6
00459d27  ja         0x459dc0

// block 00459d2d
00459d2d  jmp        dword ptr [eax*4 + 0x459e2c]

// block 00459d34
00459d34  mov        eax, dword ptr [0x4ce8f0]
00459d39  mov        ecx, dword ptr [eax]
00459d3b  mov        edx, dword ptr [ecx + 0xe4]
00459d41  push       5
00459d43  push       0x13
00459d45  push       eax
00459d46  call       edx

// block 00459d48
00459d48  push       6
00459d4a  jmp        0x459dae

// block 00459d4c
00459d4c  mov        eax, dword ptr [0x4ce8f0]
00459d51  mov        ecx, dword ptr [eax]
00459d53  mov        edx, dword ptr [ecx + 0xe4]
00459d59  push       5
00459d5b  push       0x13
00459d5d  push       eax
00459d5e  call       edx

// block 00459d60
00459d60  push       2
00459d62  jmp        0x459dae

// block 00459d64
00459d64  push       2
00459d66  jmp        0x459d9a

// block 00459d68
00459d68  mov        eax, dword ptr [0x4ce8f0]
00459d6d  mov        ecx, dword ptr [eax]
00459d6f  mov        edx, dword ptr [ecx + 0xe4]
00459d75  push       0xa
00459d77  push       0x13
00459d79  push       eax
00459d7a  call       edx

// block 00459d7c
00459d7c  push       6
00459d7e  jmp        0x459dae

// block 00459d80
00459d80  mov        eax, dword ptr [0x4ce8f0]
00459d85  mov        ecx, dword ptr [eax]
00459d87  mov        edx, dword ptr [ecx + 0xe4]
00459d8d  push       4
00459d8f  push       0x13
00459d91  push       eax
00459d92  call       edx

// block 00459d94
00459d94  push       6
00459d96  jmp        0x459dae

// block 00459d98
00459d98  push       9

// block 00459d9a
00459d9a  mov        eax, dword ptr [0x4ce8f0]
00459d9f  mov        ecx, dword ptr [eax]
00459da1  mov        edx, dword ptr [ecx + 0xe4]
00459da7  push       0x13
00459da9  push       eax
00459daa  call       edx

// block 00459dac
00459dac  push       1

// block 00459dae
00459dae  mov        eax, dword ptr [0x4ce8f0]
00459db3  mov        ecx, dword ptr [eax]
00459db5  mov        edx, dword ptr [ecx + 0xe4]
00459dbb  push       0x14
00459dbd  push       eax
00459dbe  call       edx

// block 00459dc0
00459dc0  mov        eax, dword ptr [edi + 0x480]
00459dc6  shr        eax, 1
00459dc8  and        eax, 1
00459dcb  cmp        byte ptr [esi + 0x4b5646], al
00459dd1  je         0x459e23

// block 00459dd3
00459dd3  call       0x45a3c0

// block 00459dd8
00459dd8  mov        eax, dword ptr [edi + 0x480]
00459dde  shr        eax, 1
00459de0  and        al, 1
00459de2  mov        byte ptr [esi + 0x4b5646], al
00459de8  mov        eax, dword ptr [0x4ce8f0]
00459ded  mov        ecx, dword ptr [eax]
00459def  mov        edx, dword ptr [ecx + 0x114]
00459df5  jne        0x459e04

// block 00459df7
00459df7  push       2
00459df9  push       5
00459dfb  push       0
00459dfd  push       eax
00459dfe  call       edx

// block 00459e00
00459e00  push       2
00459e02  jmp        0x459e0f

// block 00459e04
00459e04  push       1
00459e06  push       5
00459e08  push       0
00459e0a  push       eax
00459e0b  call       edx

// block 00459e0d
00459e0d  push       1

// block 00459e0f
00459e0f  mov        eax, dword ptr [0x4ce8f0]
00459e14  mov        ecx, dword ptr [eax]
00459e16  mov        edx, dword ptr [ecx + 0x114]
00459e1c  push       6
00459e1e  push       0
00459e20  push       eax
00459e21  call       edx

// block 00459e23
00459e23  inc        dword ptr [esi + 0xa8]
00459e29  pop        esi
00459e2a  ret        
