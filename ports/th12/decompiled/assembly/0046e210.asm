
// block 0046e210
0046e210  mov        edi, edi
0046e212  push       ebp
0046e213  mov        ebp, esp
0046e215  push       esi
0046e216  mov        esi, dword ptr [ebp + 0x14]
0046e219  push       edi
0046e21a  xor        edi, edi
0046e21c  cmp        esi, edi
0046e21e  jne        0x46e224

// block 0046e220
0046e220  xor        eax, eax
0046e222  jmp        0x46e289

// block 0046e224
0046e224  cmp        dword ptr [ebp + 8], edi
0046e227  jne        0x46e244

// block 0046e229
0046e229  call       0x46e96f

// block 0046e22e
0046e22e  push       0x16
0046e230  pop        esi
0046e231  mov        dword ptr [eax], esi

// block 0046e233
0046e233  push       edi
0046e234  push       edi
0046e235  push       edi
0046e236  push       edi
0046e237  push       edi
0046e238  call       0x470f2d

// block 0046e23d
0046e23d  add        esp, 0x14
0046e240  mov        eax, esi
0046e242  jmp        0x46e289

// block 0046e244
0046e244  cmp        dword ptr [ebp + 0x10], edi
0046e247  je         0x46e25f

// block 0046e249
0046e249  cmp        dword ptr [ebp + 0xc], esi
0046e24c  jb         0x46e25f

// block 0046e24e
0046e24e  push       esi
0046e24f  push       dword ptr [ebp + 0x10]
0046e252  push       dword ptr [ebp + 8]
0046e255  call       0x47a330

// block 0046e25a
0046e25a  add        esp, 0xc
0046e25d  jmp        0x46e220

// block 0046e25f
0046e25f  push       dword ptr [ebp + 0xc]
0046e262  push       edi
0046e263  push       dword ptr [ebp + 8]
0046e266  call       0x477420

// block 0046e26b
0046e26b  add        esp, 0xc
0046e26e  cmp        dword ptr [ebp + 0x10], edi
0046e271  je         0x46e229

// block 0046e273
0046e273  cmp        dword ptr [ebp + 0xc], esi
0046e276  jae        0x46e286

// block 0046e278
0046e278  call       0x46e96f

// block 0046e27d
0046e27d  push       0x22
0046e27f  pop        ecx
0046e280  mov        dword ptr [eax], ecx
0046e282  mov        esi, ecx
0046e284  jmp        0x46e233

// block 0046e286
0046e286  push       0x16
0046e288  pop        eax

// block 0046e289
0046e289  pop        edi
0046e28a  pop        esi
0046e28b  pop        ebp
0046e28c  ret        
