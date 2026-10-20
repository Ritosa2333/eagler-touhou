
// block 004724e0
004724e0  mov        edi, edi
004724e2  push       ebp
004724e3  mov        ebp, esp
004724e5  push       dword ptr [ebp + 0x14]
004724e8  push       0
004724ea  push       dword ptr [ebp + 0x10]
004724ed  push       dword ptr [ebp + 0xc]
004724f0  push       dword ptr [ebp + 8]
004724f3  push       0x47021f
004724f8  call       0x472414

// block 004724fd
004724fd  add        esp, 0x18
00472500  test       eax, eax
00472502  jge        0x472507

// block 00472504
00472504  or         eax, 0xffffffff

// block 00472507
00472507  pop        ebp
00472508  ret        
