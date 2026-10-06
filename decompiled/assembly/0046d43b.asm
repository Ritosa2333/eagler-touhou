
// block 0046d43b
0046d43b  cmp        byte ptr [ecx + 0xc], 0
0046d43f  je         0x46d448

// block 0046d441
0046d441  mov        eax, dword ptr [ecx + 8]
0046d444  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0046d448
0046d448  ret        
