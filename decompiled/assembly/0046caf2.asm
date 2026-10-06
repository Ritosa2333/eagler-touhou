
// block 0046caf2
0046caf2  mov        edi, edi
0046caf4  push       ebp
0046caf5  mov        ebp, esp
0046caf7  sub        esp, 0x20
0046cafa  push       esi
0046cafb  xor        esi, esi
0046cafd  cmp        dword ptr [ebp + 0xc], esi
0046cb00  jne        0x46cb1f

// block 0046cb02
0046cb02  call       0x46e96f

// block 0046cb07
0046cb07  push       esi
0046cb08  push       esi
0046cb09  push       esi
0046cb0a  push       esi
0046cb0b  push       esi
0046cb0c  mov        dword ptr [eax], 0x16
0046cb12  call       0x470f2d

// block 0046cb17
0046cb17  add        esp, 0x14
0046cb1a  or         eax, 0xffffffff
0046cb1d  jmp        0x46cb46

// block 0046cb1f
0046cb1f  push       dword ptr [ebp + 0x14]
0046cb22  lea        eax, [ebp - 0x20]
0046cb25  push       dword ptr [ebp + 0x10]
0046cb28  mov        dword ptr [ebp - 0x1c], 0x7fffffff
0046cb2f  push       dword ptr [ebp + 0xc]
0046cb32  mov        dword ptr [ebp - 0x14], 0x42
0046cb39  push       eax
0046cb3a  mov        dword ptr [ebp - 0x18], esi
0046cb3d  mov        dword ptr [ebp - 0x20], esi
0046cb40  call       dword ptr [ebp + 8]

// block 0046cb43
0046cb43  add        esp, 0x10

// block 0046cb46
0046cb46  pop        esi
0046cb47  leave      
0046cb48  ret        
