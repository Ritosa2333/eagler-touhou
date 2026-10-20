
// block 004dac54
004dac54  ja         0x4dac91

// block 004dac56
004dac56  out        dx, al
004dac57  add        bh, byte ptr [esi + ebx*8 - 0x1a]
004dac5b  jle        0x4dacc3

// block 004dac5d
004dac5d  or         dword ptr [edx], esi

// block 004dacc3
004dacc3  salc       
