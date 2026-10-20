
// block 0048b25d
0048b25d  push       0x10
0048b25f  push       0x4aafb8
0048b264  call       0x46fcec

// block 0048b269
0048b269  push       7
0048b26b  call       0x46ec9a

// block 0048b270
0048b270  pop        ecx
0048b271  xor        esi, esi
0048b273  mov        dword ptr [ebp - 4], esi
0048b276  xor        eax, eax
0048b278  mov        ebx, dword ptr [ebp + 8]
0048b27b  cmp        ebx, esi
0048b27d  setne      al
0048b280  cmp        eax, esi
0048b282  jne        0x48b2a3

// block 0048b284
0048b284  call       0x46e96f

// block 0048b289
0048b289  push       0x16
0048b28b  pop        edi
0048b28c  mov        dword ptr [eax], edi
0048b28e  push       esi
0048b28f  push       esi
0048b290  push       esi
0048b291  push       esi
0048b292  push       esi
0048b293  call       0x470f2d

// block 0048b298
0048b298  add        esp, 0x14
0048b29b  mov        dword ptr [ebp - 0x1c], edi
0048b29e  jmp        0x48b325

// block 0048b2a3
0048b2a3  mov        dword ptr [ebx], esi
0048b2a5  mov        eax, dword ptr [ebp + 0xc]
0048b2a8  cmp        eax, esi
0048b2aa  je         0x48b2ae

// block 0048b2ac
0048b2ac  mov        dword ptr [eax], esi

// block 0048b2ae
0048b2ae  xor        eax, eax
0048b2b0  cmp        dword ptr [ebp + 0x10], esi
0048b2b3  setne      al
0048b2b6  cmp        eax, esi
0048b2b8  je         0x48b284

// block 0048b2ba
0048b2ba  push       dword ptr [ebp + 0x10]
0048b2bd  call       0x48af42

// block 0048b2c2
0048b2c2  pop        ecx
0048b2c3  mov        dword ptr [ebp - 0x20], eax
0048b2c6  cmp        eax, esi
0048b2c8  je         0x48b322

// block 0048b2ca
0048b2ca  push       eax
0048b2cb  call       0x475860

// block 0048b2d0
0048b2d0  mov        edi, eax
0048b2d2  inc        edi
0048b2d3  push       1
0048b2d5  push       edi
0048b2d6  call       0x48e3da

// block 0048b2db
0048b2db  add        esp, 0xc
0048b2de  mov        dword ptr [ebx], eax
0048b2e0  cmp        eax, esi
0048b2e2  jne        0x48b2fb

// block 0048b2e4
0048b2e4  call       0x46e96f

// block 0048b2e9
0048b2e9  mov        dword ptr [eax], 0xc
0048b2ef  call       0x46e96f

// block 0048b2f4
0048b2f4  mov        eax, dword ptr [eax]
0048b2f6  mov        dword ptr [ebp - 0x1c], eax
0048b2f9  jmp        0x48b325

// block 0048b2fb
0048b2fb  push       dword ptr [ebp - 0x20]
0048b2fe  push       edi
0048b2ff  push       eax
0048b300  call       0x47a2b9

// block 0048b305
0048b305  add        esp, 0xc
0048b308  cmp        eax, esi
0048b30a  je         0x48b319

// block 0048b30c
0048b30c  push       esi
0048b30d  push       esi
0048b30e  push       esi
0048b30f  push       esi
0048b310  push       esi
0048b311  call       0x470dc6

// block 0048b316
0048b316  add        esp, 0x14

// block 0048b319
0048b319  mov        eax, dword ptr [ebp + 0xc]
0048b31c  cmp        eax, esi
0048b31e  je         0x48b322

// block 0048b320
0048b320  mov        dword ptr [eax], edi

// block 0048b322
0048b322  mov        dword ptr [ebp - 0x1c], esi

// block 0048b325
0048b325  mov        dword ptr [ebp - 4], 0xfffffffe
0048b32c  call       0x48b33a

// block 0048b331
0048b331  mov        eax, dword ptr [ebp - 0x1c]
0048b334  call       0x46fd31

// block 0048b339
0048b339  ret        
