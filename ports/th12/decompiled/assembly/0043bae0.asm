
// block 0043bae0
0043bae0  cmp        dword ptr [0x4b44e8], 0
0043bae7  je         0x43bb0f

// block 0043bae9
0043bae9  cmp        dword ptr [eax + 0x10], 1
0043baed  jne        0x43bb0f

// block 0043baef
0043baef  test       dword ptr [0x4d48b8], 0x200
0043baf9  je         0x43bb0f

// block 0043bafb
0043bafb  mov        eax, dword ptr [eax + 0x1d0]
0043bb01  cdq        
0043bb02  mov        ecx, 6
0043bb07  idiv       ecx
0043bb09  mov        eax, ecx
0043bb0b  test       edx, edx
0043bb0d  jne        0x43bb14

// block 0043bb0f
0043bb0f  mov        eax, 1

// block 0043bb14
0043bb14  ret        
