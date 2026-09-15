import http from 'node:http';
import crypto from 'node:crypto';
import fs from 'node:fs';

const PORT=Number(process.env.PORT||8787);
const DATA_FILE=process.env.PEAOS_CERT_DB||'./certificates.json';
const KEY_FILE=process.env.PEAOS_CERT_KEY||'./issuer-ed25519-private.pem';
const GITHUB_TOKEN=process.env.PEAOS_GITHUB_TOKEN||'';
const GITHUB_OWNER=process.env.PEAOS_GITHUB_OWNER||'alexiusandromedavsgalaxia-lgtm';
const GITHUB_REPO=process.env.PEAOS_GITHUB_REPO||'peaOS.Officialy-certificates';
const GITHUB_BRANCH=process.env.PEAOS_GITHUB_BRANCH||'main';
const REGISTRY_PATH=process.env.PEAOS_GITHUB_REGISTRY_PATH||'registry.json';
const CERTIFICATE_DIR=process.env.PEAOS_CERTIFICATE_DIR||'certificates';
const ADMIN_TOKEN=process.env.PEAOS_CERT_ADMIN_TOKEN||'';
const LOCAL_ONLY=process.env.PEAOS_CERT_LOCAL_ONLY==='true';
const REGISTRY_ISSUER='peaOS.Officialy-certificates';
const REGISTRY_VERSION=3;
const CERTIFICATE_VERSION=3;

if(ADMIN_TOKEN&&ADMIN_TOKEN.length<64)throw new Error('PEAOS_CERT_ADMIN_TOKEN must contain at least 64 characters');
if(!GITHUB_TOKEN&&!LOCAL_ONLY)console.warn('peaOS certificate service: PEAOS_GITHUB_TOKEN is not configured; GitHub publication is disabled.');

let privateKey,publicKey;
try{privateKey=crypto.createPrivateKey(fs.readFileSync(KEY_FILE));publicKey=crypto.createPublicKey(privateKey);}catch{({privateKey,publicKey}=crypto.generateKeyPairSync('ed25519'));fs.writeFileSync(KEY_FILE,privateKey.export({type:'pkcs8',format:'pem'}),{mode:0o600});}
const issuerKeyId=crypto.createHash('sha256').update(publicKey.export({type:'spki',format:'der'})).digest('hex').slice(0,32);
let state=new Map();
try{const saved=JSON.parse(fs.readFileSync(DATA_FILE,'utf8'));for(const r of saved)state.set(r.certificate.certificate_id,r);}catch{}
function persist(){fs.writeFileSync(DATA_FILE,JSON.stringify([...state.values()],null,2),{mode:0o600});}
function json(res,status,body,extra={}){const data=Buffer.from(JSON.stringify(body));res.writeHead(status,{'content-type':'application/json; charset=utf-8','content-length':data.length,'cache-control':'no-store','access-control-allow-origin':'*','access-control-allow-methods':'GET,POST,OPTIONS','access-control-allow-headers':'content-type,authorization',...extra});res.end(data);}
function validName(v){return typeof v==='string'&&v.trim().length>0&&v.trim().length<=120;}
function canonical(c){return JSON.stringify({version:c.version,type:c.type,certificate_id:c.certificate_id,issuer_key_id:c.issuer_key_id,issued_at:c.issued_at,expires_at:c.expires_at,serial:c.serial,public_key:c.public_key});}
function sign(c){return crypto.sign(null,Buffer.from(canonical(c)),privateKey).toString('base64');}
function certificatePayload(c){return JSON.stringify(c,null,2)+'\n';}
function certificateSha512(c){return crypto.createHash('sha512').update(certificatePayload(c)).digest('hex');}
function validCertificate(c){return c&&c.version===CERTIFICATE_VERSION&&c.type==='SigningCertificate'&&/^[a-f0-9]{32}$/.test(c.certificate_id)&&/^[a-f0-9]{32}$/.test(c.issuer_key_id)&&/^[a-f0-9]{16}$/.test(c.serial)&&Number.isSafeInteger(c.issued_at)&&Number.isSafeInteger(c.expires_at)&&c.expires_at>c.issued_at&&c.expires_at-c.issued_at<=365*24*60*60&&typeof c.public_key==='string'&&typeof c.signature==='string'&&typeof c.sha512==='string'&&c.sha512===certificateSha512(c);}
function active(r){const now=Math.floor(Date.now()/1000);return !r.revoked&&now>=r.certificate.issued_at&&now<r.certificate.expires_at;}
function authorized(req){return !!ADMIN_TOKEN&&req.headers.authorization===`Bearer ${ADMIN_TOKEN}`;}
function nextSerial(){return crypto.randomBytes(8).toString('hex');}
async function body(req){let d='';for await(const c of req){d+=c;if(d.length>65536)throw new Error('body_too_large')}return JSON.parse(d);}
async function github(path,options={}){if(!GITHUB_TOKEN)throw new Error('github_backend_not_configured');const response=await fetch(`https://api.github.com${path}`,{...options,headers:{Accept:'application/vnd.github+json',Authorization:`Bearer ${GITHUB_TOKEN}`,'X-GitHub-Api-Version':'2022-11-28','Content-Type':'application/json',...(options.headers||{})}});const data=await response.json().catch(()=>({}));if(!response.ok)throw new Error(data.message||`github_http_${response.status}`);return data;}
async function readRegistry(){const data=await github(`/repos/${GITHUB_OWNER}/${GITHUB_REPO}/contents/${REGISTRY_PATH}?ref=${encodeURIComponent(GITHUB_BRANCH)}`);const registry=JSON.parse(Buffer.from(data.content||'','base64').toString('utf8'));if(registry.schema!==1||registry.issuer!==REGISTRY_ISSUER||registry.peaOS_certificate_version!==REGISTRY_VERSION||!Array.isArray(registry.certificates))throw new Error('github_registry_invalid');return{registry,sha:data.sha};}
async function writeFile(path,content,sha,message){const encoded=Buffer.from(content).toString('base64');const payload={message,content:encoded,branch:GITHUB_BRANCH};if(sha)payload.sha=sha;return github(`/repos/${GITHUB_OWNER}/${GITHUB_REPO}/contents/${path}`,{method:'PUT',body:JSON.stringify(payload)});}
async function publishCertificate(c){if(LOCAL_ONLY)return;const certPath=`${CERTIFICATE_DIR}/${c.certificate_id}.json`;for(let attempt=0;attempt<3;attempt++){const {registry,sha}=await readRegistry();if(registry.certificates.some(x=>x.id===c.certificate_id))return;registry.certificates.push({id:c.certificate_id,type:c.type,status:'Active',issuer_key_id:c.issuer_key_id,public_key:c.public_key,serial:c.serial,issued_at:c.issued_at,expires_at:c.expires_at,sha512:c.sha512});try{await writeFile(REGISTRY_PATH,`${JSON.stringify(registry,null,2)}\n`,sha,`Register signing certificate ${c.certificate_id}`);break}catch(error){if(!String(error.message).includes('does not match')||attempt===2)throw error;}}await writeFile(certPath,certificatePayload(c),null,`Publish signing certificate ${c.certificate_id}`);}
function issue(){const now=Math.floor(Date.now()/1000);const {publicKey:subjectPublic,privateKey:subjectKey}=crypto.generateKeyPairSync('ed25519');const c={version:CERTIFICATE_VERSION,type:'SigningCertificate',certificate_id:crypto.randomBytes(16).toString('hex'),issuer_key_id:issuerKeyId,issued_at:now,expires_at:now+365*24*60*60,serial:nextSerial(),public_key:subjectPublic.export({type:'spki',format:'der'}).toString('base64')};c.signature=sign(c);c.sha512=certificateSha512(c);return{certificate:c,private_key:subjectKey.export({type:'pkcs8',format:'pem'})};}
const server=http.createServer(async(req,res)=>{if(req.method==='OPTIONS')return json(res,204,{});const url=new URL(req.url,`http://${req.headers.host||'localhost'}`);if(req.method==='GET'&&url.pathname==='/v1/issuer')return json(res,200,{protocol:3,issuer:REGISTRY_ISSUER,issuer_key_id:issuerKeyId,algorithm:'Ed25519',certificate_version:CERTIFICATE_VERSION,public_key:publicKey.export({type:'spki',format:'der'}).toString('base64')});const match=url.pathname.match(/^\/v1\/certificates\/([a-f0-9]{32})$/);if(req.method==='GET'&&match){const r=state.get(match[1]);if(!r)return json(res,404,{active:false,reason:'unknown_certificate'});return json(res,200,{certificate:r.certificate,active:active(r),revoked:r.revoked,reason:r.revoked?'revoked':active(r)?'active':'expired'});}if(req.method==='POST'&&(url.pathname==='/v1/certificates/issue'||url.pathname==='/v1/registrations')){if(!authorized(req))return json(res,401,{ok:false,reason:'unauthorized'});let input;try{input=await body(req);}catch{return json(res,400,{ok:false,reason:'invalid_json'});}if(!validName(input.name))return json(res,400,{ok:false,reason:'invalid_name'});if(!GITHUB_TOKEN&&!LOCAL_ONLY)return json(res,503,{ok:false,reason:'github_backend_not_configured'});try{const issued=issue(),r={certificate:issued.certificate,revoked:false,owner:String(input.name).trim()};if(!validCertificate(issued.certificate))return json(res,500,{ok:false,reason:'certificate_self_check_failed'});await publishCertificate(issued.certificate);state.set(issued.certificate.certificate_id,r);persist();return json(res,201,{ok:true,certificate_id:issued.certificate.certificate_id,type:'SigningCertificate',certificate:issued.certificate,private_key:issued.private_key});}catch(error){return json(res,502,{ok:false,reason:error.message});}}const revoke=url.pathname.match(/^\/v1\/certificates\/([a-f0-9]{32})\/revoke$/);if(req.method==='POST'&&revoke){if(!authorized(req))return json(res,401,{active:false,reason:'unauthorized'});const r=state.get(revoke[1]);if(!r)return json(res,404,{active:false,reason:'unknown_certificate'});r.revoked=true;persist();return json(res,200,{active:false,reason:'revoked'});}return json(res,404,{error:'not_found'});});
server.listen(PORT,()=>console.log(`peaOS signing certificate service listening on :${PORT}`));
