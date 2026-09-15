import fs from 'node:fs';
import crypto from 'node:crypto';

const output = process.argv[2] || 'build/generated/certificate_registry.hpp';
const manifestUrl = 'https://raw.githubusercontent.com/alexiusandromedavsgalaxia-lgtm/peaOS/main/peacloud.json';
const registryUrl = 'https://raw.githubusercontent.com/alexiusandromedavsgalaxia-lgtm/peaOS.Officialy-certificates/main/registry.json';

function cppString(value) {
  return JSON.stringify(String(value ?? ''));
}

function fail(message) {
  console.error(`certificate registry sync: ${message}`);
  process.exit(1);
}

const [manifestResponse, registryResponse] = await Promise.all([
  fetch(`${manifestUrl}?_=${Date.now()}`, { cache: 'no-store' }),
  fetch(`${registryUrl}?_=${Date.now()}`, { cache: 'no-store' })
]);

if (!manifestResponse.ok) fail(`peaOS manifest fetch failed: HTTP ${manifestResponse.status}`);
if (!registryResponse.ok) fail(`certificate registry fetch failed: HTTP ${registryResponse.status}`);

let manifest;
let registry;
try {
  manifest = await manifestResponse.json();
  registry = await registryResponse.json();
} catch (error) {
  fail(`remote JSON is invalid: ${error.message}`);
}

if (manifest.product !== 'peaOS') fail('manifest product is not peaOS');
if (registry.issuer !== 'peaOS.Officialy-certificates') fail('registry issuer mismatch');
if (manifest.certificate_version !== registry.peaOS_certificate_version) fail('certificate version mismatch');
if (!Array.isArray(registry.certificates)) fail('registry.certificates is not an array');

for (const [index, cert] of registry.certificates.entries()) {
  if (!cert || typeof cert !== 'object') fail(`certificate ${index} is not an object`);
  if (typeof cert.id !== 'string' || !cert.id) fail(`certificate ${index} has no id`);
  if (typeof cert.status !== 'string' || !cert.status) fail(`certificate ${cert.id} has no status`);
}

const canonical = JSON.stringify(registry);
const digest = crypto.createHash('sha256').update(canonical).digest('hex');
const entries = registry.certificates.map(cert => ({
  id: cert.id,
  status: cert.status,
  origin: cert.origin || '',
  distribution: cert.distribution || '',
  issued_at: cert.issued_at ?? '',
  expires_at: cert.expires_at ?? ''
}));

const lines = [
  '#pragma once',
  '#include <stdint.h>',
  '',
  'namespace certificate_registry {',
  '',
  'struct Entry {',
  '    const char* id;',
  '    const char* status;',
  '    const char* origin;',
  '    const char* distribution;',
  '    const char* issued_at;',
  '    const char* expires_at;',
  '};',
  '',
  `constexpr uint32_t kSchema = ${Number(registry.schema) || 0};`,
  `constexpr uint16_t kCertificateVersion = ${Number(registry.peaOS_certificate_version) || 0};`,
  `constexpr uint32_t kCertificateCount = ${entries.length};`,
  `constexpr char kIssuer[] = ${cppString(registry.issuer)};`,
  `constexpr char kRegistryUrl[] = ${cppString(registryUrl)};`,
  `constexpr char kPeaOsManifestUrl[] = ${cppString(manifestUrl)};`,
  `constexpr char kRegistrySha256[] = ${cppString(digest)};`,
  ''
];

if (entries.length === 0) {
  lines.push('constexpr Entry kCertificates[1] = {{"", "", "", "", "", ""}};');
} else {
  lines.push('constexpr Entry kCertificates[] = {');
  for (const entry of entries) {
    lines.push(`    {${cppString(entry.id)}, ${cppString(entry.status)}, ${cppString(entry.origin)}, ${cppString(entry.distribution)}, ${cppString(entry.issued_at)}, ${cppString(entry.expires_at)}},`);
  }
  lines.push('};');
}

lines.push('', '} // namespace certificate_registry', '');
fs.mkdirSync(new URL('.', `file://${process.cwd()}/${output}`), { recursive: true });
fs.writeFileSync(output, lines.join('\n'), 'utf8');
console.log(`certificate registry sync: ${entries.length} official certificate(s), sha256=${digest}`);
