
// block 004dac28
004dac28  movsd      dword ptr es:[edi], dword ptr [esi]
004dac29  jb         0x4dabe1

// block 004dac2b
004dac2b  pop        ecx
004dac2c  aad        0xe7
004dac2e  cwde       
004dac2f  fisttp     qword ptr [edi]
004dac31  rol        dword ptr [ebp - 0x3bdf9ebb], 0x97
004dac38  jg         0x4dabc2

// block 004dac3a
004dac3a  jnp        0x4dac2e

// block 004dac3c
004dac3c  push       -0x5c
004dac3e  fldenv     [edx - 0x27]
004dac41  mov        ah, 0xd0
004dac43  out        dx, al
004dac44  in         eax, 0xee
