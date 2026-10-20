
// block 004110e0
004110e0  push       ebx
004110e1  mov        ebx, dword ptr [0x4b43d8]
004110e7  test       ebx, ebx
004110e9  je         0x4110f9

// block 004110eb
004110eb  call       0x410ea0

// block 004110f0
004110f0  push       ebx
004110f1  call       0x46ca4f

// block 004110f6
004110f6  add        esp, 4

// block 004110f9
004110f9  pop        ebx
004110fa  ret        
