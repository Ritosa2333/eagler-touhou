
// block 0040d6e0
0040d6e0  mov        eax, dword ptr [esi + 0x494]
0040d6e6  test       eax, eax
0040d6e8  je         0x40d6f1

// block 0040d6ea
0040d6ea  movsx      edx, di
0040d6ed  mov        ecx, esi
0040d6ef  call       eax

// block 0040d6f1
0040d6f1  mov        word ptr [esi + 0x3c4], di
0040d6f8  ret        
