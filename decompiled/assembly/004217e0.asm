
// block 004217e0
004217e0  mov        eax, dword ptr [0x4b0cb0]
004217e5  cmp        eax, 7
004217e8  jge        0x4217f0

// block 004217ea
004217ea  inc        eax
004217eb  mov        dword ptr [0x4b0cb0], eax

// block 004217f0
004217f0  shl        eax, 6
004217f3  add        eax, 0x4aebf0
004217f8  mov        dword ptr [0x4b452c], eax
004217fd  ret        
