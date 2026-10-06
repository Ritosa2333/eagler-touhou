
// block 0042f410
0042f410  cmp        dword ptr [0x4cea94], 0
0042f417  je         0x42f46a

// block 0042f419
0042f419  mov        eax, dword ptr [0x4ce8f0]
0042f41e  mov        ecx, dword ptr [eax]
0042f420  mov        edx, dword ptr [ecx + 0xe4]
0042f426  push       esi
0042f427  push       edi
0042f428  push       1
0042f42a  push       0xab
0042f42f  push       eax
0042f430  call       edx

// block 0042f432
0042f432  mov        eax, dword ptr [0x4ce8cc]
0042f437  push       eax
0042f438  mov        eax, dword ptr [0x4ceaa8]
0042f43d  call       0x45c900

// block 0042f442
0042f442  mov        edi, dword ptr [0x4ce8cc]
0042f448  mov        eax, 0x1d
0042f44d  call       0x4611d0

// block 0042f452
0042f452  mov        esi, dword ptr [0x4ce8cc]
0042f458  call       0x45a3c0

// block 0042f45d
0042f45d  mov        eax, dword ptr [0x4b4508]
0042f462  pop        edi
0042f463  pop        esi
0042f464  test       eax, eax
0042f466  je         0x42f46a

// block 0042f468
0042f468  call       eax

// block 0042f46a
0042f46a  mov        eax, 1
0042f46f  ret        
