import http from 'node:http';
import crypto from 'node:crypto';
import fs from 'node:fs';

const PORT = Number(process.env.PORT || 8787);
const DATA_FILE = process.env.PEAOS_CERT_DB || './certificates.json';
const KEY_FILE = process.env.PEAOS_CERT_KEY || './issuer-ed25519-private.pem';
const ADMIN_TOKEN = process.env.PEAOS_CERT_ADMIN_TOKEN || '';

let privateKey;
let publicKey;
try {
  privateKey = crypto.createPrivateKey(fs.readFileSync(KEY_FILE));
  publicKey = crypto.createPublicKey(privateKey);
} catch {
  ({ privateKey, publicKey } = crypto.generateKeyPairSync('ed25519'));
  fs.writeFileSync(KEY_FILE, privateKey.export({ type: 'pkcs8', format: 'pem' }), { mode: 0o600 });
}
const issuerKeyId = crypto.createHash('sha256').update(publicKey.export({ type: 'spki', format: 'der' })).digest('hex').slice(0, 32);
let state = new Map();
try { const saved = JSON.parse(fs.readFileSync(DATA_FILE, 'utf8')); for (const record of saved) state.set(record.certificate.certificate_id, record); } catch {}
function persist() { fs.writeFileSync(DATA_FILE, JSON.stringify([...state.values()], null, 2), { mode: 0o600 }); }
function json(res, status, body) { const data = Buffer.from(JSON.stringify(body)); res.writeHead(status, {'content-type':'application/json; charset=utf-8','content-length':data.length,'cache-control':'no-store'}); res.end(data); }
function canonical(cert) { return JSON.stringify({version:cert.version,certificate_id:cert.certificate_id,issuer_key_id:cert.issuer_key_id,issued_at:cert.issued_at,expires_at:cert.expires_at,application_count:cert.application_count,distribution:cert.distribution,origin:cert.origin,applications:cert.applications.map(a=>({package_hash:a.package_hash}))}); }
function sign(cert) { return crypto.sign(null, Buffer.from(canonical(cert)), privateKey).toString('base64'); }
function signatureValid(cert) { try { return crypto.verify(null, Buffer.from(canonical(cert)), publicKey, Buffer.from(cert.signature,'base64')); } catch { return false; } }
function certificateState(record) { const cert=record.certificate; const now=Math.floor(Date.now()/1000); if(record.revoked)return {active:false,reason:'revoked',checked_at:now}; if(now<cert.issued_at||now>=cert.expires_at)return {active:false,reason:'expired',checked_at:now}; if(record.origin!==cert.origin)return {active:false,reason:'origin_mismatch',checked_at:now}; if(!signatureValid(cert))return {active:false,reason:'signature_invalid',checked_at:now}; return {active:true,reason:'active',checked_at:now}; }
function validOrigin(origin) { try { const u=new URL(origin); return u.protocol==='https:'&&!u.username&&!u.password; } catch { return false; } }
function verifyShape(cert) { if(!cert||cert.version!==1)return 'invalid_version'; if(!/^[a-f0-9]{32}$/.test(cert.certificate_id))return 'invalid_certificate_id'; if(cert.issuer_key_id!==issuerKeyId)return 'issuer_mismatch'; if(!Number.isSafeInteger(cert.issued_at)||!Number.isSafeInteger(cert.expires_at))return 'invalid_time'; if(cert.expires_at<=cert.issued_at||cert.expires_at-cert.issued_at>90*24*60*60)return 'invalid_lifetime'; if(!Number.isInteger(cert.application_count)||cert.application_count<1||cert.application_count>5)return 'invalid_application_count'; if(cert.distribution!=='web'||!validOrigin(cert.origin))return 'invalid_web_distribution'; if(!Array.isArray(cert.applications)||cert.applications.length!==cert.application_count)return 'invalid_bindings'; for(const app of cert.applications)if(!/^[a-f0-9]{64}$/.test(app.package_hash))return 'invalid_package_hash'; return null; }
async function body(req) { let data=''; for await(const c of req)data+=c; return JSON.parse(data); }
function authorized(req) { return ADMIN_TOKEN&&req.headers.authorization===`Bearer ${ADMIN_TOKEN}`; }
const server=http.createServer(async(req,res)=>{ const url=new URL(req.url,`http://${req.headers.host||'localhost'}`); if(req.method==='GET'&&url.pathname==='/v1/issuer')return json(res,200,{protocol:1,issuer_key_id:issuerKeyId,algorithm:'Ed25519',public_key:publicKey.export({type:'spki',format:'der'}).toString('base64')}); const match=url.pathname.match(/^\/v1\/certificates\/([a-f0-9]{32})$/); if(req.method==='GET'&&match){const record=state.get(match[1]);if(!record)return json(res,404,{active:false,reason:'unknown_certificate'});return json(res,200,{certificate:record.certificate,...certificateState(record),provenance:{issuer_key_id:issuerKeyId,origin:record.origin,package_hashes:record.certificate.applications.map(a=>a.package_hash)}});} if(req.method==='POST'&&url.pathname==='/v1/certificates/issue'){if(!authorized(req))return json(res,401,{active:false,reason:'unauthorized'});let input;try{input=await body(req);}catch{return json(res,400,{active:false,reason:'invalid_json'});}const now=Math.floor(Date.now()/1000);const cert={version:1,certificate_id:crypto.randomBytes(16).toString('hex'),issuer_key_id:issuerKeyId,issued_at:now,expires_at:Math.min(input.expires_at||now+90*24*60*60,now+90*24*60*60),application_count:input.application_count,distribution:'web',origin:input.origin,applications:input.applications||[]};const error=verifyShape(cert);if(error)return json(res,400,{active:false,reason:error});cert.signature=sign(cert);const record={certificate:cert,origin:cert.origin,revoked:false};state.set(cert.certificate_id,record);persist();return json(res,201,{certificate_id:cert.certificate_id,...certificateState(record),certificate:cert});} const revoke=url.pathname.match(/^\/v1\/certificates\/([a-f0-9]{32})\/revoke$/); if(req.method==='POST'&&revoke){if(!authorized(req))return json(res,401,{active:false,reason:'unauthorized'});const record=state.get(revoke[1]);if(!record)return json(res,404,{active:false,reason:'unknown_certificate'});record.revoked=true;persist();return json(res,200,{active:false,reason:'revoked'});} return json(res,404,{error:'not_found'}); });
server.listen(PORT,()=>console.log(`peaOS certificate service listening on :${PORT}`));
