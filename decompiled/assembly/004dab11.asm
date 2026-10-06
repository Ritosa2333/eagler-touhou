
// block 004dab11
004dab11  pop        es
004dab12  mov        esi, 0xf33ce779
004dab17  idiv       dword ptr [ebx]
004dab19  stc        
004dab1a  loopne     0x4dab1f

// block 004dab1c
004dab1c  sar        dword ptr [eax - 0xe], cl

// block 004dab1f
004dab1f  pop        ebp
004dab20  jnp        0x4dab95

// block 004dab22
004dab22  jb         0x4daaf0

// block 004dab25
004dab25  sub        al, 0x95
004dab27  mov        esp, 0x18aa3acd
004dab2c  popfd      
004dab2d  in         eax, 7
004dab2f  cdq        
004dab30  pop        ds
004dab31  xchg       ecx, eax
004dab32  cdq        
004dab33  jne        0x4daada

// block 004dab35
004dab35  mov        bl, 0x97
