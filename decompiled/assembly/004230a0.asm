
// block 004230a0
004230a0  inc        dword ptr [eax + 0x84]
004230a6  cmp        dword ptr [eax + 0x84], 0xa
004230ad  jl         0x4230b9

// block 004230af
004230af  mov        dword ptr [eax + 0x84], 9

// block 004230b9
004230b9  ret        
