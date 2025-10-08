# Release Process

This document describes how to create releases for the Network Analyzer project.

## Automated Release Workflow

The project uses GitHub Actions to automatically build and publish release assets.

### Creating a New Release

1. **Update version in documentation** (if applicable)
2. **Commit all changes** to the main branch
3. **Create and push a version tag**:

```bash
# Create a new tag with semantic versioning (v1.0.0, v1.1.0, etc.)
git tag -a v1.0.0 -m "Release version 1.0.0"

# Push the tag to GitHub
git push origin v1.0.0
```

4. **Wait for the workflow to complete**
   - The Release workflow will automatically trigger
   - It builds binaries for Linux, macOS, and Windows
   - Creates release assets as tarballs (Linux/macOS) and zip archives (Windows)
   - Publishes them to the GitHub Releases page

### Release Assets

Each release includes:
- `network_monitor-linux-amd64.tar.gz` - Linux x86_64 binary
- `network_monitor-macos-amd64.tar.gz` - macOS x86_64 binary
- `network_monitor-windows-amd64.zip` - Windows x86_64 binary

Each archive contains:
- The compiled `network_monitor` binary (or `network_monitor.exe` on Windows)
- `README.md` documentation
- `LICENSE` file

### Manual Release (Alternative)

You can also manually trigger the release workflow:

1. Go to the Actions tab on GitHub
2. Select the "Release" workflow
3. Click "Run workflow"
4. Choose the branch and click "Run workflow"

Note: Manual runs will create release assets but won't create a GitHub Release unless you also create a tag.

## Version Numbering

We follow [Semantic Versioning](https://semver.org/):

- **MAJOR** version (v1.0.0 → v2.0.0): Incompatible API changes
- **MINOR** version (v1.0.0 → v1.1.0): New functionality, backwards-compatible
- **PATCH** version (v1.0.0 → v1.0.1): Backwards-compatible bug fixes

## CI/CD Workflow

The Build workflow runs on every push and pull request to ensure:
- Code compiles successfully on all platforms
- Build artifacts are generated and stored for 30 days
- Changes don't break the build

## Troubleshooting

**Workflow fails on macOS:**
- Ensure libpcap is available (usually included with Xcode Command Line Tools)

**Workflow fails on Ubuntu:**
- Check that `libpcap-dev` installs correctly
- Review the workflow logs for specific error messages

**Workflow fails on Windows:**
- Ensure Npcap and MinGW are installed correctly via Chocolatey
- Check that the Npcap SDK path is correct (default: `C:/Program Files/Npcap/sdk/`)
- Verify that the build tools are in the system PATH

**Release not created:**
- Verify the tag follows the pattern `v*.*.*` (e.g., `v1.0.0`)
- Check that the tag was pushed to the remote repository
- Review workflow permissions in repository settings
