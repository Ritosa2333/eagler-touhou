int stageIdForEcl(const std::string& name) {
    for(int id=1;id<=16;++id)if(name==kStageEclNames[std::size_t(id)])return id;
    return 0;
}
