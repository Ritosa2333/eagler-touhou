#pragma once
#include "../../cpp/game/ScoreFile.hpp"
#include "../../cpp/game/RecordStore.hpp"
struct ScoreFileFixture {ScoreFile file;Rng random;std::vector<u8> output;};
API(score_file_create) ScoreFileFixture* score_file_create(){return new ScoreFileFixture;}
API(score_file_delete) void score_file_delete(ScoreFileFixture* p){delete p;}
API(score_file_reset) void score_file_reset(ScoreFileFixture* p,u32 seed,u32 calls){std::memcpy(&p->random,&seed,4);p->random.calls=calls;p->file.reset(p->random);}
API(score_file_field) void* score_file_field(ScoreFileFixture* p,u32 index){return index==0?static_cast<void*>(p->file.characters.data()):index==1?static_cast<void*>(p->file.settings.data()):static_cast<void*>(&p->random);}
API(score_file_save) u32 score_file_save(ScoreFileFixture* p){return p->file.save(p->output);}
API(score_file_open) u32 score_file_open(ScoreFileFixture* p,const u8* data,u32 size){return p->file.open(data,size);}
API(score_file_data) const u8* score_file_data(ScoreFileFixture* p){return p->output.data();}
API(score_file_size) u32 score_file_size(ScoreFileFixture* p){return p->output.size();}
API(score_file_error) const char* score_file_error(ScoreFileFixture* p){return p->file.error.c_str();}
struct RecordStoreFixture {RecordStore records;Rng random;std::vector<u8> bytes;};
API(record_store_create) RecordStoreFixture* record_store_create(){return new RecordStoreFixture;}
API(record_store_delete) void record_store_delete(RecordStoreFixture* p){delete p;}
API(record_store_import) u32 record_store_import(RecordStoreFixture* p,const u8* characters,const u8* settings){return p->records.import_blocks(characters,settings);}
API(record_store_export) const u8* record_store_export(RecordStoreFixture* p,u32 settings){const auto& f=p->records.export_blocks();return settings?f.settings.data():f.characters.data()->data();}
API(record_store_unlock) u32 record_store_unlock(RecordStoreFixture* p){return p->records.unlock_all();}
