#include "game/StageResourceCatalog.hpp"

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {

using th128::stage::Catalog;
using th128::stage::StageResourceRecord;

std::string unmark(std::string& token) {
    return token == "~" ? std::string() : std::move(token);
}

void printField(const std::string& value) {
    if (value.empty()) {
        std::cout << " ~";
    } else {
        std::cout << ' ' << value;
    }
}

void printRecord(const StageResourceRecord& record) {
    std::cout << ' ' << record.id;
    printField(record.background);
    printField(record.ecl);
    printField(record.message);
    printField(record.logo);
    printField(record.music[0]);
    printField(record.music[1]);
}

bool readRecord(StageResourceRecord& record) {
    std::string background, ecl, message, logo, firstMusic, secondMusic;
    if (!(std::cin >> record.id >> background >> ecl >> message >> logo >> firstMusic >> secondMusic)) {
        return false;
    }
    record.background = unmark(background);
    record.ecl = unmark(ecl);
    record.message = unmark(message);
    record.logo = unmark(logo);
    record.music = {unmark(firstMusic), unmark(secondMusic)};
    return true;
}

}

int main() {
    std::ios::sync_with_stdio(false);
    Catalog catalog;
    std::vector<StageResourceRecord> pending;
    const StageResourceRecord* lastPointer = nullptr;
    std::string command;
    while (std::cin >> command) {
        if (command == "REC") {
            StageResourceRecord record;
            if (!readRecord(record)) {
                return 2;
            }
            pending.push_back(std::move(record));
        } else if (command == "DROP") {
            pending = std::vector<StageResourceRecord>{};
        } else if (command == "MUTATE") {
            for (StageResourceRecord& record : pending) {
                record.id = 0;
                record.background = "mutated.std";
                record.ecl = "mutated.ecl";
                record.message = "mutated.msg";
                record.logo = "mutated.anm";
                record.music = {"mutated_0.wav", "mutated_1.wav"};
            }
        } else if (command == "LOAD") {
            std::string error;
            const bool loaded = catalog.load(pending, error);
            std::cout << "load " << (loaded ? 1 : 0) << " size " << catalog.size()
                      << " error " << error.size() << '\n';
        } else if (command == "FIND") {
            int id = 0;
            if (!(std::cin >> id)) {
                return 2;
            }
            const StageResourceRecord* record = catalog.find(id);
            std::cout << "find " << id;
            if (record == nullptr) {
                std::cout << " none";
            } else {
                printRecord(*record);
            }
            std::cout << '\n';
        } else if (command == "SIZE") {
            std::cout << "size " << catalog.size() << '\n';
        } else if (command == "CLONE") {
            const Catalog clone = catalog;
            std::cout << "clone " << clone.size();
            if (const StageResourceRecord* first = clone.find(1)) {
                printRecord(*first);
            }
            std::cout << '\n';
        } else if (command == "PTR") {
            int id = 0;
            if (!(std::cin >> id)) {
                return 2;
            }
            const StageResourceRecord* record = catalog.find(id);
            std::cout << "ptr " << id << ' ';
            if (record == nullptr) {
                std::cout << "none";
            } else if (record == lastPointer) {
                std::cout << "stable";
            } else {
                std::cout << reinterpret_cast<std::uintptr_t>(record);
                lastPointer = record;
            }
            std::cout << '\n';
        } else {
            std::cout << "unknown " << command << '\n';
            return 2;
        }
    }
    return 0;
}
