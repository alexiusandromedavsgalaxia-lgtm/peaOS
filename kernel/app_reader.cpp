#include "app_reader.hpp"

namespace app_reader {
namespace { Entry g_entries[kMaxEntries]{}; uint32_t g_count=0; char g_names[kMaxEntries][48]{};
bool eq(const char*a,const char*b){if(!a||!b)return false;while(*a&&*b&&*a==*b){++a;++b;}return *a==*b;}
bool starts(const char*a,const char*b){if(!a||!b)return false;while(*b){if(*a++!=*b++)return false;}return true;}
void copy_name(char*d,const char*s){uint32_t i=0;while(s&&s[i]&&s[i]!='\r'&&s[i]!='\n'&&i<47){d[i]=s[i];++i;}d[i]=0;}
const char* trim(const char*p){while(*p==' '||*p=='\t')++p;return p;}
}
void init(){for(auto&e:g_entries)e={};for(auto&n:g_names)n[0]=0;g_count=0;}
uint32_t parse(const char*manifest,uint32_t length){init();if(!manifest)return 0;uint32_t pos=0;while(pos<length&&g_count<kMaxEntries){uint32_t end=pos;while(end<length&&manifest[end]!='\n')++end;const char*p=trim(manifest+pos);if(*p&&*p!='#'&&*p!='['){copy_name(g_names[g_count],p);if(g_names[g_count][0]){g_entries[g_count]={g_names[g_count],app_registry::Runtime::Native,true,true};++g_count;}}pos=end+1;}return g_count;}
const Entry* entries(uint32_t*count){if(count)*count=g_count;return g_entries;}
const Entry* find(const char*name){for(uint32_t i=0;i<g_count;++i)if(eq(g_entries[i].name,name))return &g_entries[i];return nullptr;}
const char* extension_for(const char*path){if(!path)return "";const char*dot=path;for(const char*p=path;*p;++p)if(*p=='.')dot=p;return *dot=='.'?dot:"";}
bool is_native_package(const char*path){const char*e=extension_for(path);return eq(e,".pea")||eq(e,".xpea")||eq(e,".peac");}
bool is_supported_package(const char*path){const char*e=extension_for(path);return is_native_package(path)||eq(e,".exe")||eq(e,".dll")||eq(e,".apk")||eq(e,".aab");}
}
