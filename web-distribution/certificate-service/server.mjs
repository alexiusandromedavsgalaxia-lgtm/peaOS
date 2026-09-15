import http from 'node:http';
import crypto from 'node:crypto';

const PORT = Number(process.env.PORT || 8787);
const ISSUER_ID = process.env.PEAOS_ISSUER_ID || 'peaOS-web-issuer-v1';
const state = new Map();

function json(res, status, body) {
  const data = Buffer.from(JSON.stringify(body));
  res.writeHead(status, {
    'content-type': 'application/json; charset=utf-8',
    'content-length': data.length,
    'cache-control': 'no-store'
  });
  res.end(data);
}

function sha256(value) {
  return crypto.createHash('sha256').update(value).digest('hex');
}

function canonical(cert) {
  return JSON.stringify({
    version: cert.version,
    certificate_id: cert.certificate_id,
    issuer_key_id: cert.issuer_key_id,
    issued_at: cert.issued_at,
    expires_at: cert.expires_at,
    application_count: cert.application_count,
    distribution: cert.distribution,
    origin: cert.origin,
    applications: cert.applications.map(a => ({ package_hash: a.package_hash }))
  });
}

function certificateState(cert) {
  const record = state.get(cert.certificate_id);
  if (!record) return { active: false, reason: 'unknown_certificate' };
  const now = Math.floor(Date.now() / 1000);
  if (record.revoked) return { active: false, reason: 'revoked' };
  if (now < cert.issued_at || now >= cert.expires_at) return { active: false, reason: 'expired' };
  if (record.origin !== cert.origin) return { active: false, reason: 'origin_mismatch' };
  return { active: true, reason: 'active', checked_at: now };
}

function verifyShape(cert) {
  if (!cert || cert.version !== 1) return 'invalid_version';
  if (!/^[a-f0-9]{32}$/.test(cert.certificate_id)) return 'invalid_certificate_id';
  if (!/^[a-f0-9]{32}$/.test(cert.issuer_key_id)) return 'invalid_issuer_key_id';
  if (!Number.isSafeInteger(cert.issued_at) || !Number.isSafeInteger(cert.expires_at)) return 'invalid_time';
  if (cert.expires_at <= cert.issued_at || cert.expires_at - cert.issued_at > 90 * 24 * 60 * 60) return 'invalid_lifetime';
  if (cert.application_count < 1 || cert.application_count > 5) return 'invalid_application_count';
  if (cert.distribution !== 'web') return 'invalid_distribution';
  if (typeof cert.origin !== 'string' || !/^https:\/\//.test(cert.origin)) return 'invalid_origin';
  if (!Array.isArray(cert.applications) || cert.applications.length !== cert.application_count) return 'invalid_bindings';
  for (const app of cert.applications) if (!/^[a-f0-9]{64}$/.test(app.package_hash)) return 'invalid_package_hash';
  if (!/^[a-f0-9]+$/.test(cert.signature || '')) return 'missing_signature';
  return null;
}

const server = http.createServer(async (req, res) => {
  const url = new URL(req.url, `http://${req.headers.host || 'localhost'}`);

  if (req.method === 'GET' && url.pathname === '/v1/issuer') {
    return json(res, 200, { issuer_key_id: sha256(ISSUER_ID).slice(0, 32), issuer: ISSUER_ID, protocol: 1 });
  }

  const match = url.pathname.match(/^\/v1\/certificates\/([a-f0-9]{32})$/);
  if (req.method === 'GET' && match) {
    const id = match[1];
    const record = state.get(id);
    if (!record) return json(res, 404, { active: false, reason: 'unknown_certificate' });
    return json(res, 200, { certificate: record.certificate, ...certificateState(record.certificate) });
  }

  if (req.method === 'POST' && url.pathname === '/v1/certificates') {
    let body = '';
    for await (const chunk of req) body += chunk;
    let cert;
    try { cert = JSON.parse(body); } catch { return json(res, 400, { active: false, reason: 'invalid_json' }); }
    const error = verifyShape(cert);
    if (error) return json(res, 400, { active: false, reason: error });
    if (cert.issuer_key_id !== sha256(ISSUER_ID).slice(0, 32)) return json(res, 403, { active: false, reason: 'issuer_mismatch' });
    if (state.has(cert.certificate_id)) return json(res, 409, { active: false, reason: 'certificate_exists' });
    state.set(cert.certificate_id, { certificate: cert, origin: cert.origin, revoked: false });
    return json(res, 201, { certificate_id: cert.certificate_id, ...certificateState(cert) });
  }

  const revoke = url.pathname.match(/^\/v1\/certificates\/([a-f0-9]{32})\/revoke$/);
  if (req.method === 'POST' && revoke) {
    const record = state.get(revoke[1]);
    if (!record) return json(res, 404, { active: false, reason: 'unknown_certificate' });
    record.revoked = true;
    return json(res, 200, { active: false, reason: 'revoked' });
  }

  json(res, 404, { error: 'not_found' });
});

server.listen(PORT, () => console.log(`peaOS certificate service listening on :${PORT}`));
