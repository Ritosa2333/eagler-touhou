
// block 0043ccd0
0043ccd0  lea        eax, [eax + eax*2]
0043ccd3  mov        eax, dword ptr [ecx + eax*4 + 0x44]
0043ccd7  push       edi
0043ccd8  xor        edi, edi
0043ccda  cmp        eax, edi
0043ccdc  je         0x43cd2b

// block 0043ccde
0043ccde  push       esi
0043ccdf  nop        

// block 0043cce0
0043cce0  mov        esi, dword ptr [eax + 4]
0043cce3  mov        eax, dword ptr [eax]
0043cce5  cmp        eax, edi
0043cce7  je         0x43cd24

// block 0043cce9
0043cce9  mov        ecx, dword ptr [eax + 0x18a8]
0043ccef  cmp        ecx, edi
0043ccf1  je         0x43ccfc

// block 0043ccf3
0043ccf3  mov        edx, dword ptr [eax + 0x18ac]
0043ccf9  mov        dword ptr [ecx + 8], edx

// block 0043ccfc
0043ccfc  mov        ecx, dword ptr [eax + 0x18ac]
0043cd02  cmp        ecx, edi
0043cd04  je         0x43cd0f

// block 0043cd06
0043cd06  mov        edx, dword ptr [eax + 0x18a8]
0043cd0c  mov        dword ptr [ecx + 4], edx

// block 0043cd0f
0043cd0f  push       eax
0043cd10  mov        dword ptr [eax + 0x18a8], edi
0043cd16  mov        dword ptr [eax + 0x18ac], edi
0043cd1c  call       0x46ca4f

// block 0043cd21
0043cd21  add        esp, 4

// block 0043cd24
0043cd24  mov        eax, esi
0043cd26  cmp        esi, edi
0043cd28  jne        0x43cce0

// block 0043cd2a
0043cd2a  pop        esi

// block 0043cd2b
0043cd2b  pop        edi
0043cd2c  ret        
