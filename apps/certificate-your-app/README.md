# Certificate Your App

**Certificate Your App** is the official peaOS application-signing tool for `.pea`, `.xpea` and `.peac` packages.

## Free tier

The free certificate is deliberately small and predictable:

- 1 certificate per account/company identity.
- Maximum 5 applications bound to that certificate.
- 90-day validity period.
- Renewal is manual. The app never silently renews a certificate.
- Renewal creates a new certificate and replaces the expired certificate in the system trust store.
- Applications whose certificate has expired or been revoked cannot be launched by peaOS.

## Signing flow

1. Open **Certificate Your App**.
2. Select a native `.pea`, `.xpea` or `.peac` package.
3. The application checks that the package is a supported native format and has a valid manifest.
4. The package is bound to the company's certificate identity.
5. The signing service signs the package digest and certificate metadata.
6. The signed package is exported for installation.
7. The certificate private material stays in the protected certificate service and is never embedded as plain source data in the application package.

The installer rejects a native package with a missing, malformed, expired, revoked or mismatched certificate.

## Web Distribution Program

Internet distribution has an additional trust boundary. A package intended for public web distribution must be enrolled in the **peaOS Web Distribution Program** before it can be installed from the web.

A package can therefore be:

- locally signed and installed through a trusted local transfer;
- web-distribution signed and enrolled for public web installation;
- rejected because it has no valid certificate;
- rejected because its web distribution enrollment is missing or invalid.

The Web Distribution Program is not a way around signing. It is an additional policy layer above signing.

## Important security rule

The downloadable application package does **not** contain the company's private signing key. If a package is copied or uploaded without its valid certificate/signature envelope, peaOS treats it as unsigned and refuses to launch it.

Certificate expiry is checked by the OS trust service, not only by the application itself. This prevents an application from bypassing the three-month validity period by simply changing its own clock or UI state.
