import http from 'node:http';
import { spawn } from 'node:child_process';
import { once } from 'node:events';
import fs from 'node:fs';

const TEST_TOKEN='self-test-signing-token-9f4d8a7c2b1e6f0d3c5a7b9e1d4f6a8c2b5e7f9d1c3a6e8b0d2f4a7c9e1b3d5f7a9c';
process.env.PORT='18787';process.env.PEAOS_CERT_ADMIN_TOKEN=TEST_TOKEN;process.env.PEAOS_CERT_KEY='./self-test-issuer.pem';process.env.PEAOS_CERT_DB='./self-test-certificates.json';process.env.PEAOS_CERT_LOCAL_ONLY='true';
try{fs.rmSync(process.env.PEAOS_CERT_KEY);fs.rmSync(process.env.PEAOS_CERT_DB);}catch{}
const child=spawn(process.execPath,['server.mjs'],{env:process.env,stdio:['ignore','pipe','inherit']});await once(child.stdout,'data');
const request=(method,path,body,token)=>new Promise((resolve,reject)=>{const headers={'content-type':'application/json'};if(token)headers.authorization=`Bearer ${token}`;const req=http.request({hostname:'127.0.0.1',port:18787,path,method,headers},res=>{let data='';res.on('data',c=>data+=c);res.on('end',()=>resolve({status:res.statusCode,body:JSON.parse(data)}));});req.on('error',reject);if(body)req.write(JSON.stringify(body));req.end();});
const issuer=await request('GET','/v1/issuer');if(issuer.status!==200||issuer.body.algorithm!=='Ed25519'||issuer.body.protocol!==3||issuer.body.certificate_version!==3)throw new Error('issuer endpoint failed');
const unauthorized=await request('POST','/v1/certificates/issue',{name:'unauthorized'});if(unauthorized.status!==401)throw new Error('admin authentication failed');
const issued=await request('POST','/v1/certificates/issue',{name:'R35 self test'},TEST_TOKEN);if(issued.status!==201||!issued.body.certificate||issued.body.type!=='SigningCertificate'||!issued.body.certificate.signature||issued.body.certificate.sha512.length!==128||issued.body.certificate.public_key)throw new Error('signing issuance failed');
const id=issued.body.certificate_id;const active=await request('GET',`/v1/certificates/${id}`);if(active.status!==200||!active.body.active||active.body.certificate.sha512!==issued.body.certificate.sha512)throw new Error('server provenance validation failed');
const revoked=await request('POST',`/v1/certificates/${id}/revoke`,null,TEST_TOKEN);if(revoked.status!==200||revoked.body.active)throw new Error('revocation failed');
const after=await request('GET',`/v1/certificates/${id}`);if(after.status!==200||after.body.active||after.body.reason!=='revoked')throw new Error('revocation was not enforced');
child.kill();try{fs.rmSync(process.env.PEAOS_CERT_KEY);fs.rmSync(process.env.PEAOS_CERT_DB);}catch{}console.log('certificate service self-test: OK');
