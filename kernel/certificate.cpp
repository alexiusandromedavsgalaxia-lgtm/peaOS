#include "certificate.hpp"

namespace certificate {
namespace {
Certificate g_current{};
bool g_active=false;
bool zero(const uint8_t* p,uint32_t n){if(!p)return true;for(uint32_t i=0;i<n;i++)if(p[i])return false;return true;}
}
void init(){g_current={};g_active=false;}
bool expired(const Certificate* c,uint64_t now){if(!c||c->magic!=kMagic||c->version!=kVersion||now==0)return true;return c->expires_at<=c->issued_at||now<c->issued_at||now>=c->expires_at||c->expires_at-c->issued_at>kValiditySeconds;}
Status validate(const Certificate* c,uint64_t now){if(!c||c->magic!=kMagic||c->version!=kVersion||c->type!=static_cast<uint8_t>(CertificateType::Signing)||c->serial==0)return Status::Invalid;if(zero(c->certificate_id,16)||zero(c->issuer_key_id,16)||zero(c->public_key,kEd25519PublicKeySize)||zero(c->payload_sha512,kSha512Size)||zero(c->signature,kEd25519SignatureSize))return Status::Invalid;if(expired(c,now))return Status::Expired;return Status::Valid;}
Status validate_server(const Certificate* c,const ServerValidation& s){if(!c||c->magic!=kMagic||c->version!=kVersion)return Status::Invalid;if(!s.reachable)return Status::ServerUnavailable;if(s.revoked||!s.active)return Status::NotActiveOnServer;if(!s.signature_valid)return Status::SignatureInvalid;if(!s.issuer_valid)return Status::IssuerMismatch;if(!s.digest_valid)return Status::DigestMismatch;return Status::Valid;}
bool install(const Certificate* c,uint64_t now){if(validate(c,now)!=Status::Valid)return false;g_current=*c;g_active=true;return true;}
bool active(){return g_active;}
void invalidate(){g_current={};g_active=false;}
const Certificate* current(){return g_active?&g_current:nullptr;}
}
