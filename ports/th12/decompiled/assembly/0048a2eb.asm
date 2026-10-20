
// block 0048a2eb
0048a2eb  mov        edi, edi
0048a2ed  push       ebp
0048a2ee  mov        ebp, esp
0048a2f0  sub        esp, 0x10
0048a2f3  push       ebx
0048a2f4  push       esi
0048a2f5  push       edi
0048a2f6  push       dword ptr [ebp + 0x1c]
0048a2f9  lea        ecx, [ebp - 0x10]
0048a2fc  mov        ebx, eax
0048a2fe  call       0x46d3b4

// block 0048a303
0048a303  xor        esi, esi
0048a305  cmp        ebx, esi
0048a307  jne        0x48a334

// block 0048a309
0048a309  call       0x46e96f

// block 0048a30e
0048a30e  push       0x16

// block 0048a310
0048a310  pop        edi
0048a311  push       esi
0048a312  push       esi
0048a313  push       esi
0048a314  push       esi
0048a315  push       esi
0048a316  mov        dword ptr [eax], edi
0048a318  call       0x470f2d

// block 0048a31d
0048a31d  add        esp, 0x14
0048a320  cmp        byte ptr [ebp - 4], 0
0048a324  je         0x48a32d

// block 0048a326
0048a326  mov        eax, dword ptr [ebp - 8]
0048a329  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048a32d
0048a32d  mov        eax, edi
0048a32f  jmp        0x48a455

// block 0048a334
0048a334  cmp        dword ptr [ebp + 8], esi
0048a337  jbe        0x48a309

// block 0048a339
0048a339  cmp        dword ptr [ebp + 0xc], esi
0048a33c  jle        0x48a343

// block 0048a33e
0048a33e  mov        eax, dword ptr [ebp + 0xc]
0048a341  jmp        0x48a345

// block 0048a343
0048a343  xor        eax, eax

// block 0048a345
0048a345  add        eax, 9
0048a348  cmp        dword ptr [ebp + 8], eax
0048a34b  ja         0x48a356

// block 0048a34d
0048a34d  call       0x46e96f

// block 0048a352
0048a352  push       0x22
0048a354  jmp        0x48a310

// block 0048a356
0048a356  cmp        byte ptr [ebp + 0x18], 0
0048a35a  je         0x48a37a

// block 0048a35c
0048a35c  mov        edx, dword ptr [ebp + 0x14]
0048a35f  xor        eax, eax
0048a361  cmp        dword ptr [ebp + 0xc], esi
0048a364  setg       al
0048a367  xor        ecx, ecx
0048a369  cmp        dword ptr [edx], 0x2d
0048a36c  sete       cl
0048a36f  mov        edi, eax
0048a371  add        ecx, ebx
0048a373  mov        eax, ecx
0048a375  call       0x48a2a6

// block 0048a37a
0048a37a  mov        edi, dword ptr [ebp + 0x14]
0048a37d  cmp        dword ptr [edi], 0x2d
0048a380  mov        esi, ebx
0048a382  jne        0x48a38a

// block 0048a384
0048a384  mov        byte ptr [ebx], 0x2d
0048a387  lea        esi, [ebx + 1]

// block 0048a38a
0048a38a  cmp        dword ptr [ebp + 0xc], 0
0048a38e  jle        0x48a3a8

// block 0048a390
0048a390  lea        eax, [esi + 1]
0048a393  mov        cl, byte ptr [eax]
0048a395  mov        byte ptr [esi], cl
0048a397  mov        esi, eax
0048a399  mov        eax, dword ptr [ebp - 0x10]
0048a39c  mov        eax, dword ptr [eax + 0xbc]
0048a3a2  mov        eax, dword ptr [eax]
0048a3a4  mov        al, byte ptr [eax]
0048a3a6  mov        byte ptr [esi], al

// block 0048a3a8
0048a3a8  xor        eax, eax
0048a3aa  cmp        byte ptr [ebp + 0x18], al
0048a3ad  sete       al
0048a3b0  add        eax, dword ptr [ebp + 0xc]
0048a3b3  add        esi, eax
0048a3b5  cmp        dword ptr [ebp + 8], -1
0048a3b9  jne        0x48a3c0

// block 0048a3bb
0048a3bb  or         ebx, 0xffffffff
0048a3be  jmp        0x48a3c5

// block 0048a3c0
0048a3c0  sub        ebx, esi
0048a3c2  add        ebx, dword ptr [ebp + 8]

// block 0048a3c5
0048a3c5  push       0x49f30c
0048a3ca  push       ebx
0048a3cb  push       esi
0048a3cc  call       0x47a2b9

// block 0048a3d1
0048a3d1  add        esp, 0xc
0048a3d4  xor        ebx, ebx
0048a3d6  test       eax, eax
0048a3d8  je         0x48a3e7

// block 0048a3da
0048a3da  push       ebx
0048a3db  push       ebx
0048a3dc  push       ebx
0048a3dd  push       ebx
0048a3de  push       ebx
0048a3df  call       0x470dc6

// block 0048a3e4
0048a3e4  add        esp, 0x14

// block 0048a3e7
0048a3e7  lea        ecx, [esi + 2]
0048a3ea  cmp        dword ptr [ebp + 0x10], ebx
0048a3ed  je         0x48a3f2

// block 0048a3ef
0048a3ef  mov        byte ptr [esi], 0x45

// block 0048a3f2
0048a3f2  mov        eax, dword ptr [edi + 0xc]
0048a3f5  inc        esi
0048a3f6  cmp        byte ptr [eax], 0x30
0048a3f9  je         0x48a429

// block 0048a3fb
0048a3fb  mov        eax, dword ptr [edi + 4]
0048a3fe  dec        eax
0048a3ff  jns        0x48a406

// block 0048a401
0048a401  neg        eax
0048a403  mov        byte ptr [esi], 0x2d

// block 0048a406
0048a406  inc        esi
0048a407  cmp        eax, 0x64
0048a40a  jl         0x48a416

// block 0048a40c
0048a40c  cdq        
0048a40d  push       0x64
0048a40f  pop        edi
0048a410  idiv       edi
0048a412  add        byte ptr [esi], al
0048a414  mov        eax, edx

// block 0048a416
0048a416  inc        esi
0048a417  cmp        eax, 0xa
0048a41a  jl         0x48a426

// block 0048a41c
0048a41c  cdq        
0048a41d  push       0xa
0048a41f  pop        edi
0048a420  idiv       edi
0048a422  add        byte ptr [esi], al
0048a424  mov        eax, edx

// block 0048a426
0048a426  add        byte ptr [esi + 1], al

// block 0048a429
0048a429  test       byte ptr [0x4b43ac], 1
0048a430  je         0x48a446

// block 0048a432
0048a432  cmp        byte ptr [ecx], 0x30
0048a435  jne        0x48a446

// block 0048a437
0048a437  push       3
0048a439  lea        eax, [ecx + 1]
0048a43c  push       eax
0048a43d  push       ecx
0048a43e  call       0x47a6a0

// block 0048a443
0048a443  add        esp, 0xc

// block 0048a446
0048a446  cmp        byte ptr [ebp - 4], 0
0048a44a  je         0x48a453

// block 0048a44c
0048a44c  mov        eax, dword ptr [ebp - 8]
0048a44f  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048a453
0048a453  xor        eax, eax

// block 0048a455
0048a455  pop        edi
0048a456  pop        esi
0048a457  pop        ebx
0048a458  leave      
0048a459  ret        
