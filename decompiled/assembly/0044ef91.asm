
// block 0044ef91
0044ef91  mov        eax, dword ptr [ebp + 8]
0044ef94  mov        ecx, dword ptr [ebp - 0x14]
0044ef97  mov        dword ptr [ebp - 0x18], eax
0044ef9a  inc        eax
0044ef9b  mov        dword ptr [ebp - 0x10], esp
0044ef9e  push       eax
0044ef9f  mov        byte ptr [ebp - 4], 2
0044efa3  call       0x44f0a0

// block 0044efa8
0044efa8  mov        dword ptr [ebp + 8], eax
0044efab  mov        dword ptr [ebp - 4], 1
0044efb2  mov        eax, 0x44efb8
0044efb7  ret        
