
// block 0048ed4f
0048ed4f  mov        edi, edi
0048ed51  push       ebp
0048ed52  mov        ebp, esp
0048ed54  cmp        dword ptr [ebp + 0x10], -1
0048ed58  jge        0x48ed5e

// block 0048ed5a
0048ed5a  xor        eax, eax
0048ed5c  pop        ebp
0048ed5d  ret        

// block 0048ed5e
0048ed5e  push       dword ptr [ebp + 0x14]
0048ed61  push       dword ptr [ebp + 0x10]
0048ed64  push       dword ptr [ebp + 0xc]
0048ed67  push       dword ptr [ebp + 8]
0048ed6a  call       dword ptr [0x498048]

// block 0048ed70
0048ed70  pop        ebp
0048ed71  ret        
