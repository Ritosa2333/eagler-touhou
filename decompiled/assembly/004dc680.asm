
// block 004dc680
004dc680  mov        eax, dword ptr [esp + 4]
004dc684  mov        edx, dword ptr [esp + 8]
004dc688  mov        dword ptr [ecx + 0x84], eax
004dc68e  mov        dword ptr [ecx + 0x88], edx
004dc694  lea        eax, [edx + eax*4]
004dc697  mov        dword ptr [ecx + 0x8c], eax
004dc69d  add        eax, 0x100
004dc6a2  ret        8
