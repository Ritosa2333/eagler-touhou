
// block 004759a3
004759a3  mov        edi, edi
004759a5  push       ebp
004759a6  mov        ebp, esp
004759a8  cmp        dword ptr [0x4b40dc], 0
004759af  jne        0x4759c3

// block 004759b1
004759b1  mov        eax, dword ptr [ebp + 8]
004759b4  mov        ecx, dword ptr [0x4adaa8]
004759ba  movzx      eax, word ptr [ecx + eax*2]
004759be  and        eax, dword ptr [ebp + 0xc]
004759c1  pop        ebp
004759c2  ret        

// block 004759c3
004759c3  push       0
004759c5  push       dword ptr [ebp + 0xc]
004759c8  push       dword ptr [ebp + 8]
004759cb  call       0x4758eb

// block 004759d0
004759d0  add        esp, 0xc
004759d3  pop        ebp
004759d4  ret        
