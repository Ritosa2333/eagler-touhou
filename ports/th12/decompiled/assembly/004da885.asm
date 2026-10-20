
// block 004da885
004da885  lodsb      al, byte ptr [esi]
004da886  inc        esi
004da887  add        ebx, ebx
004da889  mov        al, 5
004da88b  mov        eax, dword ptr [0x7d1d553a]
004da890  xchg       esp, eax
004da891  pop        ds
004da892  adc        al, 0xe5
004da894  jns        0x4da875

// block 004da896
004da896  fdivp      st(4)
004da898  jnp        0x4da8e1

// block 004da89a
004da89a  jp         0x4da81f

// block 004da89c
004da89c  mov        eax, 0x5bd46dc9
004da8a1  clc        
004da8a2  mov        esi, dword ptr [edi*2 - 0x268bcd1a]

// block 004da8e1
004da8e1  cmp        edx, ecx
004da8e3  dec        esp
004da8e4  adc        bl, byte ptr [ecx - 0x3a26ea03]
004da8ea  inc        esi
004da8eb  inc        ebp
004da8ec  jg         0x4da952

// block 004da8ee
004da8ee  push       esp
004da8ef  pop        ebp
004da8f0  loop       0x4da912

// block 004da8f2
004da8f2  pop        ecx
004da8f3  sub        dword ptr [esi - 0x28], ecx
004da8f6  jns        0x4da911

// block 004da8f8
004da8f8  dec        esp
004da8f9  inc        ecx
004da8fa  and        byte ptr [eax - 0x75], bl
004da8fd  dec        edx
004da8fe  in         eax, 0x3e
004da900  sub        dword ptr [eax - 0x59], 0x2beb03c6
004da907  cwde       
004da908  add        eax, 0xcefa70f1
004da90d  jge        0x4da938

// block 004da911
004da911  mov        ds, word ptr [ebx + edx*8 + 0x72301bce]
004da918  stosb      byte ptr es:[edi], al
004da919  inc        ebp
004da91a  out        0x12, al
004da91c  popal      
004da91d  cmp        eax, 0xc988d3fc

// block 004da938
004da938  fimul      dword ptr [ebp - 0x6d]
004da93b  ret        0xb9fb
