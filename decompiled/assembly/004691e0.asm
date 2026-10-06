
// block 004691e0
004691e0  mov        eax, dword ptr [eax + 0x1034]
004691e6  xor        edx, edx
004691e8  cmp        eax, edx
004691ea  je         0x4691fe

// block 004691ec
004691ec  lea        esp, [esp]

// block 004691f0
004691f0  mov        ecx, dword ptr [eax + 4]
004691f3  mov        eax, dword ptr [eax]
004691f5  mov        dword ptr [eax + 4], edx
004691f8  mov        eax, ecx
004691fa  cmp        ecx, edx
004691fc  jne        0x4691f0

// block 004691fe
004691fe  ret        
