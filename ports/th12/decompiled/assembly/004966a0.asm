
// block 004966a0
004966a0  mov        edi, edi
004966a2  push       ebp
004966a3  mov        ebp, esp
004966a5  xor        eax, eax

// block 004966a7
004966a7  mov        ecx, dword ptr [eax*8 + 0x4b37a8]
004966ae  cmp        ecx, dword ptr [ebp + 8]
004966b1  je         0x4966bd

// block 004966b3
004966b3  inc        eax
004966b4  cmp        eax, 0x1d
004966b7  jl         0x4966a7

// block 004966b9
004966b9  xor        eax, eax
004966bb  pop        ebp
004966bc  ret        

// block 004966bd
004966bd  mov        eax, dword ptr [eax*8 + 0x4b37ac]
004966c4  pop        ebp
004966c5  ret        
