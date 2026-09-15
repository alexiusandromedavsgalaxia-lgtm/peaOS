import http from 'node:http';
import { once } from 'node:events';

process.env.PORT = '18787';
const child = (await import('node:child_process')).spawn(process.execPath, ['server.mjs'], { stdio: ['ignore', 'pipe', 'inherit'] });
await once(child.stdout, 'data');

const request = (method, path, body) => new Promise((resolve, reject) => {
  const req = http.request({ hostname: '127.0.0.1', port: 18787, path, method, headers: { 'content-type': 'application/json' } }, res => {
    let data = '';
    res.on('data', c => data += c);
    res.on('end', () => resolve({ status: res.statusCode, body: JSON.parse(data) }));
  });
  req.on('error', reject);
  if (body) req.write(JSON.stringify(body));
  req.end();
});

const issuer = await request('GET', '/v1/issuer');
if (issuer.status !== 200) throw new Error('issuer endpoint failed');
const now = Math.floor(Date.now() / 1000);
const id = '0123456789abcdef0123456789abcdef';
const cert = {
  version: 1,
  certificate_id: id,
  issuer_key_id: issuer.body.issuer_key_id,
  issued_at: now,
  expires_at: now + 3600,
  application_count: 1,
  distribution: 'web',
  origin: 'https://example.invalid',
  applications: [{ package_hash: 'a'.repeat(64) }],
  signature: '1'
};
const created = await request('POST', '/v1/certificates', cert);
if (created.status !== 201 || !created.body.active) throw new Error('certificate activation failed');
const active = await request('GET', `/v1/certificates/${id}`);
if (active.status !== 200 || !active.body.active) throw new Error('certificate validation failed');
const revoked = await request('POST', `/v1/certificates/${id}/revoke`);
if (revoked.status !== 200 || revoked.body.active) throw new Error('certificate revocation failed');
child.kill();
console.log('certificate service self-test: OK');
