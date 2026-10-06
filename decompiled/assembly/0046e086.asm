
// block 0046e086
0046e086  cmp        dword ptr [ecx + 8], 0
0046e08a  mov        dword ptr [ecx], 0x49cd28
0046e090  je         0x46e09b

// block 0046e092
0046e092  push       dword ptr [ecx + 4]
0046e095  call       0x46c941

// block 0046e09a
0046e09a  pop        ecx

// block 0046e09b
0046e09b  ret        
