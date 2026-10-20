
// block 004dab79
004dab79  dec        esi

// block 004dab7d
004dab7d  sar        eax, 1
004dab7f  xchg       esi, eax
004dab80  out        0x2a, al
004dab82  cmp        al, 0x6c
004dab84  ret        
