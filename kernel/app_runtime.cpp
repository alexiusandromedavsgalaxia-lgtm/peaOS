#include "app_runtime.hpp"
namespace app_runtime { namespace { Process g_processes[kMaxProcesses]{};uint32_t g_count=0,g_next_pid=1;
Process* find(app_registry::AppId id){for(auto&p:g_processes)if(p.state!=State::Empty&&p.app==id)return &p;return nullptr;}
}
void init(){for(auto&p:g_processes)p={};g_count=0;g_next_pid=1;}
Result start(app_registry::AppId id,bool network_available){const auto*a=app_registry::find(id);if(!a||!a->bundled)return Result::NotInstalled;if(find(id))return Result::AlreadyRunning;if(a->requires_network&&!network_available)return Result::NetworkRequired;if(a->runtime!=app_registry::Runtime::Native&&a->runtime!=app_registry::Runtime::X90Fusion&&a->runtime!=app_registry::Runtime::WebSandbox)return Result::UnsupportedRuntime;if(g_count>=kMaxProcesses)return Result::LimitReached;for(auto&p:g_processes)if(p.state==State::Empty){p={g_next_pid++,id,a->runtime,State::Starting,a->isolated?180:128,a->isolated?65536u:32768u,a->isolated,a->requires_network,0};p.state=State::Running;++g_count;return Result::Ok;}return Result::LimitReached;}
Result stop(app_registry::AppId id){Process*p=find(id);if(!p)return Result::NotInstalled;p->state=State::Stopping;p->state=State::Empty;if(g_count)--g_count;return Result::Ok;}
Result suspend(app_registry::AppId id){Process*p=find(id);if(!p)return Result::NotInstalled;if(p->state!=State::Running)return Result::InvalidApp;p->state=State::Suspended;return Result::Ok;}
Result resume(app_registry::AppId id){Process*p=find(id);if(!p)return Result::NotInstalled;if(p->state!=State::Suspended)return Result::InvalidApp;p->state=State::Running;return Result::Ok;}
void tick(uint64_t ticks){for(auto&p:g_processes)if(p.state==State::Running)p.ticks+=ticks;}
const Process* processes(uint32_t*count){if(count)*count=g_count;return g_processes;}
const Process* process_for(app_registry::AppId id){return find(id);}
uint32_t process_count(){return g_count;}
const char* runtime_name(app_registry::Runtime r){switch(r){case app_registry::Runtime::Native:return "pea native";case app_registry::Runtime::X90Fusion:return "X90 Fusion";case app_registry::Runtime::WebSandbox:return "Web Sandbox";default:return "unknown";}}
}
