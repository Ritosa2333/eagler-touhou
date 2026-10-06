
// block 004696c0
004696c0  mov        edx, dword ptr [eax + 0x1000]
004696c6  add        ecx, edx
004696c8  cmp        ecx, 0x1000
004696ce  jl         0x4696d4

// block 004696d0
004696d0  or         eax, 0xffffffff
004696d3  ret        

// block 004696d4
004696d4  push       esi
004696d5  lea        esi, [ecx + 4]
004696d8  cmp        esi, 0x1000
004696de  mov        dword ptr [eax + 0x1000], ecx
004696e4  jge        0x4696f6

// block 004696e6
004696e6  mov        esi, dword ptr [eax + 0x1004]
004696ec  mov        dword ptr [ecx + eax], esi
004696ef  add        dword ptr [eax + 0x1000], 4

// block 004696f6
004696f6  mov        dword ptr [eax + 0x1004], edx
004696fc  xor        eax, eax
004696fe  pop        esi
004696ff  ret        
