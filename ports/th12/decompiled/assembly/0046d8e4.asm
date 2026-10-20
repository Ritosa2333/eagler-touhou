
// block 0046d8e4
0046d8e4  mov        edi, edi
0046d8e6  push       ebp
0046d8e7  mov        ebp, esp
0046d8e9  push       esi
0046d8ea  call       0x477735

// block 0046d8ef
0046d8ef  mov        esi, eax
0046d8f1  test       esi, esi
0046d8f3  je         0x46d908

// block 0046d8f5
0046d8f5  push       dword ptr [ebp + 8]
0046d8f8  push       esi
0046d8f9  call       0x46d62c

// block 0046d8fe
0046d8fe  neg        eax
0046d900  sbb        eax, eax
0046d902  pop        ecx
0046d903  not        eax
0046d905  pop        ecx
0046d906  and        eax, esi

// block 0046d908
0046d908  pop        esi
0046d909  pop        ebp
0046d90a  ret        
