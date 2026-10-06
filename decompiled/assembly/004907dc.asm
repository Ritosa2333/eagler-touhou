
// block 004907dc
004907dc  mov        edi, edi
004907de  push       ebp
004907df  mov        ebp, esp
004907e1  mov        ecx, dword ptr [ebp + 8]
004907e4  jmp        0x4907ed

// block 004907e6
004907e6  dec        ecx
004907e7  cmp        byte ptr [eax], 0
004907ea  je         0x4907f2

// block 004907ec
004907ec  inc        eax

// block 004907ed
004907ed  test       ecx, ecx
004907ef  jne        0x4907e6

// block 004907f1
004907f1  dec        ecx

// block 004907f2
004907f2  mov        eax, dword ptr [ebp + 8]
004907f5  sub        eax, ecx
004907f7  dec        eax
004907f8  pop        ebp
004907f9  ret        
