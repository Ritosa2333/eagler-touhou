
// block 004925ab
004925ab  push       4
004925ad  mov        eax, 0x496daf
004925b2  call       0x491e8c

// block 004925b7
004925b7  call       0x475467

// block 004925bc
004925bc  cmp        dword ptr [eax + 0x94], 0
004925c3  je         0x4925ca

// block 004925c5
004925c5  call       0x472b94

// block 004925ca
004925ca  and        dword ptr [ebp - 4], 0
004925ce  call       0x472b81
