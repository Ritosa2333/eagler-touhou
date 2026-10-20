
// block 0048c971
0048c971  xor        eax, eax
0048c973  mov        ecx, dword ptr fs:[0]
0048c97a  cmp        dword ptr [ecx + 4], 0x48c8a8
0048c981  jne        0x48c993

// block 0048c983
0048c983  mov        edx, dword ptr [ecx + 0xc]
0048c986  mov        edx, dword ptr [edx + 0xc]
0048c989  cmp        dword ptr [ecx + 8], edx
0048c98c  jne        0x48c993

// block 0048c98e
0048c98e  mov        eax, 1

// block 0048c993
0048c993  ret        
