
// block 0045a3c0
0045a3c0  cmp        dword ptr [esi + 0x4b56a0], 0
0045a3c7  je         0x45a494

// block 0045a3cd
0045a3cd  mov        eax, dword ptr [0x4ce8cc]
0045a3d2  cmp        byte ptr [eax + 0x4b5647], 1
0045a3d9  je         0x45a413

// block 0045a3db
0045a3db  mov        eax, dword ptr [0x4ce8f0]
0045a3e0  mov        ecx, dword ptr [eax]
0045a3e2  mov        edx, dword ptr [ecx + 0x10c]
0045a3e8  push       4
0045a3ea  push       4
0045a3ec  push       0
0045a3ee  push       eax
0045a3ef  call       edx

// block 0045a3f1
0045a3f1  mov        eax, dword ptr [0x4ce8f0]
0045a3f6  mov        ecx, dword ptr [eax]
0045a3f8  mov        edx, dword ptr [ecx + 0x10c]
0045a3fe  push       4
0045a400  push       1
0045a402  push       0
0045a404  push       eax
0045a405  call       edx

// block 0045a407
0045a407  mov        eax, dword ptr [0x4ce8cc]
0045a40c  mov        byte ptr [eax + 0x4b5647], 1

// block 0045a413
0045a413  mov        eax, dword ptr [0x4ce8f0]
0045a418  mov        ecx, dword ptr [eax]
0045a41a  mov        edx, dword ptr [ecx + 0x10c]
0045a420  push       0
0045a422  push       6
0045a424  push       0
0045a426  push       eax
0045a427  call       edx

// block 0045a429
0045a429  mov        eax, dword ptr [0x4ce8f0]
0045a42e  mov        ecx, dword ptr [eax]
0045a430  mov        edx, dword ptr [ecx + 0x10c]
0045a436  push       0
0045a438  push       3
0045a43a  push       0
0045a43c  push       eax
0045a43d  call       edx

// block 0045a43f
0045a43f  mov        eax, dword ptr [0x4ce8f0]
0045a444  mov        ecx, dword ptr [eax]
0045a446  mov        edx, dword ptr [ecx + 0x164]
0045a44c  push       0x144
0045a451  push       eax
0045a452  call       edx

// block 0045a454
0045a454  mov        edx, dword ptr [esi + 0x8356a8]
0045a45a  mov        eax, dword ptr [0x4ce8f0]
0045a45f  mov        ecx, dword ptr [eax]
0045a461  push       0x1c
0045a463  push       edx
0045a464  mov        edx, dword ptr [esi + 0x4b56a0]
0045a46a  add        edx, edx
0045a46c  push       edx
0045a46d  push       4
0045a46f  push       eax
0045a470  mov        eax, dword ptr [ecx + 0x14c]
0045a476  call       eax

// block 0045a478
0045a478  mov        ecx, dword ptr [esi + 0x8356a4]
0045a47e  inc        dword ptr [esi + 0xac]
0045a484  mov        dword ptr [esi + 0x8356a8], ecx
0045a48a  mov        dword ptr [esi + 0x4b56a0], 0

// block 0045a494
0045a494  ret        
