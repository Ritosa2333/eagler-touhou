
// block 004db23b
004db23b  xchg       edx, eax
004db23c  test       byte ptr [esi], ah
004db23e  mov        eax, dword ptr [0x3ff67ce8]

// block 004db26d
004db26d  jg         0x4db294

// block 004db26f
004db26f  adc        ch, byte ptr [ecx]
004db271  or         dh, cl
004db273  pop        es

// block 004db275
004db275  cmp        dword ptr [edi - 0x176a015c], edi
004db27b  test       dword ptr [eax + edx*4], ebx
004db27e  adc        dword ptr [eax - 0x19], ecx
004db281  cld        
004db282  jnp        0x4db275

// block 004db284
004db284  pop        esi
004db285  ljmp       0x94ff:0x8b7f7622

// block 004db28c
004db28c  or         dword ptr [eax + 0x76], 0x4f7dfff5
004db293  cld        

// block 004db294
004db294  fdivr      qword ptr [ecx + eax*8 - 0xe]
004db298  fild       word ptr [eax - 0x5600b6b5]
004db29e  jg         0x4db23b

// block 004db2a0
004db2a0  adc        eax, 0x483c39d2
004db2a5  and        byte ptr [eax], 0x19
004db2a9  mov        ah, 0xaf
004db2ab  lahf       
004db2ac  pmaxub     mm1, mm0
004db2af  jl         0x4db2c4

// block 004db2b1
004db2b1  mov        eax, dword ptr [0xac1ca45d]
004db2b6  fstp       qword ptr [edi]
004db2b8  jmp        0x4db2ec

// block 004db2bc
004db2bc  sti        

// block 004db2bd
004db2bd  dec        esp
004db2be  imul       edi

// block 004db2c4
004db2c4  add        ecx, esp
004db2c6  mov        esp, 0x7d15f6ec
004db2cb  jo         0x4db32c

// block 004db2cd
004db2cd  mov        edx, 0x26f72799
004db2d2  cld        
004db2d3  jne        0x4db34e

// block 004db2d5

// block 004db2d6
004db2d6  mov        edx, 0x1e232fcf
004db2db  iret       

// block 004db32c
004db32c  lodsd      eax, dword ptr [esi]
004db32d  in         eax, dx
004db32e  pop        eax

// block 004db34e
004db34e  xchg       esi, eax
004db34f  ret        0xc5ab
