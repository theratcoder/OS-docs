# ADR-0008: Security Model

## Status
Proposed

## Context
RatOS must provide a secure foundation for both legacy Linux applications and RatAPI-native applications. The security model should balance:

- **Compatibility**: Retain Linux DAC (UID/GID, permissions) for existing apps.
- **Usability**: Offer developers and users a clear and consistent model for permissions in RatAPI.
- **Robustness**: Leverage proven Linux primitives rather than building new kernel subsystems from scratch.
- **Future-proofing**: Provide a path for stronger confinement and app-level permissions as RatOS grows.

RatOS also aims to offload as much responsibility for low-level enforcement as possible to trusted third parties (Linux kernel, LSM modules), minimizing the amount of custom security-critical code in RatOS itself.

## Options Considered
1. **Rely solely on Linux DAC (UID/GID)**  
   - Simple, compatible, but coarse-grained and not suitable for modern security expectations.

2. **Implement custom RatOS-only security kernel extensions**  
   - Would give full control, but very high implementation complexity and security risk.

3. **Adopt a hybrid model: Linux primitives + RatAPI semantics**  
   - Kernel-level security is enforced by proven Linux mechanisms (capabilities, namespaces, seccomp, LSM/AppArmor).
   - RatAPI adds higher-level ACLs, permissions, and capability tokens for Rat-native objects.
   - App installation is tied to manifest-declared permissions, with signing and optional runtime prompts.

## Decision
RatOS will adopt a **hybrid security model**:

1. **Base compatibility**  
   - Preserve standard Linux DAC (UID/GID, r/w/x) for existing applications.  
   - Use Linux capabilities (`CAP_*`) to restrict privileged operations.

2. **Kernel-enforced isolation**  
   - Use namespaces and cgroups for app sandboxing.  
   - Apply seccomp-BPF filters to restrict syscalls.  
   - Employ an LSM (AppArmor initially) for mandatory access control policies.

3. **RatAPI-level access control**  
   - All named RatAPI objects (e.g. message queues, shared memory, windows, devices) will support ACLs.  
   - RatAPI will introduce **capability tokens** to securely transfer object handles between processes.  
   - Handles themselves will be unforgeable and tied to process permissions.

4. **Application model**  
   - Applications are packaged and signed.  
   - Packages must declare requested permissions (network, camera, filesystem paths).  
   - Users grant permissions at install time (default), with optional runtime prompts for sensitive requests.  
   - Privileged operations are mediated by system broker services.

5. **Networking & firewall integration**  
   - Integrate with Linux netfilter/nftables for per-app network policies, linked to app manifests.

6. **Auditing & logging**  
   - All denials and security-sensitive actions are logged.  
   - Admin tools (`ratctl`) will allow inspection of current ACLs, app permissions, and granted capabilities.

## Consequences
- **Pros**  
  - Builds on mature, trusted Linux security infrastructure.  
  - Provides RatAPI-native apps with a consistent and modern security model.  
  - Enables future app-store-like ecosystems via manifest signing and permissions.  
  - Minimizes custom kernel code while still allowing fine-grained RatAPI control.

- **Cons**  
  - Multi-layer model (DAC, LSM, RatAPI) increases conceptual complexity.  
  - Requires integration with package manager, broker services, and UX for permissions.  
  - Policy authoring and defaults must be carefully managed to avoid user friction.

## Alternatives Considered
- **Pure Linux DAC**: too weak for modern systems.  
- **Custom kernel-only model**: too risky and expensive.  
- **High-level sandbox-only model (Option C)**: insufficient granularity for RatAPI objects.

## Notes
- Initial implementation will focus on compatibility and RatAPI ACLs.  
- Manifest-driven permissions and signed packages may be phased in once package infrastructure matures.  
- AppArmor will be used for initial MAC enforcement due to simpler profile model; SELinux may be evaluated later.