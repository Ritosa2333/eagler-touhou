
// block 004db148
004db148  salc       
004db149  dec        esp
004db14a  add        dh, ch
004db14c  push       ebx
004db14d  or         al, 0x38
004db14f  imul       ecx, esp, 0xc2ed416b
004db155  push       ebp
004db156  sbb        ah, bl
004db158  jle        0x4db18f

// block 004db15a
004db15a  fnsave     dword ptr [eax - 0x3c]
004db15d  push       ds
004db15e  js         0x4db126

// block 004db160
004db160  lodsd      eax, dword ptr [esi]
004db161  pop        eax
004db162  and        eax, esi
004db164  mov        byte ptr [edx + 0x5a], al
004db167  and        dword ptr [ebx + 0x1c], 0x5538be4e
004db16e  fdiv       qword ptr [ebx - 0x6b]
004db171  cbw        
004db173  push       ebp
004db174  pushal     
004db175  push       edx
