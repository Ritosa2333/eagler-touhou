#include "../../src/game/BulletPool.hpp"
th12::bullet_pool::Slots pool;
extern "C" {
void pool_reset(int cursor,int busy){pool.reset();pool.cursor=cursor;pool.phases.fill(busy);}
void pool_free(int slot){pool.release(slot);}
int pool_reserve(){return pool.reserve();}
void pool_commit(int slot){pool.commit(slot);}
int pool_cursor(){return pool.cursor;}
}
