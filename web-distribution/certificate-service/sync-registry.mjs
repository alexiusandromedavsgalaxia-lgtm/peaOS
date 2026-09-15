import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';

const output=process.argv[2]||'build/generated/certificate_registry.hpp';
const manifestUrl='https://raw.githubusercontent.com/alexiusandromedavsgalaxia-lgtm/peaOS/main/peacloud.json';
const registryUrl='https://raw.githubusercontent.com/alexiusandromedavsgalaxia-lgtm/peaOS.Officialy-certificates/main/registry.json';
function cppString(value){return JSON.stringify(String(value??''));}
function fail(message){console.error(`certificate registry sync: ${message}`);process.exit(1);}
const [manifestResponse,registryResponse]=await Promise.all([fetch(`${manifestUrl}?_=${Date.now()}`,{cache:'no-store'}),fetch(`${registryUrl}?_=${Date.now()}`,{cache:'no-store'})]);
if(!manifestResponse.ok)fail(`peaOS manifest fetch failed: HTTP ${manifestResponse.status}`);
if(!registryResponse.ok)fail(`certificate registry fetch failed: HTTP ${registryResponse.status}`);
let manifest,registry;try{manifest=await manifestResponse.json();registry=await registryResponse.json();}catch(error){fail(`remote JSON is invalid: ${error.message}`);}
if(manifest.product!=='peaOS')fail('manifest product is not peaOS');
if(manifest.certificate_type!=='SigningCertificate'||manifest.certificate_protocol!=='R35-SHA512')fail('manifest certificate protocol mismatch');
if(registry.issuer!=='peaOS.Officialy-certificates'||registry.peaOS_certificate_version!==3||registry.certificate_type!=='SigningCertificate'||registry.authentication!=='R35-SHA512')fail('registry signing protocol mismatch');
if(manifest.certificate_version!==registry.peaOS_certificate_version)fail('certificate version mismatch');
if(!Array.isArray(registry.certificates))fail('registry.certificates is not an array');
for(const [index,cert] of registry.certificates.entries()){if(!cert||typeof cert!=='object')fail(`certificate ${index} is not an object`);if(typeof cert.id!=='string'||!/^[a-f0-9]{32}$/.test(cert.id))fail(`certificate ${index} has invalid id`);if(cert.type!=='SigningCertificate')fail(`certificate ${cert.id} has invalid type`);if(typeof cert.status!=='string'||!cert.status)fail(`certificate ${cert.id} has no status`);if(typeof cert.sha512!=='string'||!/^[a-f0-9]{128}$/.test(cert.sha512))fail(`certificate ${cert.id} has invalid SHA-512 fingerprint`);}
const canonical=JSON.stringify(registry);const digest=crypto.createHash('sha512').update(canonical).digest('hex');
const entries=registry.certificates.map(cert=>({id:cert.id,type:cert.type,status:cert.status,issuer_key_id:cert.issuer_key_id||'',public_key:cert.public_key||'',serial:cert.serial||'',sha512:cert.sha512||'',issued_at:cert.issued_at??'',expires_at:cert.expires_at??''}));
const lines=['#pragma once','#include <stdint.h>','', 'namespace certificate_registry {','', 'struct Entry {','    const char* id;','    const char* type;','    const char* status;','    const char* issuer_key_id;','    const char* public_key;','    const char* serial;','    const char* sha512;','    const char* issued_at;','    const char* expires_at;','};','',`constexpr uint32_t kSchema = ${Number(registry.schema)||0};`,`constexpr uint16_t kCertificateVersion = ${Number(registry.peaOS_certificate_version)||0};`,`constexpr uint32_t kCertificateCount = ${entries.length};`,`constexpr char kIssuer[] = ${cppString(registry.issuer)};`,`constexpr char kAuthentication[] = ${cppString(registry.authentication)};`,`constexpr char kRegistryUrl[] = ${cppString(registryUrl)};`,`constexpr char kPeaOsManifestUrl[] = ${cppString(manifestUrl)};`,`constexpr char kRegistrySha512[] = ${cppString(digest)};`,''];
if(entries.length===0)lines.push('constexpr Entry kCertificates[1]={{"","","","","","","","",""}};');else{lines.push('constexpr Entry kCertificates[]={');for(const e of entries)lines.push(`    {${cppString(e.id)},${cppString(e.type)},${cppString(e.status)},${cppString(e.issuer_key_id)},${cppString(e.public_key)},${cppString(e.serial)},${cppString(e.sha512)},${cppString(e.issued_at)},${cppString(e.expires_at)}},`);lines.push('};');}
lines.push('','} // namespace certificate_registry','');
fs.mkdirSync(path.dirname(output),{recursive:true});fs.writeFileSync(output,lines.join('\n'),'utf8');console.log(`certificate registry sync: ${entries.length} signing certificate(s), sha512=${digest}`);
