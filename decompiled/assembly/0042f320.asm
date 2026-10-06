
// block 0042f320
0042f320  cmp        dword ptr [0x4cea94], 0
0042f327  je         0x42f374

// block 0042f329
0042f329  push       esi
0042f32a  mov        esi, dword ptr [0x4ce8cc]
0042f330  call       0x45a3c0

// block 0042f335
0042f335  mov        edx, dword ptr [0x4cea9c]
0042f33b  mov        eax, dword ptr [0x4ce8f0]
0042f340  mov        ecx, dword ptr [eax]
0042f342  push       edx
0042f343  push       0
0042f345  push       eax
0042f346  mov        eax, dword ptr [ecx + 0x94]
0042f34c  call       eax

// block 0042f34e
0042f34e  fld1       
0042f350  mov        eax, dword ptr [0x4ce8f0]
0042f355  mov        ecx, dword ptr [eax]
0042f357  mov        edx, dword ptr [0x4cf2a8]
0042f35d  push       0
0042f35f  push       ecx
0042f360  fstp       dword ptr [esp]
0042f363  push       edx
0042f364  push       3
0042f366  push       0
0042f368  push       0
0042f36a  push       eax
0042f36b  mov        eax, dword ptr [ecx + 0xac]
0042f371  call       eax

// block 0042f373
0042f373  pop        esi

// block 0042f374
0042f374  mov        eax, 1
0042f379  ret        
