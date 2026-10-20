
// block 0047ebae
0047ebae  mov        edi, edi
0047ebb0  push       ebp
0047ebb1  mov        ebp, esp
0047ebb3  push       ecx
0047ebb4  push       ecx
0047ebb5  mov        eax, dword ptr [ebp + 0xc]
0047ebb8  mov        ecx, dword ptr [eax]
0047ebba  push       esi
0047ebbb  mov        esi, dword ptr [ebp + 8]
0047ebbe  mov        dword ptr [esi], ecx
0047ebc0  mov        eax, dword ptr [eax + 4]
0047ebc3  push       0x49de08
0047ebc8  mov        ecx, esi
0047ebca  mov        dword ptr [esi + 4], eax
0047ebcd  call       0x47ea48

// block 0047ebd2
0047ebd2  lea        eax, [ebp - 8]
0047ebd5  push       eax
0047ebd6  call       0x4814b0

// block 0047ebdb
0047ebdb  pop        ecx
0047ebdc  push       eax
0047ebdd  mov        ecx, esi
0047ebdf  call       0x47e7bf

// block 0047ebe4
0047ebe4  push       0x7d
0047ebe6  mov        ecx, esi
0047ebe8  call       0x47e9f6

// block 0047ebed
0047ebed  mov        eax, dword ptr [0x4b4318]
0047ebf2  cmp        byte ptr [eax], 0x40
0047ebf5  jne        0x47ebfd

// block 0047ebf7
0047ebf7  inc        dword ptr [0x4b4318]

// block 0047ebfd
0047ebfd  mov        eax, esi
0047ebff  pop        esi
0047ec00  leave      
0047ec01  ret        
