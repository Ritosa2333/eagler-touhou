
// block 004db660
004db660  jno        0x4db6ba

// block 004db662
004db662  in         eax, dx
004db663  mov        ds, word ptr [ecx + 0x20]
004db666  push       ecx
004db667  in         al, dx
004db668  jecxz      0x4db61f

// block 004db66a
004db66a  mov        edx, 0x249ae758

// block 004db6ba
004db6ba  out        dx, al
