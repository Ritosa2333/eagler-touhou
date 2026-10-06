
// block 00422d30
00422d30  push       ecx
00422d31  cmp        dword ptr [eax + 0x58], 9
00422d35  jl         0x422d40

// block 00422d37
00422d37  mov        dword ptr [eax + 0x5c], 0
00422d3e  pop        ecx
00422d3f  ret        

// block 00422d40
00422d40  add        dword ptr [eax + 0x5c], ecx
00422d43  cmp        dword ptr [eax + 0x5c], 5
00422d47  jl         0x422d55

// block 00422d49
00422d49  mov        dword ptr [eax + 0x5c], 0
00422d50  call       0x422ce0

// block 00422d55
00422d55  mov        edx, dword ptr [0x4b0c9c]
00422d5b  mov        eax, dword ptr [0x4b0c98]
00422d60  mov        ecx, dword ptr [0x4b43e4]
00422d66  push       edx
00422d67  push       eax
00422d68  push       ecx
00422d69  call       0x41ce60

// block 00422d6e
00422d6e  pop        ecx
00422d6f  ret        
