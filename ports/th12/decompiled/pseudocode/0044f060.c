// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f060; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

class std::string {
    char *m_data;
    unsigned int m_size;
    unsigned int m_capacity;
} std::string;

char sub_44f060(std::string *a0, unsigned int a1, unsigned int a2)
{
    std::string *v0;  // eax
    std::string *v1;  // 4174
    char *v2;  // eax

    if (!a2)
        return 0;
    v0 = &a0->m_size;
    if (a0[2].m_data >= 16)
        v0 = v0->m_data;
    if (a2 < v0)
        return 0;
    v1 = &a0->m_size;
    v2 = std::string::c_str(v1);
    if (&v2[a0[1].m_capacity] <= a2)
        return 0;
    return 1;
}
