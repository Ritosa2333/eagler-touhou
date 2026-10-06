
// block 0044ce10
0044ce10  mov        eax, dword ptr [esp + 8]
0044ce14  sub        eax, 0
0044ce17  push       esi
0044ce18  je         0x44ce62

// block 0044ce1a
0044ce1a  sub        eax, 1
0044ce1d  je         0x44ce49

// block 0044ce1f
0044ce1f  sub        eax, 1
0044ce22  jne        0x44ce5c

// block 0044ce24
0044ce24  mov        edx, dword ptr [esp + 8]
0044ce28  test       edx, edx
0044ce2a  jg         0x44ce5c

// block 0044ce2c
0044ce2c  mov        eax, edx
0044ce2e  jge        0x44ce32

// block 0044ce30
0044ce30  neg        eax

// block 0044ce32
0044ce32  mov        esi, dword ptr [ecx + 4]
0044ce35  cmp        eax, esi
0044ce37  jae        0x44ce5c

// block 0044ce39
0044ce39  mov        eax, dword ptr [ecx + 0xc]
0044ce3c  add        eax, esi

// block 0044ce3e
0044ce3e  add        eax, edx
0044ce40  mov        dword ptr [ecx + 8], eax
0044ce43  mov        al, 1
0044ce45  pop        esi
0044ce46  ret        8

// block 0044ce49
0044ce49  mov        esi, dword ptr [ecx + 0xc]
0044ce4c  add        esi, dword ptr [ecx + 4]
0044ce4f  mov        eax, dword ptr [ecx + 8]
0044ce52  mov        edx, dword ptr [esp + 8]
0044ce56  sub        esi, eax
0044ce58  cmp        esi, edx
0044ce5a  jg         0x44ce3e

// block 0044ce5c
0044ce5c  xor        al, al
0044ce5e  pop        esi
0044ce5f  ret        8

// block 0044ce62
0044ce62  mov        eax, dword ptr [esp + 8]
0044ce66  test       eax, eax
0044ce68  jl         0x44ce5c

// block 0044ce6a
0044ce6a  cmp        eax, dword ptr [ecx + 4]
0044ce6d  jae        0x44ce5c

// block 0044ce6f
0044ce6f  mov        edx, dword ptr [ecx + 0xc]
0044ce72  add        edx, eax
0044ce74  mov        dword ptr [ecx + 8], edx
0044ce77  mov        al, 1
0044ce79  pop        esi
0044ce7a  ret        8
