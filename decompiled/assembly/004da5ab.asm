
// block 004da5ab
004da5ab  pop        esp

// block 004da5b1
004da5b1  xlatb      
004da5b2  or         eax, 0xfdb06f93
004da5b7  imul       edx, dword ptr [esi - 0x53], 0xca3b450b
004da5bf  mov        cl, dh
004da5c1  mov        dword ptr ds:[0xd48883d3], eax
004da5c7  in         eax, dx
004da5c8  cmp        dword ptr [0x5994395c], esi
004da5ce  dec        ebp
004da5cf  cmp        ebp, dword ptr [ecx - 0x255aa3f1]
004da5d5  loop       0x4da63d

// block 004da5d7
004da5d7  push       ecx
004da5d8  jg         0x4da5b1

// block 004da5da
004da5da  sbb        dword ptr [ebp - 7], esi
004da5dd  imul       edi, esp, 0xf6b6de35
004da5e3  popfd      
004da5e4  xchg       ebx, eax
004da5e5  inc        ebp
004da5e6  mov        eax, 0x21dc5304
004da5eb  out        0x63, eax

// block 004da63d
004da63d  jl         0x4da6be

// block 004da63f
004da63f  push       ss

// block 004da640
004da640  aad        0xb1
