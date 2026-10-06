
// block 0046ce49
0046ce49  push       0x10
0046ce4b  push       0x4aaa18
0046ce50  call       0x46fcec

// block 0046ce55
0046ce55  xor        eax, eax
0046ce57  mov        dword ptr [ebp - 0x20], eax
0046ce5a  mov        dword ptr [ebp - 4], eax
0046ce5d  mov        dword ptr [ebp - 0x1c], eax

// block 0046ce60
0046ce60  mov        eax, dword ptr [ebp - 0x1c]
0046ce63  cmp        eax, dword ptr [ebp + 0x10]
0046ce66  jge        0x46ce7b

// block 0046ce68
0046ce68  mov        esi, dword ptr [ebp + 8]
0046ce6b  mov        ecx, esi
0046ce6d  call       dword ptr [ebp + 0x14]

// block 0046ce70
0046ce70  add        esi, dword ptr [ebp + 0xc]
0046ce73  mov        dword ptr [ebp + 8], esi
0046ce76  inc        dword ptr [ebp - 0x1c]
0046ce79  jmp        0x46ce60

// block 0046ce7b
0046ce7b  mov        dword ptr [ebp - 0x20], 1
0046ce82  mov        dword ptr [ebp - 4], 0xfffffffe
0046ce89  call       0x46ce96

// block 0046ce8e
0046ce8e  call       0x46fd31

// block 0046ce93
0046ce93  ret        0x14
