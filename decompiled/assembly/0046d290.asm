
// block 0046d290
0046d290  mov        ecx, dword ptr [esp + 0xc]
0046d294  push       edi
0046d295  test       ecx, ecx
0046d297  je         0x46d32f

// block 0046d29d
0046d29d  push       esi
0046d29e  push       ebx
0046d29f  mov        ebx, ecx
0046d2a1  mov        esi, dword ptr [esp + 0x14]
0046d2a5  test       esi, 3
0046d2ab  mov        edi, dword ptr [esp + 0x10]
0046d2af  jne        0x46d2bc

// block 0046d2b1
0046d2b1  shr        ecx, 2
0046d2b4  jne        0x46d33f

// block 0046d2ba
0046d2ba  jmp        0x46d2e3

// block 0046d2bc
0046d2bc  mov        al, byte ptr [esi]
0046d2be  add        esi, 1
0046d2c1  mov        byte ptr [edi], al
0046d2c3  add        edi, 1
0046d2c6  sub        ecx, 1
0046d2c9  je         0x46d2f6

// block 0046d2cb
0046d2cb  test       al, al
0046d2cd  je         0x46d2fe

// block 0046d2cf
0046d2cf  test       esi, 3
0046d2d5  jne        0x46d2bc

// block 0046d2d7
0046d2d7  mov        ebx, ecx
0046d2d9  shr        ecx, 2
0046d2dc  jne        0x46d33f

// block 0046d2de
0046d2de  and        ebx, 3
0046d2e1  je         0x46d2f6

// block 0046d2e3
0046d2e3  mov        al, byte ptr [esi]
0046d2e5  add        esi, 1
0046d2e8  mov        byte ptr [edi], al
0046d2ea  add        edi, 1
0046d2ed  test       al, al
0046d2ef  je         0x46d328

// block 0046d2f1
0046d2f1  sub        ebx, 1
0046d2f4  jne        0x46d2e3

// block 0046d2f6
0046d2f6  mov        eax, dword ptr [esp + 0x10]
0046d2fa  pop        ebx
0046d2fb  pop        esi
0046d2fc  pop        edi
0046d2fd  ret        

// block 0046d2fe
0046d2fe  test       edi, 3
0046d304  je         0x46d31c

// block 0046d306
0046d306  mov        byte ptr [edi], al
0046d308  add        edi, 1
0046d30b  sub        ecx, 1
0046d30e  je         0x46d3ac

// block 0046d314
0046d314  test       edi, 3
0046d31a  jne        0x46d306

// block 0046d31c
0046d31c  mov        ebx, ecx
0046d31e  shr        ecx, 2
0046d321  jne        0x46d397

// block 0046d323
0046d323  mov        byte ptr [edi], al
0046d325  add        edi, 1

// block 0046d328
0046d328  sub        ebx, 1
0046d32b  jne        0x46d323

// block 0046d32d
0046d32d  pop        ebx
0046d32e  pop        esi

// block 0046d32f
0046d32f  mov        eax, dword ptr [esp + 8]
0046d333  pop        edi
0046d334  ret        

// block 0046d335
0046d335  mov        dword ptr [edi], edx
0046d337  add        edi, 4
0046d33a  sub        ecx, 1
0046d33d  je         0x46d2de

// block 0046d33f
0046d33f  mov        edx, 0x7efefeff
0046d344  mov        eax, dword ptr [esi]
0046d346  add        edx, eax
0046d348  xor        eax, 0xffffffff
0046d34b  xor        eax, edx
0046d34d  mov        edx, dword ptr [esi]
0046d34f  add        esi, 4
0046d352  test       eax, 0x81010100
0046d357  je         0x46d335

// block 0046d359
0046d359  test       dl, dl
0046d35b  je         0x46d389

// block 0046d35d
0046d35d  test       dh, dh
0046d35f  je         0x46d37f

// block 0046d361
0046d361  test       edx, 0xff0000
0046d367  je         0x46d375

// block 0046d369
0046d369  test       edx, 0xff000000
0046d36f  jne        0x46d335

// block 0046d371
0046d371  mov        dword ptr [edi], edx
0046d373  jmp        0x46d38d

// block 0046d375
0046d375  and        edx, 0xffff
0046d37b  mov        dword ptr [edi], edx
0046d37d  jmp        0x46d38d

// block 0046d37f
0046d37f  and        edx, 0xff
0046d385  mov        dword ptr [edi], edx
0046d387  jmp        0x46d38d

// block 0046d389
0046d389  xor        edx, edx
0046d38b  mov        dword ptr [edi], edx

// block 0046d38d
0046d38d  add        edi, 4
0046d390  xor        eax, eax
0046d392  sub        ecx, 1
0046d395  je         0x46d3a3

// block 0046d397
0046d397  xor        eax, eax

// block 0046d399
0046d399  mov        dword ptr [edi], eax
0046d39b  add        edi, 4
0046d39e  sub        ecx, 1
0046d3a1  jne        0x46d399

// block 0046d3a3
0046d3a3  and        ebx, 3
0046d3a6  jne        0x46d323

// block 0046d3ac
0046d3ac  mov        eax, dword ptr [esp + 0x10]
0046d3b0  pop        ebx
0046d3b1  pop        esi
0046d3b2  pop        edi
0046d3b3  ret        
