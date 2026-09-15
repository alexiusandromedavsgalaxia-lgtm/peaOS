#include "system_provision.hpp"
#include "sha512.hpp"
#include "certificate.hpp"

namespace systemprovision {
namespace {
constexpr char kEndpointPrefix[]="https://raw.githubusercontent.com/alexiusandromedavsgalaxia-lgtm/peaOS.Officialy-certificates/main/certificates/";
constexpr char kEndpointSuffix[]=".json";
uint16_t append(char* out,uint16_t pos,uint16_t size,const char* text){while(*text){if(pos+1>=size)return 0;out[pos++]=*text++;}out[pos]=0;return pos;}
char hex(uint8_t n){return n<10?char('0'+n):char('a'+n-10);}
bool id_hex(const uint8_t* id,char* out,uint16_t size){if(size<33)return false;for(uint8_t i=0;i<16;i++){out[i*2]=hex(id[i]>>4);out[i*2+1]=hex(id[i]&15);}out[32]=0;return true;}
}
void init(){}
bool constant_time_equal(const uint8_t* a,const uint8_t* b,uint64_t length){if(!a||!b)return false;uint8_t diff=0;for(uint64_t i=0;i<length;i++)diff|=a[i]^b[i];return diff==0;}
bool build_github_endpoint(const uint8_t certificate_id[kCertificateIdSize],char* out,uint16_t out_size){if(!certificate_id||!out||out_size<80)return false;char id[33];if(!id_hex(certificate_id,id,sizeof(id)))return false;uint16_t p=append(out,0,out_size,kEndpointPrefix);if(!p)return false;p=append(out,p,out_size,id);if(!p)return false;return append(out,p,out_size,kEndpointSuffix)!=0;}
Result authenticate_endpoint(const uint8_t certificate_id[kCertificateIdSize],const uint8_t* payload,uint64_t payload_length,const R35Record* record,uint64_t now){if(!certificate_id||!payload||payload_length==0||payload_length>kMaxPayloadLength||!record)return Result::InvalidInput;if(!constant_time_equal(certificate_id,record->certificate_id,kCertificateIdSize))return Result::CertificateMismatch;if(record->revoked)return Result::Revoked;if(now<record->issued_at||now>=record->expires_at)return Result::Expired;uint8_t digest[sha512::kDigestSize];sha512::hash(payload,payload_length,digest);if(!constant_time_equal(digest,record->payload_sha512,kR35DigestSize))return Result::DigestMismatch;return Result::Ok;}
}
