#include "Scores.hpp"
#include <cstdlib>
#include <new>
using namespace th10;
#define SCORE_EXPORT(name) extern "C" __attribute__((export_name(name)))
SCORE_EXPORT("scores_create") browser::Scores* scores_create(browser::FileSystem* files,Rng* random,u32 chinese){auto* bytes=std::malloc(sizeof(browser::Scores));return bytes?new(bytes)browser::Scores(*files,*random,chinese!=0):nullptr;}
SCORE_EXPORT("scores_destroy") void scores_destroy(browser::Scores* scores){if(scores){scores->~Scores();std::free(scores);}}
SCORE_EXPORT("scores_data") ScoreData* scores_data(browser::Scores* scores){return scores->data;}
SCORE_EXPORT("scores_save") i32 scores_save(browser::Scores* scores){return scores->save();}
SCORE_EXPORT("scores_reload") void scores_reload(browser::Scores* scores){scores->reload();}
SCORE_EXPORT("scores_allocations") u32 scores_allocations(browser::Scores* scores){return scores->memory.count;}
