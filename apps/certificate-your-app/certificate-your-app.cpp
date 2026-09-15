#include "certificate-your-app.hpp"

namespace certificate_app {

static bool ends_with(const char* value,const char* suffix){if(!value||!suffix)return false;const char*a=value;const char*b=suffix;while(*a)++a;while(*b)++b;while(a>value&&b>suffix&&*(a-1)==*(b-1)){--a;--b;}return b==suffix;}

bool supports_native_package(const char* extension){return ends_with(extension,".pea")||ends_with(extension,".xpea")||ends_with(extension,".peac");}

RenewalResult renew_manually(const char* certificate_id,uint64_t now){RenewalResult result{};result.status=certificate::Status::ServerUnavailable;if(!certificate_id||!*certificate_id||now==0)return result;return result;}

RenewalResult apply_server_validation(const certificate::Certificate* cert,const certificate::ServerValidation& server,const uint8_t* expected_origin_hash,const uint8_t* package_hash){
    RenewalResult result{};
    result.status=certificate::validate_server(cert,server,expected_origin_hash,package_hash);
    if(result.status==certificate::Status::Valid){result.success=true;result.applications_bound=cert->application_count;result.expires_at=cert->expires_at;certificate::install(cert);}else{certificate::invalidate();}
    return result;
}

void remove_invalid_certificate(){certificate::invalidate();}

}
