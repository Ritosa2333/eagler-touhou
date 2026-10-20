
// block 0043cdb0
0043cdb0  push       ecx
0043cdb1  push       ebx
0043cdb2  push       esi
0043cdb3  push       edi
0043cdb4  mov        edi, dword ptr [esp + 0x14]
0043cdb8  xor        ebx, ebx
0043cdba  add        edi, -8
0043cdbd  xor        edx, edx
0043cdbf  xor        esi, esi
0043cdc1  xor        eax, eax
0043cdc3  cmp        edi, 2
0043cdc6  mov        dword ptr [esp + 0xc], ebx
0043cdca  jl         0x43cdea

// block 0043cdcc
0043cdcc  lea        ebx, [edi - 1]
0043cdcf  push       ebp

// block 0043cdd0
0043cdd0  movzx      ebp, byte ptr [ecx + eax + 8]
0043cdd5  add        edx, ebp
0043cdd7  movzx      ebp, byte ptr [ecx + eax + 9]
0043cddc  add        eax, 2
0043cddf  add        esi, ebp
0043cde1  cmp        eax, ebx
0043cde3  jl         0x43cdd0

// block 0043cde5
0043cde5  mov        ebx, dword ptr [esp + 0x10]
0043cde9  pop        ebp

// block 0043cdea
0043cdea  cmp        eax, edi
0043cdec  jge        0x43cdf3

// block 0043cdee
0043cdee  movzx      ebx, byte ptr [eax + ecx + 8]

// block 0043cdf3
0043cdf3  pop        edi
0043cdf4  lea        eax, [esi + edx]
0043cdf7  pop        esi
0043cdf8  add        eax, ebx
0043cdfa  pop        ebx
0043cdfb  pop        ecx
0043cdfc  ret        4
