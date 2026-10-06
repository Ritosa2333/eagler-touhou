
// block 004dab96
004dab96  mov        bl, 0x93
004dab98  jl         0x4dabce

// block 004dab9a
004dab9a  mov        ah, 0xed

// block 004dabce
004dabce  or         dword ptr [ecx - 0x488e69d8], esi
004dabd4  nop        
004dabd5  aam        0x4a
004dabd7  xor        ah, al
004dabd9  out        dx, eax
004dabda  sbb        ecx, dword ptr [edx - 0x67]
004dabdd  sbb        esi, dword ptr [esi - 0x1492e9ac]
004dabe3  push       0x73847f4e
004dabe8  imul       dword ptr [edi - 0x5f29c9e0]
004dabee  inc        ebp
004dabef  call       0xa182dc7f

// block 004dabf4
004dabf4  loopne     0x4dac06

// block 004dabf6
004dabf6  loope      0x4dac33

// block 004dabf8
004dabf8  cmp        byte ptr [edi + 0x46], ch
004dabfb  adc        dword ptr [edx - 0x4b], 0xabd628ab
004dac02  in         al, 0x72
