
// block 004318f0
004318f0  mov        eax, dword ptr [esi + 0xc]
004318f3  test       eax, eax
004318f5  je         0x431906

// block 004318f7
004318f7  mov        ecx, dword ptr [eax]
004318f9  mov        edx, dword ptr [ecx + 8]
004318fc  push       eax
004318fd  call       edx

// block 004318ff
004318ff  mov        dword ptr [esi + 0xc], 0

// block 00431906
00431906  ret        
