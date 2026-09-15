import http from 'node:http';
import { spawn } from 'node:child_process';
import { once } from 'node:events';
import fs from 'node:fs';

process.env.PORT='18787';process.env.PEAOS_CERT_ADMIN_TOKEN='self-test-token';process.env.PEAOS_CERT_KEY='./self-test-issuer.pem';process.env.PEAOS_CERT_DB='./self-test-certificates.json';
try{fs.rmSync(process.env.PEAOS_CERT_KEY);fs.rmSync(process.env.PEAOS_CERT_DB);}catch{}
const child=spawn(process.execPath,['server.mjs'],{env:process.env,stdio:['ignore','pipe','inherit']});await once(child.stdout,'data');
const request=(method,path,body,token)=>new Promise((resolve,reject)=>{const headers={'content-type':'application/json'};if(token)headers.authorization=`Bearer ${token}`;const req=http.request({hostname:'127.0.0.1',port:18787,path,method,headers},res=>{let data='';res.on('data',c=>data+=c);res.on('end',()=>resolve({status:res.statusCode,body:JSON.parse(data)}));});req.on('error',reject);if(body)req.write(JSON.stringify(body));req.end();});
const issuer=await request('GET','/v1/issuer');if(issuer.status!==200||issuer.body.algorithm!=='Ed25519'||issuer.body.protocol!==2)throw new Error('issuer endpoint failed');
const issued=await request('POST','/v1/certificates/issue',{application_count:1,origin:'https://example.invalid',applications:[{package_hash:'a'.repeat(64)}]},'self-test-token');
if(issued.status!==201||!issued.body.active||!issued.body.certificate.signature||issued.body.certificate.serial<=0)throw new Error('signed issuance failed');
const id=issued.body.certificate_id;const active=await request('GET',`/v1/certificates/${id}`);
if(active.status!==200||!active.body.active||active.body.certificate.signature!==issued.body.certificate.signature||active.body.provenance.serial!==issued.body.certificate.serial)throw new Error('server provenance validation failed');
const revoked=await request('POST',`/v1/certificates/${id}/revoke`,null,'self-test-token');if(revoked.status!==200||revoked.body.active)throw new Error('revocation failed');
const after=await request('GET',`/v1/certificates/${id}`);if(after.status!==200||after.body.active||after.body.reason!=='revoked')throw new Error('revocation was not enforced');
child.kill();try{fs.rmSync(process.env.PEAOS_CERT_KEY);fs.rmSync(process.env.PEAOS_CERT_DB);}catch{}console.log('certificate service self-test: OK');
