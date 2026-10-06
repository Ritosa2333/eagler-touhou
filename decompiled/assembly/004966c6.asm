
// block 004966c6
004966c6  mov        edi, edi
004966c8  push       ebp
004966c9  mov        ebp, esp
004966cb  mov        al, byte ptr [ebp + 8]
004966ce  test       al, 0x20
004966d0  je         0x4966d6

// block 004966d2
004966d2  push       5
004966d4  jmp        0x4966ed

// block 004966d6
004966d6  test       al, 8
004966d8  je         0x4966df

// block 004966da
004966da  xor        eax, eax
004966dc  inc        eax
004966dd  pop        ebp
004966de  ret        

// block 004966df
004966df  test       al, 4
004966e1  je         0x4966e7

// block 004966e3
004966e3  push       2
004966e5  jmp        0x4966ed

// block 004966e7
004966e7  test       al, 1
004966e9  je         0x4966f0

// block 004966eb
004966eb  push       3

// block 004966ed
004966ed  pop        eax
004966ee  pop        ebp
004966ef  ret        

// block 004966f0
004966f0  movzx      eax, al
004966f3  and        eax, 2
004966f6  add        eax, eax
004966f8  pop        ebp
004966f9  ret        
