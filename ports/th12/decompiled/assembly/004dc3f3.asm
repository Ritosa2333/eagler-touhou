
// block 004dc3f3
004dc3f3  push       edx
004dc3f4  mov        edx, 0x9c3b248e

// block 004dc3f9
004dc3f9  lodsb      al, byte ptr [esi]
004dc3fa  or         al, al
004dc3fc  je         0x4dc412

// block 004dc3fe
004dc3fe  xor        dl, al
004dc400  mov        al, 8

// block 004dc402
004dc402  shr        edx, 1
004dc404  jae        0x4dc40c

// block 004dc406
004dc406  xor        edx, 0xc1a7f39a

// block 004dc40c
004dc40c  dec        al
004dc40e  jne        0x4dc402

// block 004dc410
004dc410  jmp        0x4dc3f9

// block 004dc412
004dc412  xchg       edx, eax
004dc413  pop        edx
004dc414  ret        
