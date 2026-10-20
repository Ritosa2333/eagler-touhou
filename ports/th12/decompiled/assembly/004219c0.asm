
// block 004219c0
004219c0  mov        eax, dword ptr [esi + 0x494]
004219c6  test       eax, eax
004219c8  je         0x4219d1

// block 004219ca
004219ca  movsx      edx, di
004219cd  mov        ecx, esi
004219cf  call       eax

// block 004219d1
004219d1  push       esi
004219d2  mov        word ptr [esi + 0x3c4], di
004219d9  call       0x455630

// block 004219de
004219de  ret        
