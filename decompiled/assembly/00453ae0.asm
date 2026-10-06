
// block 00453ae0
00453ae0  cmp        dword ptr [0x4cf4f8], 0
00453ae7  push       ebx
00453ae8  push       esi
00453ae9  jne        0x453af1

// block 00453aeb
00453aeb  or         eax, 0xffffffff
00453aee  pop        esi
00453aef  pop        ebx
00453af0  ret        

// block 00453af1
00453af1  cmp        byte ptr [0x4ceacb], 0
00453af8  je         0x453aeb

// block 00453afa
00453afa  cmp        dword ptr [0x4cf4e8], 0
00453b01  je         0x453aeb

// block 00453b03
00453b03  test       byte ptr [0x4ceae8], 0x10
00453b0a  jne        0x453b1e

// block 00453b0c
00453b0c  mov        ecx, edi
00453b0e  shl        ecx, 8
00453b11  add        ecx, 0x4d3654
00453b17  pop        esi
00453b18  pop        ebx
00453b19  jmp        0x453890

// block 00453b1e
00453b1e  cmp        dword ptr [edi*4 + 0x4d0da8], 0
00453b26  je         0x453aeb

// block 00453b28
00453b28  push       edi
00453b29  push       0x4a2f70
00453b2e  call       0x454b00

// block 00453b33
00453b33  mov        eax, dword ptr [edi*4 + 0x4d0d68]
00453b3a  movzx      ebx, word ptr [eax + 0x2c]
00453b3e  mov        esi, dword ptr [eax + 0x24]
00453b41  imul       esi, ebx
00453b44  add        esp, 8
00453b47  push       0
00453b49  push       0
00453b4b  add        esi, esi
00453b4d  push       0
00453b4f  add        esi, esi
00453b51  push       0
00453b53  shr        esi, 4
00453b56  call       dword ptr [0x4980f8]

// block 00453b5c
00453b5c  push       0x4cf4fc
00453b61  push       0
00453b63  mov        dword ptr [0x4d4758], eax
00453b68  mov        eax, dword ptr [0x4ce940]
00453b6d  push       eax
00453b6e  push       0x4548a0
00453b73  push       0
00453b75  push       0
00453b77  call       dword ptr [0x4980f0]

// block 00453b7d
00453b7d  mov        dword ptr [0x4cf500], eax
00453b82  xor        edx, edx
00453b84  mov        eax, esi
00453b86  div        ebx
00453b88  mov        ecx, dword ptr [0x4d4758]
