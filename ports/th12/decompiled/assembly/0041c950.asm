
// block 0041c950
0041c950  mov        ecx, dword ptr [eax + 4]
0041c953  xor        edx, edx
0041c955  push       esi
0041c956  cmp        ecx, edx
0041c958  je         0x41c960

// block 0041c95a
0041c95a  mov        esi, dword ptr [eax + 8]
0041c95d  mov        dword ptr [ecx + 8], esi

// block 0041c960
0041c960  mov        ecx, dword ptr [eax + 8]
0041c963  cmp        ecx, edx
0041c965  je         0x41c96d

// block 0041c967
0041c967  mov        esi, dword ptr [eax + 4]
0041c96a  mov        dword ptr [ecx + 4], esi

// block 0041c96d
0041c96d  mov        dword ptr [eax + 8], edx
0041c970  mov        dword ptr [eax + 4], edx
0041c973  pop        esi
0041c974  ret        
