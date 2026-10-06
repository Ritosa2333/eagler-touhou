
// block 004921d6
004921d6  mov        edi, edi
004921d8  push       ebp
004921d9  mov        ebp, esp
004921db  mov        ecx, dword ptr [ebp + 0xc]
004921de  mov        eax, dword ptr [ecx]
004921e0  push       esi
004921e1  mov        esi, dword ptr [ebp + 8]
004921e4  add        eax, esi
004921e6  cmp        dword ptr [ecx + 4], 0
004921ea  jl         0x4921fc

// block 004921ec
004921ec  mov        edx, dword ptr [ecx + 4]
004921ef  mov        ecx, dword ptr [ecx + 8]
004921f2  mov        esi, dword ptr [edx + esi]
004921f5  mov        ecx, dword ptr [esi + ecx]
004921f8  add        ecx, edx
004921fa  add        eax, ecx

// block 004921fc
004921fc  pop        esi
004921fd  pop        ebp
004921fe  ret        
