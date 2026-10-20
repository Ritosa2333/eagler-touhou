
// block 004daf23
004daf23  aad        0xdc
004daf25  jl         0x4daf42

// block 004daf27
004daf27  jle        0x4daebe

// block 004daf29
004daf29  xor        byte ptr [ebx + 0x64597bdd], dl
004daf2f  test       byte ptr [0xb5e1cf0e], dh
004daf35  inc        esi
004daf36  jns        0x4daf3e

// block 004daf38
004daf38  mov        esp, 0x4178ec5f
004daf3d  out        dx, eax

// block 004daf3e
004daf3e  popal      
004daf3f  jb         0x4daf94

// block 004daf41

// block 004daf42
004daf42  sbb        byte ptr [eax + 0x45], dl
004daf45  mov        cs, word ptr [edi + 0x7e6b45db]
004daf4b  jmp        0xc278902d
