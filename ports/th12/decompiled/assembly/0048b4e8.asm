
// block 0048b4e8
0048b4e8  push       0xc
0048b4ea  push       0x4aaff8
0048b4ef  call       0x46fcec

// block 0048b4f4
0048b4f4  mov        ecx, dword ptr [ebp + 8]
0048b4f7  xor        edi, edi
0048b4f9  cmp        ecx, edi
0048b4fb  jbe        0x48b52b

// block 0048b4fd
0048b4fd  push       -0x20
0048b4ff  pop        eax
0048b500  xor        edx, edx
0048b502  div        ecx
0048b504  cmp        eax, dword ptr [ebp + 0xc]
0048b507  sbb        eax, eax
0048b509  inc        eax
0048b50a  jne        0x48b52b

// block 0048b50c
0048b50c  call       0x46e96f

// block 0048b511
0048b511  mov        dword ptr [eax], 0xc
0048b517  push       edi
0048b518  push       edi
0048b519  push       edi
0048b51a  push       edi
0048b51b  push       edi
0048b51c  call       0x470f2d

// block 0048b521
0048b521  add        esp, 0x14

// block 0048b524
0048b524  xor        eax, eax
0048b526  jmp        0x48b600

// block 0048b52b
0048b52b  imul       ecx, dword ptr [ebp + 0xc]
0048b52f  mov        esi, ecx
0048b531  mov        dword ptr [ebp + 8], esi
0048b534  cmp        esi, edi
0048b536  jne        0x48b53b

// block 0048b538
0048b538  xor        esi, esi
0048b53a  inc        esi

// block 0048b53b
0048b53b  xor        ebx, ebx
0048b53d  mov        dword ptr [ebp - 0x1c], ebx
0048b540  cmp        esi, -0x20
0048b543  ja         0x48b5ae

// block 0048b545
0048b545  cmp        dword ptr [0x4d6458], 3
0048b54c  jne        0x48b599

// block 0048b54e
0048b54e  add        esi, 0xf
0048b551  and        esi, 0xfffffff0
0048b554  mov        dword ptr [ebp + 0xc], esi
0048b557  mov        eax, dword ptr [ebp + 8]
0048b55a  cmp        eax, dword ptr [0x4d6448]
0048b560  ja         0x48b599

// block 0048b562
0048b562  push       4
0048b564  call       0x46ec9a

// block 0048b569
0048b569  pop        ecx
0048b56a  mov        dword ptr [ebp - 4], edi
0048b56d  push       dword ptr [ebp + 8]
0048b570  call       0x46fa07

// block 0048b575
0048b575  pop        ecx
0048b576  mov        dword ptr [ebp - 0x1c], eax
0048b579  mov        dword ptr [ebp - 4], 0xfffffffe
0048b580  call       0x48b5e4

// block 0048b585
0048b585  mov        ebx, dword ptr [ebp - 0x1c]
0048b588  cmp        ebx, edi
0048b58a  je         0x48b59d

// block 0048b58c
0048b58c  push       dword ptr [ebp + 8]
0048b58f  push       edi
0048b590  push       ebx
0048b591  call       0x477420

// block 0048b596
0048b596  add        esp, 0xc

// block 0048b599
0048b599  cmp        ebx, edi
0048b59b  jne        0x48b5fe

// block 0048b59d
0048b59d  push       esi
0048b59e  push       8
0048b5a0  push       dword ptr [0x4b3c04]
0048b5a6  call       dword ptr [0x4981d4]

// block 0048b5ac
0048b5ac  mov        ebx, eax

// block 0048b5ae
0048b5ae  cmp        ebx, edi
0048b5b0  jne        0x48b5fe

// block 0048b5b2
0048b5b2  cmp        dword ptr [0x4b40bc], edi
0048b5b8  je         0x48b5ed

// block 0048b5ba
0048b5ba  push       esi
0048b5bb  call       0x46ff67

// block 0048b5c0
0048b5c0  pop        ecx
0048b5c1  test       eax, eax
0048b5c3  jne        0x48b53b

// block 0048b5c9
0048b5c9  mov        eax, dword ptr [ebp + 0x10]
0048b5cc  cmp        eax, edi
0048b5ce  je         0x48b524

// block 0048b5d4
0048b5d4  mov        dword ptr [eax], 0xc
0048b5da  jmp        0x48b524

// block 0048b5ed
0048b5ed  cmp        ebx, edi
0048b5ef  jne        0x48b5fe

// block 0048b5f1
0048b5f1  mov        eax, dword ptr [ebp + 0x10]
0048b5f4  cmp        eax, edi
0048b5f6  je         0x48b5fe

// block 0048b5f8
0048b5f8  mov        dword ptr [eax], 0xc

// block 0048b5fe
0048b5fe  mov        eax, ebx

// block 0048b600
0048b600  call       0x46fd31

// block 0048b605
0048b605  ret        
