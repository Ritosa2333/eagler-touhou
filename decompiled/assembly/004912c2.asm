
// block 004912c2
004912c2  xor        eax, eax
004912c4  cmp        dword ptr [0x4d52dc], eax
004912ca  je         0x4912d9

// block 004912cc
004912cc  call       0x491202

// block 004912d1
004912d1  and        eax, 0x3f
004912d4  jmp        0x491292

// block 004912d9
004912d9  ret        
