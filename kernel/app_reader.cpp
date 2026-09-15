#include "app_reader.hpp"

namespace app_reader {
namespace {
Entry g_entries[kMaxEntries]{};
uint32_t g_count=0;
char g_names[kMaxEntries][48]{};
char g_values[kMaxEntries][128]{};

bool eq(const char*a,const char*b){if(!a||!b)return false;while(*a&&*b&&*a==*b){++a;++b;}return *a==*b;}
bool starts(const char*a,const char*b){if(!a||!b)return false;while(*b){if(*a++!=*b++)return false;}return true;}
const char* trim(const char*p){while(*p==' '||*p=='\t')++p;return p;}
void copy_text(char*d,const char*s,uint32_t n){uint32_t i=0;while(s&&s[i]&&s[i]!='\r'&&s[i]!='\n'&&i+1<n){d[i]=s[i];++i;}d[i]=0;}
const char* value(const char*manifest,uint32_t length,const char*key,char*out,uint32_t out_size){uint32_t pos=0;while(pos<length){uint32_t end=pos;while(end<length&&manifest[end]!='\n')++end;const char*p=trim(manifest+pos);uint32_t k=0;while(key[k]&&p[k]==key[k])++k;if(key[k]==0&&p[k]=='='){copy_text(out,trim(p+k+1),out_size);return out;}pos=end+1;}return nullptr;}
app_registry::Runtime runtime_from(const char*s){if(eq(s,"x90")||eq(s,"fusion")||eq(s,"x90-fusion"))return app_registry::Runtime::X90Fusion;if(eq(s,"web")||eq(s,"web-sandbox"))return app_registry::Runtime::WebSandbox;return app_registry::Runtime::Native;}
bool truthy(const char*s){return eq(s,"1")||eq(s,"true")||eq(s,"yes");}
}
void init(){for(auto&e:g_entries)e={};for(auto&n:g_names)n[0]=0;for(auto&v:g_values)v[0]=0;g_count=0;}
uint32_t parse(const char*manifest,uint32_t length){init();if(!manifest||!length)return 0;uint32_t pos=0;while(pos<length&&g_count<kMaxEntries){uint32_t end=pos;while(end<length&&manifest[end]!='\n')++end;const char*p=trim(manifest+pos);if(*p&&*p!='#'&&*p!='['){uint32_t eqpos=0;while(p[eqpos]&&p[eqpos]!='='&&p[eqpos]!='\r'&&p[eqpos]!='\n')++eqpos;if(p[eqpos]=='='){copy_text(g_names[g_count],p,48);g_names[g_count][eqpos]=0;copy_text(g_values[g_count],trim(p+eqpos+1),128);const char*ext=extension_for(g_values[g_count]);g_entries[g_count]={g_names[g_count],runtime_from(g_values[g_count]),is_native_package(g_values[g_count]),is_supported_package(g_values[g_count])};++g_count;}}pos=end+1;}return g_count;}
bool read_manifest(const char*manifest,uint32_t length,Manifest*out){if(!out||!manifest||!length||length>kMaxManifestText)return false;char name[128]{},version[128]{},package[128]{},runtime[128]{},entry[128]{},network[128]{},isolated[128]{};if(!value(manifest,length,"name",name,sizeof(name)))return false;value(manifest,length,"version",version,sizeof(version));value(manifest,length,"package",package,sizeof(package));value(manifest,length,"runtime",runtime,sizeof(runtime));value(manifest,length,"entry",entry,sizeof(entry));value(manifest,length,"network",network,sizeof(network));value(manifest,length,"isolated",isolated,sizeof(isolated));static Manifest m{};static char n[128],v[128],p[128],r[128],e[128];copy_text(n,name,sizeof(n));copy_text(v,version,sizeof(v));copy_text(p,package,sizeof(p));copy_text(r,runtime,sizeof(r));copy_text(e,entry,sizeof(e));m={n,v,p,r,e,truthy(network),truthy(isolated)};*out=m;return true;}
const Entry* entries(uint32_t*count){if(count)*count=g_count;return g_entries;}
const Entry* find(const char*name){for(uint32_t i=0;i<g_count;++i)if(eq(g_entries[i].name,name))return &g_entries[i];return nullptr;}
const char* extension_for(const char*path){if(!path)return "";const char*dot=path;for(const char*p=path;*p;++p)if(*p=='.')dot=p;return *dot=='.'?dot:"";}
bool is_native_package(const char*path){const char*e=extension_for(path);return eq(e,".pea")||eq(e,".xpea")||eq(e,".peac");}
bool is_supported_package(const char*path){const char*e=extension_for(path);return is_native_package(path)||eq(e,".exe")||eq(e,".dll")||eq(e,".apk")||eq(e,".aab");}
bool validate_package(const char*path,const char*manifest,uint32_t length){if(!is_supported_package(path)||!manifest||!length)return false;Manifest m{};if(!read_manifest(manifest,length,&m))return false;if(!m.package||!m.package[0])return true;return eq(extension_for(path),extension_for(m.package));}
}
