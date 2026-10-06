
// block 0046e7de
0046e7de  mov        esp, dword ptr [ebp - 0x18]
0046e7e1  mov        eax, dword ptr [ebp - 0x24]
0046e7e4  mov        dword ptr [ebp - 0x20], eax
0046e7e7  cmp        dword ptr [ebp - 0x1c], 0
0046e7eb  jne        0x46e7f3

// block 0046e7ed
0046e7ed  push       eax
0046e7ee  call       0x472f0b

// block 0046e7f3
0046e7f3  call       0x472f30

// block 0046e7f8
0046e7f8  mov        dword ptr [ebp - 4], 0xfffffffe
