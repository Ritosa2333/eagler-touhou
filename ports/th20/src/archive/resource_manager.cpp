#include "resource_manager.hpp"
#include "../platform_services/services.hpp"
#include <stdexcept>
namespace th20::source::resources {
bool Manager::open(const std::filesystem::path& path) {
    close();error_.clear();
    try {archive_=std::make_unique<Archive>(path,dictionary_);path_=path;return true;}
    catch(const std::exception& error) {error_=error.what();close();return false;}
}
void Manager::close() noexcept {archive_.reset();path_.clear();}
std::size_t Manager::entry_count() const noexcept {return archive_?archive_->entries().size():0;}
std::uint32_t Manager::size(std::string_view name) const {
    if(!archive_)return 0;
    try {return archive_->entries().at(archive_->find(name)).size;}
    catch(const std::out_of_range&) {return 0;}
}
std::optional<Bytes> Manager::read(std::string_view name) {
    if(!archive_)return std::nullopt;
    try {return archive_->read(name);}
    catch(const std::out_of_range&) {return std::nullopt;}
    catch(const std::bad_alloc&) {return std::nullopt;}
}
Manager& manager() noexcept {static Manager value;return value;}
Bytes Manager::decode(const Bytes& compressed,std::uint32_t expected_size){return dictionary_.decode(compressed,expected_size);}
void Manager::with_dictionary(const std::function<void(std::array<std::uint8_t,8192>&)>& operation){dictionary_.with_dictionary(operation);}
void with_shared_dictionary(const std::function<void(std::array<std::uint8_t,8192>&)>& operation){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));manager().with_dictionary(operation);
}
Bytes decode_shared(const Bytes& compressed,std::uint32_t expected_size){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));return manager().decode(compressed,expected_size);
}
bool open(const std::filesystem::path& path) {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));
    return manager().open(path);
}
void close() noexcept {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));
    manager().close();
}
std::string_view archive_lookup_name(std::string_view name) noexcept {
    const auto backslash=name.rfind('\\');
    const auto remaining=backslash==std::string_view::npos?name:name.substr(backslash+1);
    const auto slash=remaining.rfind('/');
    // 0x410b2d restores the ORIGINAL pointer when '/' is absent, even after
    // finding '\\'. Preserve this specimen's observable mixed-separator quirk.
    return slash==std::string_view::npos?name:remaining.substr(slash+1);
}
std::optional<Bytes> read(const char* name,bool loose_only) {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));
    if(loose_only)return platform::read_loose_file(name);
    const auto member=archive_lookup_name(name);
    if(!manager().size(member))return std::nullopt;
    return manager().read(member);
}
}
