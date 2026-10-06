
// block 0043c530
0043c530  mov        eax, dword ptr [0x4b44e8]
0043c535  test       eax, eax
0043c537  je         0x43c565

// block 0043c539
0043c539  test       byte ptr [eax + 0x60], 4
0043c53d  jne        0x43c565

// block 0043c53f
0043c53f  cmp        dword ptr [ecx + 0x10], 1
0043c543  jne        0x43c565

// block 0043c545
0043c545  test       dword ptr [0x4d48b8], 0x200
0043c54f  je         0x43c565

// block 0043c551
0043c551  mov        eax, dword ptr [ecx + 0x1d0]
0043c557  cdq        
0043c558  mov        ecx, 6
0043c55d  idiv       ecx
0043c55f  mov        eax, ecx
0043c561  test       edx, edx
0043c563  jne        0x43c56a

// block 0043c565
0043c565  mov        eax, 1

// block 0043c56a
0043c56a  ret        
