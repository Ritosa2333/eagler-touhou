
// block 0048599a
0048599a  call       0x475467

// block 0048599f
0048599f  mov        ecx, dword ptr [eax + 0x6c]
004859a2  cmp        ecx, dword ptr [0x4adab8]
004859a8  je         0x4859ba

// block 004859aa
004859aa  mov        ecx, dword ptr [0x4ad9d4]
004859b0  test       dword ptr [eax + 0x70], ecx
004859b3  jne        0x4859ba

// block 004859b5
004859b5  call       0x474181

// block 004859ba
004859ba  mov        eax, dword ptr [0x4adf98]
004859bf  ret        
