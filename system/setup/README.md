# peaOS first-run setup and activation

The first boot of peaOS 1 Beta 1 must not silently mark the installation as activated.

Required first-run sequence:

1. Choose language, keyboard layout, timezone and display defaults.
2. Create the local user account.
3. Generate the installation identity and device-bound key material through the privileged X90 security service.
4. Show the activation screen.
5. Connect to the configured network when available.
6. Verify the peaOS activation service and its server identity.
7. Verify the installation package/origin and the activation token.
8. Persist the activation record only after a valid signed response.
9. If verification fails, keep the installation in `Unactivated` or `Failed`; never manufacture an `Active` state locally.

Activation states:

- `Unactivated`: fresh installation or no successful activation.
- `Activating`: online verification is in progress.
- `Active`: the signed activation has been accepted by the privileged service.
- `OfflineGrace`: only for a future, explicitly defined grace policy. It is not implemented yet.
- `Revoked`: the activation authority has revoked the installation.
- `Failed`: the last activation attempt was rejected or malformed.

Native application cloud certificates use the same principle: a certificate copied to a different web origin, modified package, or unauthorized device cannot be made active by editing local metadata. The final implementation must verify signed server responses, package digests, authorized web origin and device binding in the privileged X90 security layer.
