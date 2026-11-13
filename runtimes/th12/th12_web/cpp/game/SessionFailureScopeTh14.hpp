#pragma once
#include "PortTitle.hpp"
#if PORT_TITLE == 14
#include <string>
namespace th11 {
// An initial writer reports into its own error string. Main may fail its
// renderer independently, so GameSession::error cannot be shared scratch.
class SessionFailureScopeTh14 {
    const void* owner;std::string& error;SessionFailureScopeTh14* previous;
#if defined(TH14_CPP_STAGE_WORKER) && defined(_REENTRANT)
    static inline thread_local SessionFailureScopeTh14* active=nullptr;
#else
    static inline SessionFailureScopeTh14* active=nullptr;
#endif
public:
    SessionFailureScopeTh14(const void* session,std::string& output):owner(session),error(output),previous(active){active=this;}
    ~SessionFailureScopeTh14(){active=previous;}
    static bool report(const void* session,const char* message){
        for(auto* scope=active;scope;scope=scope->previous)if(scope->owner==session){scope->error=message?message:"session error";return true;}
        return false;
    }
    SessionFailureScopeTh14(const SessionFailureScopeTh14&)=delete;
};
}
#endif
