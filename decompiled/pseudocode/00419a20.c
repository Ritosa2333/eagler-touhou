// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x419a20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_419a20_0 {
    char padding_0[4732];
    unsigned int field_127c;
    unsigned int field_1280;
    unsigned int field_1284;
    unsigned int field_1288;
    char padding_128c[36];
    unsigned int field_12b0;
    char padding_12b4[5012];
    unsigned int field_2648;
    char padding_264c[172];
    unsigned int field_26f8;
} st_419a20_0;

typedef struct st_419a20_1 {
    char padding_0[16];
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    struct st_419a20_0 *field_1c;
    char padding_20[80];
    unsigned int field_70;
    char padding_74[4];
    unsigned int field_78;
} st_419a20_1;

extern unsigned int g_4b0c48;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern char g_4b0ca8;
extern unsigned int g_4b0ccc;
extern st_419a20_1 *g_4b43dc;

unsigned int sub_419a20(st_419a20_0 *a0, unsigned int a1, unsigned int a2)
{
    switch (a2)
    {
    case -10000:
        return sub_464440();
    case -9999:
        sub_4645b0();
        return sub_4931e0();
    case -9997: case -9977:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9996: case -9976:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9995: case -9975:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9994: case -9974:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9993: case -9973:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9992: case -9972:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9991: case -9965:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9990: case -9964:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9988:
        return a0->field_12b0;
    case -9987:
        sub_4645e0();
        return sub_4931e0();
    case -9986:
        return a0->field_26f8 >> 23 & 1;
    case -9985:
        return a0->field_127c;
    case -9984:
        return a0->field_1280;
    case -9982:
        return a0->field_1288;
    case -9981:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9979:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9978:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9971:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9970:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9969:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9968:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9967:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9966:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9963:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9962:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9961:
        return *((short *)(sub_461c50() + 1002));
    case -9960:
        return g_4b0ccc;
    case -9959:
        return *((int *)&g_4b0ca8);
    case -9958:
        sub_41c530();
        return sub_4931e0();
    case -9957:
        return 1;
    case -9954:
        return a0->field_2648;
    case -9953:
        return !*((int *)&g_4b0ca8);
    case -9952:
        return *((int *)&g_4b0ca8) == 1;
    case -9951:
        return *((int *)&g_4b0ca8) == 2;
    case -9950:
        return *((int *)&g_4b0ca8) == 3;
    case -9949:
        return g_4b43dc->field_10;
    case -9948:
        return g_4b43dc->field_14;
    case -9947:
        return g_4b43dc->field_18;
    case -9946:
        return g_4b43dc->field_70;
    case -9945:
        return g_4b0c94 + g_4b0c90 * 2;
    case -9944:
        sub_41c4c0();
        return sub_4931e0();
    case -9943:
        return g_4b43dc->field_1c->field_127c;
    case -9942:
        return g_4b43dc->field_1c->field_1280;
    case -9940:
        return g_4b43dc->field_1c->field_1288;
    case -9939:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9937:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9936:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9935:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9934:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9933:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9932:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    case -9931:
        return g_4b43dc->field_78;
    case -9930:
        return g_4b0c48;
    case -9941:
        a0 = g_4b43dc->field_1c;
    case -9983:
        return a0->field_1284;
    case -9980:
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4931e0();
    default:
        return 0;
    }
}
