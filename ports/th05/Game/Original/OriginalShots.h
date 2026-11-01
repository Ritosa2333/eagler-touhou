#pragma once
// TH05 shot patterns transcribed from ReC98 b6ba5b0 (th05/p_*.cpp).
// Keep emission order and the original skipping allocator for recorded demos.
namespace th5w { namespace original { namespace shots {
struct Scalar { int v; void set(float f){v=int(f*16);} Scalar& operator+=(float f){v+=int(f*16);return *this;} Scalar& operator-=(float f){v-=int(f*16);return *this;} };
struct Point {Scalar x,y;};
struct Shot {struct {Point cur,velocity;} pos; unsigned char flag,age,patnum_base,type,damage,angle;
 void from_option_l(float offset=0){pos.cur.x-=24+offset;} void from_option_r(float offset=0){pos.cur.x+=24+offset;}
 void set_option_sprite(){patnum_base=22;} void set_option_sprite_and_damage(char d){set_option_sprite();damage=d;}
 void set_random_angle_forwards(unsigned char min=0xb8,unsigned char max=0xc8);
};
static Shot slots[65];static int cursor,shot_time,originX,originY;
static unsigned char randring1_next8_ge_lt(unsigned char min,unsigned char max){return (min+(original::Random()&(unsigned char(max-min)-1)))&255;}
static void shot_velocity_set(Point* p,unsigned char angle){p->x.v=math::X(angle,192);p->y.v=math::Y(angle,192);for(int i=0;i<65;i++)if(p==&slots[i].pos.velocity){slots[i].angle=angle;break;}}
inline void Shot::set_random_angle_forwards(unsigned char min,unsigned char max){shot_velocity_set(&pos.velocity,randring1_next8_ge_lt(min,max));}
static Shot* shots_add(){while(cursor<64){int i=++cursor;if(slots[i].flag)continue;Shot& s=slots[i];s.flag=1;s.age=0;s.pos.cur.x.v=originX;s.pos.cur.y.v=originY;s.pos.velocity.x.v=0;s.pos.velocity.y.v=-192;s.patnum_base=20;s.type=0;s.angle=192;cursor=i+1;return &s;}return nullptr;}
static unsigned char shot_cycle_init(){cursor=0;switch(shot_time){case 18:return 15;case 15:return 4;case 12:return 5;case 9:return 6;case 6:return 5;case 3:return 4;default:return 0;}}
#define ST_HOMING 1
#define ST_MISSILE_LEFT 2
#define ST_MISSILE_RIGHT 3
#define ST_MISSILE_STRAIGHT 4
#define SC_1X 8
#define SC_2X 2
#define SC_3X 1
#define SC_6X 4
struct ShotAddIterator {
	unsigned char angle=0;
	unsigned char i;

	ShotAddIterator(unsigned char count)
		: i(count) {
	}

	void add_secondary(unsigned char n) {
		i += n;
	}

	void set_random_angle(
		unsigned char min = -0x48, unsigned char max = -0x38
	) {
		angle = randring1_next8_ge_lt(min, max);
	}

	unsigned char next(void) {
		return i -= 1;
	}
};

#define shot_func_init( \
	shot, \
	sai, \
	cycle, \
	count, \
	primary_cycle, \
	secondary_cycle, \
	secondary_offset_expr \
) \
	Shot *shot; unsigned char cycle; \
	ShotAddIterator sai(count); \
	\
	cycle = shot_cycle_init(); \
	if((cycle & (primary_cycle | secondary_cycle)) == 0) { \
		return; \
	} \
	if(cycle & secondary_cycle) { \
		sai.secondary_offset_expr; \
	}

// We *really* want to fit those on a line...
#define OPT_L shot->from_option_l()
#define OPT_R shot->from_option_r()
#define MISSILE_L shot->type = ST_MISSILE_LEFT
#define MISSILE_R shot->type = ST_MISSILE_RIGHT
#define MISSILE_S shot->type = ST_MISSILE_STRAIGHT

#define VELOCITY_X(screen_x) shot->pos.velocity.x.set(screen_x);
#define VELOCITY_Y(screen_y) shot->pos.velocity.y.set(screen_y);

#define VELOCITY_XY(screen_x, screen_y) \
	VELOCITY_X(screen_x); \
	VELOCITY_Y(screen_y);

#define VELOCITY_YX(screen_y, screen_x) \
	VELOCITY_Y(screen_y); \
	VELOCITY_X(screen_x);

static void shot_l0(void)
{
	if( (shot_cycle_init() & SC_3X) == 0) {
		return;
	}
	Shot *shot;
	if(( shot = shots_add() ) != nullptr) {
		shot->damage = 10;
	}
}

static void shot_l1(void)
{
	if( (shot_cycle_init() & SC_3X) == 0) {
		return;
	}
	Shot *shot;
	if(( shot = shots_add() ) != nullptr) {
		shot_velocity_set(
			&shot->pos.velocity, randring1_next8_ge_lt(-0x44, -0x3C)
		);
		shot->damage = 10;
	}
}


/* ReC98
 * -----
 * Reimu's shot control functions
 */








#define reimu_forward(shot, sai, dmg, i_left, angle_left, angle_add) \
	if(sai.i == i_left) { \
		sai.angle = angle_left; \
	} \
	shot->damage = dmg; \
	sai.angle += angle_add;

#define reimu_sub_spread_constant(shot, sai, i_left) \
	if(sai.i == i_left)  { shot->from_option_l(); sai.angle = -0x48; } \
	else/*i == i_right*/ { shot->from_option_r(); sai.angle = -0x38; }

#define reimu_sub_spread_random(shot, sai, i_left) \
	if(sai.i == i_left) { \
		shot->from_option_l(); \
		sai.set_random_angle(-0x4A, -0x46); \
	} else /* i == i_right */ { \
		shot->from_option_r(); \
		sai.set_random_angle(-0x3A, -0x36); \
		if(option_only == 0) { \
			sai.i = 1; \
		} \
	}

#define set_homing(shot, dmg) \
	shot->set_option_sprite_and_damage(dmg); \
	shot->type = ST_HOMING;

static void shot_reimu_l2(void)
{
	shot_func_init(shot, sai, cycle, 1, SC_3X, SC_1X, i += 2);

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i == 1) {
			shot->set_random_angle_forwards();
			shot->damage = 10;
		} else {
			reimu_sub_spread_constant(shot, sai, 3);
			shot_velocity_set(&shot->pos.velocity, sai.angle);
			set_homing(shot, 4);
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_reimu_l3(void)
{
	shot_func_init(shot, sai, cycle, 2, SC_3X, SC_1X, i += 2);

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 2) {
			if(sai.i == 2) { shot->pos.cur.x -= 8; }
			else/*i == 1*/ { shot->pos.cur.x += 8; }
			shot->damage = 9;
		} else {
			reimu_sub_spread_constant(shot, sai, 4);
			shot_velocity_set(&shot->pos.velocity, sai.angle);
			set_homing(shot, 4);
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_reimu_l4(void)
{
	shot_func_init(shot, sai, cycle, 3, SC_3X, SC_1X, i += 2);

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 3) {
			reimu_forward(shot, sai, 9, 3, -0x4C, +0x06);
		} else {
			reimu_sub_spread_constant(shot, sai, 5);
			set_homing(shot, 3);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}

#define REIMU_L5 \
	shot_func_init(shot, sai, cycle, 3, SC_3X, SC_2X, add_secondary(2)); \
	char option_only = cycle & SC_3X; \
	\
	while(( shot = shots_add() ) != nullptr) { \
		if(sai.i <= 3) { \
			shot->damage = 9; \
			if(sai.i == 3) { \
				sai.angle = -0x4E; \
			} \
			sai.angle += 0x07; \
		} else { \
			reimu_sub_spread_random(shot, sai, 5); \
			shot->set_option_sprite_and_damage(3); \
			shot->type = ST_HOMING; \
		} \
		shot_velocity_set(&shot->pos.velocity, sai.angle); \
		if(sai.next() <= 0) { \
			break; \
		} \
	} \

static void shot_reimu_l5(void)
{
	REIMU_L5;
}

static void shot_reimu_l6(void)
{
	REIMU_L5;
}

static void shot_reimu_l7(void)
{
	shot_func_init(shot, sai, cycle, 5, SC_3X, SC_2X, add_secondary(2));
	char option_only = cycle & SC_3X;

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 5) {
			reimu_forward(shot, sai, 9, 5, -0x4F, +0x05);
		} else {
			reimu_sub_spread_random(shot, sai, 7);
			set_homing(shot, 3);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_reimu_l8(void)
{
	Shot *shot;
	ShotAddIterator sai(7);
	if( (shot_cycle_init() & SC_3X) == 0) {
		return;
	}
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 5) {
			reimu_forward(shot, sai, 9, 5, -0x4F, +0x05);
		} else {
			if(sai.i == 7) {
				shot->from_option_l(); sai.set_random_angle(-0x4A, -0x46);
			} else/*i == 6*/ {
				shot->from_option_r(); sai.set_random_angle(-0x3A, -0x36);
			}
			set_homing(shot, 3);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_reimu_l9(void)
{
	shot_func_init(shot, sai, cycle, 7, SC_3X, SC_2X, add_secondary(2));
	char option_only = cycle & SC_3X;

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 5) {
			reimu_forward(shot, sai, 9, 5, -0x4F, +0x05);
		} else {
			if(sai.i <= 7) {
				reimu_sub_spread_constant(shot, sai, 7);
			} else {
				if(sai.i == 9) {
					shot->from_option_l();
					sai.angle = -0x56;
				} else /* i == 8 */ {
					shot->from_option_r();
					sai.angle = -0x2A;
					if(option_only == 0) {
						sai.i = 1;
					}
				}
			}
			set_homing(shot, 2);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}


/* ReC98
 * -----
 * Marisa's shot control functions
 */










static const int STAR_DISTANCE = 256;
inline int star_offset(int count) {
	return (128 - ((count / 2) * STAR_DISTANCE));
}

#define marisa_stars(shot, dmg) \
	shot->pos.cur.x.v += star_distance; \
	shot->damage = dmg; \
	shot->pos.velocity.y.set(-12.0f); \
	star_distance += STAR_DISTANCE;

static void shot_marisa_l2(void)
{
#define STAR_COUNT 1
	shot_func_init(shot, sai, cycle, STAR_COUNT, SC_3X, SC_1X, i += 2);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i == STAR_COUNT) {
			shot->set_random_angle_forwards();
			shot->damage = 8;
		} else {
			if(sai.i == 3) { OPT_L;	MISSILE_L;	VELOCITY_X(-1.625f); }
			else/*i == 2*/ { OPT_R;	MISSILE_R;	VELOCITY_X( 1.625f); }
			VELOCITY_Y(2.0f);
			shot->set_option_sprite_and_damage(16);
		}
		if(sai.next() <= 0) {
			break;
		}
	}
#undef STAR_COUNT
}

static void shot_marisa_l3(void)
{
#define STAR_COUNT 2
	shot_func_init(shot, sai, cycle, STAR_COUNT, SC_3X, SC_1X, i += 4);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= STAR_COUNT) {
			if(sai.i == STAR_COUNT) { shot->pos.cur.x.v -= star_offset(0); }
			else /* sai_i == 1 */   { shot->pos.cur.x.v += star_offset(0); }
			shot->damage = 7;
		} else {
			switch(sai.i - 3u) {
			case 0:	OPT_L;	MISSILE_L;	VELOCITY_XY(-1.375f, 2.25f);	break;
			case 1:	OPT_R;	MISSILE_R;	VELOCITY_XY( 1.375f, 2.25f);	break;
			case 2:	OPT_L;	MISSILE_S;	VELOCITY_Y(-3.0f);	break;
			case 3:	OPT_R;	MISSILE_S;	VELOCITY_Y(-3.0f);	break;
			}
			shot->set_option_sprite_and_damage(13);
		}
		if(sai.next() <= 0) {
			break;
		}
	}
#undef STAR_COUNT
}

static void shot_marisa_l4(void)
{
#define STAR_COUNT 3
	shot_func_init(shot, sai, cycle, STAR_COUNT, SC_3X, SC_1X, i += 6);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= STAR_COUNT) {
			if(sai.i == STAR_COUNT) {
				sai.angle = -0x4C;
			}
			shot->damage = 6;
			sai.angle += 0x06;
			shot_velocity_set(&shot->pos.velocity, sai.angle);
		} else {
			switch(sai.i - 4u) {
			case 0: OPT_L;	MISSILE_L;	VELOCITY_XY(-1.375f, 2.25f);	break;
			case 1: OPT_R;	MISSILE_R;	VELOCITY_XY( 1.375f, 2.25f);	break;
			case 2: OPT_L;	MISSILE_S;	VELOCITY_XY(-1.0f, -3.0f);	break;
			case 3: OPT_R;	MISSILE_S;	VELOCITY_XY( 1.0f, -3.0f);	break;
			case 4:	OPT_L;	MISSILE_S;	VELOCITY_XY( 0.0f, -4.0f);	break;
			case 5: OPT_R;	MISSILE_S;	VELOCITY_XY( 0.0f, -4.0f);	break;
			}
			shot->set_option_sprite_and_damage(12);
		}
		if(sai.next() <= 0) {
			break;
		}
	}
#undef STAR_COUNT
}

static void shot_marisa_l5(void)
{
#define STAR_COUNT 3
	shot_func_init(shot, sai, cycle, STAR_COUNT, SC_3X, SC_1X, i += 6);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= STAR_COUNT) {
			shot->damage = 6;
			if(sai.i == STAR_COUNT) {
				sai.angle = -0x4E;
			}
			sai.angle += 0x07;
			shot_velocity_set(&shot->pos.velocity, sai.angle);
		} else {
			switch(sai.i - 4u) {
			case 0: OPT_L;	MISSILE_L;	VELOCITY_XY(-1.375f, 2.25f);	break;
			case 1: OPT_R;	MISSILE_R;	VELOCITY_XY( 1.375f, 2.25f);	break;
			case 2: OPT_L;	MISSILE_S;	VELOCITY_XY(-1.0f, -3.0f);	break;
			case 3: OPT_R;	MISSILE_S;	VELOCITY_XY( 1.0f, -3.0f);	break;
			case 4:	OPT_L;	MISSILE_S;	VELOCITY_XY( 0.5f, -4.0f);	break;
			case 5:	OPT_R;	MISSILE_S;	VELOCITY_XY(-0.5f, -4.0f);	break;
			}
			shot->set_option_sprite_and_damage(11);
		}
		if(sai.next() <= 0) {
			break;
		}
	}
#undef STAR_COUNT
}

static void shot_marisa_l6(void)
{
#define STAR_COUNT 4
	shot_func_init(shot, sai, cycle, STAR_COUNT, SC_3X, SC_1X, i += 8);
	int star_distance = star_offset(STAR_COUNT);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= STAR_COUNT) {
			marisa_stars(shot, 5);
		} else {
			switch(sai.i - 5u) {
			case 0:	OPT_L;	MISSILE_L;	VELOCITY_XY(-1.375f, 2.25f);	break;
			case 1:	OPT_R;	MISSILE_R;	VELOCITY_XY( 1.375f, 2.25f);	break;
			case 2:	OPT_L;	MISSILE_S;	VELOCITY_XY(-1.0f, -3.0f);	break;
			case 3:	OPT_R;	MISSILE_S;	VELOCITY_XY( 1.0f, -3.0f);	break;
			case 4:	OPT_L;	MISSILE_S;	VELOCITY_Y(-5.0f);	break;
			case 5:	OPT_R;	MISSILE_S;	VELOCITY_Y(-5.0f);	break;
			case 6:	OPT_L;	MISSILE_S;	VELOCITY_Y(-2.0f);	break;
			case 7:	OPT_R;	MISSILE_S;	VELOCITY_Y(-2.0f);	break;
			}
			shot->set_option_sprite_and_damage(11);
		}
		if(sai.next() <= 0) {
			break;
		}
	}
#undef STAR_COUNT
}

#define MARISA_L7(star_count, star_damage, missile_count, missile_damage) \
	shot_func_init( \
		shot, sai, cycle, star_count, SC_3X, SC_1X, i += missile_count \
	); \
	int star_distance = star_offset(star_count); \
	while(( shot = shots_add() ) != nullptr) { \
		if(sai.i <= star_count) { \
			marisa_stars(shot, star_damage); \
		} else { \
			switch(sai.i - 5u) { \
			case 0:	OPT_L;	MISSILE_L;	VELOCITY_XY(-1.75f, 2.25f);	break; \
			case 1:	OPT_R;	MISSILE_R;	VELOCITY_XY( 1.75f, 2.25f);	break; \
			case 2:	OPT_L;	MISSILE_R;	VELOCITY_XY( 2.0f,  2.0f);	break; \
			case 3:	OPT_R;	MISSILE_L;	VELOCITY_XY(-2.0f,  2.0f);	break; \
			case 4:	OPT_L;	MISSILE_S;	VELOCITY_YX( 3.0f, -1.0f);	break; \
			case 5:	OPT_R;	MISSILE_S;	VELOCITY_YX( 3.0f,  1.0f);	break; \
			case 6:	OPT_L;	MISSILE_S;	VELOCITY_Y(-5.0f);	break; \
			case 7:	OPT_R;	MISSILE_S;	VELOCITY_Y(-5.0f);	break; \
			case 8:	OPT_L;	MISSILE_S;	VELOCITY_Y(-2.0f);	break; \
			case 9:	OPT_R;	MISSILE_S;	VELOCITY_Y(-2.0f);	break; \
			} \
			shot->set_option_sprite_and_damage(missile_damage); \
		} \
		if(sai.next() <= 0) { \
			break; \
		} \
	}

static void shot_marisa_l7(void)
{
	MARISA_L7(4, 5, 10, 11);
}

static void shot_marisa_l8(void)
{
	MARISA_L7(4, 4, 10, 10);
}

static void shot_marisa_l9(void)
{
#define STAR_COUNT 6

	shot_func_init(shot, sai, cycle, STAR_COUNT, SC_3X, SC_1X, i += 12);
	int star_distance = star_offset(STAR_COUNT);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= STAR_COUNT) {
			marisa_stars(shot, 4);
		} else {
			switch(sai.i - 7u) {
			case  0:	OPT_L;	MISSILE_L;	VELOCITY_XY(-2.0f, 2.25f);	break;
			case  1:	OPT_R;	MISSILE_R;	VELOCITY_XY( 2.0f, 2.25f);	break;
			case  2:	OPT_L;	MISSILE_R;	VELOCITY_XY( 2.0f, 2.0f);	break;
			case  3:	OPT_R;	MISSILE_L;	VELOCITY_XY(-2.0f, 2.0f);	break;
			case  4:	OPT_L;	MISSILE_S;	VELOCITY_Y(3.0f);	break;
			case  5:	OPT_R;	MISSILE_S;	VELOCITY_Y(3.0f);	break;
			case  6:	OPT_L;	MISSILE_S;	VELOCITY_YX( 0.0f, -1.0f);	break;
			case  7:	OPT_R;	MISSILE_S;	VELOCITY_YX( 0.0f,  1.0f);	break;
			case  8:	OPT_L;	MISSILE_S;	VELOCITY_Y(-8.0f);	break;
			case  9:	OPT_R;	MISSILE_S;	VELOCITY_Y(-8.0f);	break;
			case 10:	OPT_L;	MISSILE_S;	VELOCITY_Y(-4.0f);	break;
			case 11:	OPT_R;	MISSILE_S;	VELOCITY_Y(-4.0f);	break;
			case 12:	OPT_L;	MISSILE_S;	VELOCITY_Y(-1.0f);	break;
			case 13:	OPT_R;	MISSILE_S;	VELOCITY_Y(-1.0f);	break;
			}
			shot->set_option_sprite_and_damage(9);
		}
		if(sai.next() <= 0) {
			break;
		}
	}
#undef STAR_COUNT
}


/* ReC98
 * -----
 * Mima's shot control functions
 */










static void shot_mima_l2(void)
{
	shot_func_init(shot, sai, cycle, 2, SC_6X, SC_3X, i++);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 2) {
			if(sai.i == 2) { shot->from_option_l(); }
			else/*i == 1*/ { shot->from_option_r(); }
			shot->pos.velocity.y.set(-20.0f);
			shot->set_option_sprite_and_damage(5);
		} else {
			shot->set_random_angle_forwards();
			shot->damage = 8;
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_mima_l3(void)
{
	shot_func_init(shot, sai, cycle, 2, SC_6X, SC_3X, i += 2);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 2) {
			if(sai.i == 2) { shot->from_option_l(); }
			else/*i == 1*/ { shot->from_option_r(); }
			shot->pos.velocity.y.set(-20.0f);
			shot->set_option_sprite_and_damage(5);
		} else {
			if(sai.i == 4) { shot->pos.cur.x -= 8.0f; }
			else/*i == 3*/ { shot->pos.cur.x += 8.0f; }
			shot->pos.velocity.y.set(-12.0f);
			shot->damage = 7;
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_mima_l4(void)
{
	shot_func_init(shot, sai, cycle, 4, SC_6X, SC_3X, i += 2);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 4) {
			switch(sai.i - 1) {
			// Beware, non-sequential case order!
			case 1:	VELOCITY_X(-1.0f);	shot->from_option_l();	break;
			case 0:	VELOCITY_X( 1.0f);	shot->from_option_r();	break;
			case 3:	                  	shot->from_option_l();	break;
			case 2:	                  	shot->from_option_r();	break;
			}
			shot->pos.velocity.y.set(-20.0f);
			shot->set_option_sprite_and_damage(4);
		} else {
			if(sai.i == 6) { shot->pos.cur.x -= 8.0f; }
			else/* i== 5*/ { shot->pos.cur.x += 8.0f; }
			shot->pos.velocity.y.set(-12.0f);
			shot->damage = 7;
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_mima_l5(void)
{
	shot_func_init(shot, sai, cycle, 4, SC_6X, SC_3X, i += 3);
	sai.angle = -0x46;
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 4) {
			switch(sai.i - 1) {
			// Beware, non-sequential case order!
			case 0:	VELOCITY_X( 1.0f);	shot->from_option_l();	break;
			case 3:	                  	shot->from_option_l();	break;
			case 1:	VELOCITY_X(-1.0f);	shot->from_option_r();	break;
			case 2:	                  	shot->from_option_r();	break;
			}
			shot->pos.velocity.y.set(-20.0f);
			shot->set_option_sprite_and_damage(4);
		} else {
			shot_velocity_set(&shot->pos.velocity, sai.angle);
			sai.angle += 0x06;
			shot->damage = 7;
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_mima_l6(void)
{
	shot_func_init(shot, sai, cycle, 4, SC_6X, SC_3X, i += 3);
	sai.angle = -0x46;
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 4) {
			switch(sai.i - 1) {
			case 0:	VELOCITY_X( 1.0f);	shot->from_option_r();	break;
			case 1:	VELOCITY_X(-1.0f);	shot->from_option_l();	break;
			case 2:	VELOCITY_X(-0.5f);	shot->from_option_r();	break;
			case 3:	VELOCITY_X( 0.5f);	shot->from_option_l();	break;
			}
			shot->pos.velocity.y.set(-20.0f);
			shot->set_option_sprite_and_damage(4);
		} else {
			shot_velocity_set(&shot->pos.velocity, sai.angle);
			sai.angle += 0x06;
			shot->damage = 7;
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_mima_l7(void)
{
	shot_func_init(shot, sai, cycle, 6, SC_6X, SC_3X, i += 3);
	sai.angle = -0x46;
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 6) {
			switch(sai.i - 1) {
			// Beware, non-sequential case order!
			case 0:	VELOCITY_X( 1.0f);	shot->from_option_r(4.0f);	break;
			case 1:	VELOCITY_X(-1.0f);	shot->from_option_l(4.0f);	break;
			case 2:	VELOCITY_X(-0.5f);	shot->from_option_r();	break;
			case 4:	                 	shot->from_option_r();	break;
			case 3:	VELOCITY_X( 0.5f);	shot->from_option_l();	break;
			case 5:	                	shot->from_option_l();	break;
			}
			shot->pos.velocity.y.set(-20.0f);
			shot->set_option_sprite_and_damage(4);
		} else {
			shot_velocity_set(&shot->pos.velocity, sai.angle);
			sai.angle += 0x06;
			shot->damage = 7;
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_mima_l8(void)
{
	shot_func_init(shot, sai, cycle, 6, SC_6X, SC_3X, i += 3);
	sai.angle = -0x46;
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 6) {
			shot->pos.velocity.y.set(-20.0f);
			switch(sai.i - 1) {
			// Beware, non-sequential case order!
			case 0:	VELOCITY_XY( 1.75f, -18.0f); shot->from_option_r(8.0f);	break;
			case 1:	VELOCITY_XY(-1.75f, -18.0f); shot->from_option_l(8.0f);	break;
			case 2:	VELOCITY_X (-0.5f);          shot->from_option_r();	break;
			case 4:	                             shot->from_option_r();	break;
			case 3:	VELOCITY_X ( 0.5f);          shot->from_option_l();	break;
			case 5:                              shot->from_option_l();	break;
			}
			shot->pos.velocity.y.set(-20.0f);
			shot->set_option_sprite_and_damage(4);
		} else {
			shot_velocity_set(&shot->pos.velocity, sai.angle);
			sai.angle += 0x06;
			shot->damage = 7;
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_mima_l9(void)
{
	shot_func_init(shot, sai, cycle, 6, SC_6X, SC_3X, i += 4);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 6) {
			shot->pos.velocity.y.set(-20.0f);
			switch(sai.i - 1) {
			case 0:	VELOCITY_XY( 2.625f, -18.0f); shot->from_option_r(8.0f); break;
			case 1:	VELOCITY_XY(-2.625f, -18.0f); shot->from_option_l(8.0f); break;
			case 2:	VELOCITY_X (-0.5f);       	shot->from_option_r();	break;
			case 3:	VELOCITY_X ( 0.5f);       	shot->from_option_l();	break;
			case 4:	VELOCITY_X ( 0.5f);       	shot->from_option_r();	break;
			case 5:	VELOCITY_X (-0.5f);       	shot->from_option_l();	break;
			}
			shot->set_option_sprite_and_damage(4);
		} else {
			switch(sai.i - 7u) {
			case 0:	sai.angle = -0x48;	                        	break;
			case 1:	sai.angle = -0x40;	shot->pos.cur.x -= 8.0f;	break;
			case 2:	sai.angle = -0x40;	shot->pos.cur.x += 8.0f;	break;
			case 3:	sai.angle = -0x38;	                         	break;
			}
			shot_velocity_set(&shot->pos.velocity, sai.angle);
			shot->damage = 7;
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}


/* ReC98
 * -----
 * Yuuka's shot control functions
 */










#define YUUKA_INNER_L4 \
	switch(sai.i - 1) { \
	case 0:	sai.angle = -0x46;	break; \
	case 1:	sai.angle = -0x40;	shot->pos.cur.x -= 8;	break; \
	case 2:	sai.angle = -0x40;	shot->pos.cur.x += 8;	break; \
	case 3:	sai.angle = -0x3A;	break; \
	} \
	shot->damage = 7;

#define YUUKA_INNER_L5 \
	switch(sai.i - 1) { \
	case 0:	sai.angle = -0x4C; break; \
	case 1:	sai.angle = -0x46; break; \
	case 2:	sai.angle = -0x40; break; \
	case 3:	sai.angle = -0x3A; break; \
	case 4:	sai.angle = -0x34; break; \
	} \
	shot->damage = 7;

static void shot_yuuka_l2(void)
{
	shot_func_init(shot, sai, cycle, 1, SC_3X, SC_1X, i += 2);
	while(( shot = shots_add() ) != nullptr) {
		if(sai.i == 1) {
			sai.set_random_angle(-0x48, -0x38);
			shot->damage = 9;
		} else {
			if(sai.i == 3) { shot->from_option_l(); sai.angle = -0x48; }
			else/*i == 2*/ { shot->from_option_r(); sai.angle = -0x38; }
			shot->set_option_sprite_and_damage(6);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_yuuka_l3(void)
{
	shot_func_init(shot, sai, cycle, 2, SC_3X, SC_2X, add_secondary(2));
	char option_only = cycle & SC_3X;

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 2) {
			if(sai.i == 2) { shot->pos.cur.x -= 8.0f; }
			else/*i == 1*/ { shot->pos.cur.x += 8.0f; }
			shot->damage = 8;
		} else {
			if(sai.i == 4) {
				shot->from_option_l();
				sai.angle = -0x48;
			} else {
				shot->from_option_r();
				sai.angle = -0x38;
				if(option_only == 0) {
					sai.i = 1;
				}
			}
			shot->set_option_sprite_and_damage(6);
			shot_velocity_set(&shot->pos.velocity, sai.angle);
		}
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_yuuka_l4(void)
{
	shot_func_init(shot, sai, cycle, 3, SC_3X, SC_2X, add_secondary(2));
	char option_only = cycle & SC_3X;

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 3) {
			if(sai.i == 3) {
				sai.angle = -0x4C;
			}
			shot->damage = 7;
			sai.angle += 0x06;
		} else {
			if(sai.i == 5) {
				shot->from_option_l();
				sai.set_random_angle(-0x4C, -0x44);
			} else {
				shot->from_option_r();
				sai.set_random_angle(-0x3C, -0x34);
				if(option_only == 0) {
					sai.i = 1;
				}
			}
			shot->pos.velocity.y.set(2.0f);
			shot->set_option_sprite_and_damage(6);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_yuuka_l5(void)
{
	shot_func_init(shot, sai, cycle, 3, SC_3X, SC_2X, add_secondary(4));
	char option_only = cycle & SC_3X;

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 3) {
			shot->damage = 7;
			if(sai.i == 3) {
				sai.angle = -0x4C;
			}
			sai.angle += 0x06;
		} else {
			switch(sai.i - 4u) {
			case 0:
				if(option_only == 0) {
					sai.i = 1;
				}
			/*  */	shot->from_option_l();	sai.angle = -0x4E;	break;
			case 1:	shot->from_option_r();	sai.angle = -0x32;	break;
			case 2:	shot->from_option_l();	sai.angle = -0x46;	break;
			case 3:	shot->from_option_r();	sai.angle = -0x3A;	break;
			}
			shot->set_option_sprite_and_damage(6);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_yuuka_l6(void)
{
	shot_func_init(shot, sai, cycle, 4, SC_3X, SC_2X, add_secondary(4));
	char option_only = cycle & SC_3X;

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 4) {
			YUUKA_INNER_L4;
		} else {
			switch(sai.i - 5u) {
			case 0:	shot->from_option_l();	sai.angle = -0x50;
				if(option_only == 0) {
					sai.i = 1;
				}
				break;
			case 1:	shot->from_option_r();	sai.angle = -0x30;	break;
			case 2:	shot->from_option_l();	sai.angle = -0x48;	break;
			case 3:	shot->from_option_r();	sai.angle = -0x38;	break;
			}
			shot->set_option_sprite_and_damage(6);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_yuuka_l7(void)
{
	shot_func_init(shot, sai, cycle, 4, SC_3X, SC_2X, add_secondary(6));
	char option_only = cycle & SC_3X;

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 4) {
			YUUKA_INNER_L4;
		} else {
			switch(sai.i - 5u) {
			case 0:	shot->from_option_l();	sai.angle = -0x50;
				if(option_only == 0) {
					sai.i = 1;
				}
				break;
			case 1:	shot->from_option_r();	sai.angle = -0x30;	break;
			case 2:	shot->from_option_l();	sai.angle = -0x49;	break;
			case 3:	shot->from_option_r();	sai.angle = -0x37;	break;
			case 4:	shot->from_option_l();	sai.angle = -0x42;	break;
			case 5:	shot->from_option_r();	sai.angle = -0x3E;	break;
			}
			shot->set_option_sprite_and_damage(6);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_yuuka_l8(void)
{
	shot_func_init(shot, sai, cycle, 5, SC_3X, SC_2X, add_secondary(6));
	char option_only = cycle & SC_3X;

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 5) {
			YUUKA_INNER_L5;
		} else {
			switch(sai.i - 6u) {
			case 0:	shot->from_option_l();	sai.angle = -0x50;
				if(option_only == 0) {
					sai.i = 1;
				}
				break;
			case 1:	shot->from_option_r();	sai.angle = -0x30;	break;
			case 2:	shot->from_option_l();	sai.angle = -0x49;	break;
			case 3:	shot->from_option_r();	sai.angle = -0x37;	break;
			case 4:	shot->from_option_l();	sai.angle = -0x42;	break;
			case 5:	shot->from_option_r();	sai.angle = -0x3E;	break;
			}
			shot->set_option_sprite_and_damage(6);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}

static void shot_yuuka_l9(void)
{
	shot_func_init(shot, sai, cycle, 5, SC_3X, SC_2X, add_secondary(8));
	char option_only = cycle & SC_3X;

	while(( shot = shots_add() ) != nullptr) {
		if(sai.i <= 5) {
			YUUKA_INNER_L5;
		} else {
			switch(sai.i - 6u) {
			case 0:	shot->from_option_l();	sai.angle = -0x58;
				if(option_only == 0) {
					sai.i = 1;
				}
				break;
			case 1:	shot->from_option_r();	sai.angle = -0x28;	break;
			case 2:	shot->from_option_l();	sai.angle = -0x50;	break;
			case 3:	shot->from_option_r();	sai.angle = -0x30;	break;
			case 4:	shot->from_option_l();	sai.angle = -0x48;	break;
			case 5:	shot->from_option_r();	sai.angle = -0x38;	break;
			case 6:	shot->from_option_l();	sai.angle = -0x3F;	break;
			case 7:	shot->from_option_r();	sai.angle = -0x41;	break;
			}
			shot->set_option_sprite_and_damage(6);
		}
		shot_velocity_set(&shot->pos.velocity, sai.angle);
		if(sai.next() <= 0) {
			break;
		}
	}
}


static void Run(int character,int power){typedef void(*Fn)();static Fn table[4][10]={{shot_l0,shot_l1,shot_reimu_l2,shot_reimu_l3,shot_reimu_l4,shot_reimu_l5,shot_reimu_l6,shot_reimu_l7,shot_reimu_l8,shot_reimu_l9},{shot_l0,shot_l1,shot_marisa_l2,shot_marisa_l3,shot_marisa_l4,shot_marisa_l5,shot_marisa_l6,shot_marisa_l7,shot_marisa_l8,shot_marisa_l9},{shot_l0,shot_l1,shot_mima_l2,shot_mima_l3,shot_mima_l4,shot_mima_l5,shot_mima_l6,shot_mima_l7,shot_mima_l8,shot_mima_l9},{shot_l0,shot_l1,shot_yuuka_l2,shot_yuuka_l3,shot_yuuka_l4,shot_yuuka_l5,shot_yuuka_l6,shot_yuuka_l7,shot_yuuka_l8,shot_yuuka_l9},};table[character][power]();}
} } }

#undef MARISA_L7
#undef MISSILE_L
#undef MISSILE_R
#undef MISSILE_S
#undef OPT_L
#undef OPT_R
#undef REIMU_L5
#undef SC_1X
#undef SC_2X
#undef SC_3X
#undef SC_6X
#undef STAR_COUNT
#undef ST_HOMING
#undef ST_MISSILE_LEFT
#undef ST_MISSILE_RIGHT
#undef ST_MISSILE_STRAIGHT
#undef VELOCITY_X
#undef VELOCITY_XY
#undef VELOCITY_Y
#undef VELOCITY_YX
#undef YUUKA_INNER_L4
#undef YUUKA_INNER_L5
#undef marisa_stars
#undef reimu_forward
#undef reimu_sub_spread_constant
#undef reimu_sub_spread_random
#undef set_homing
#undef shot_func_init
