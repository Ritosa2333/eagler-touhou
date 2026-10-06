
// block 004695e0
004695e0  push       eax
004695e1  mov        eax, dword ptr [esp + 8]
004695e5  call       0x4694f0

// block 004695ea
004695ea  fldz       
004695ec  mov        ecx, dword ptr [esi + 4]
004695ef  mov        dword ptr [ecx + 4], eax
004695f2  mov        eax, dword ptr [esi + 4]
004695f5  xor        edx, edx
004695f7  fstp       dword ptr [eax]
004695f9  cmp        dword ptr [eax + 4], edx
004695fc  sete       dl
004695ff  mov        eax, edx
00469601  ret        4
