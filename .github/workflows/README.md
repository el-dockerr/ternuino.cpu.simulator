# GitHub Actions Workflows for Ternuino CPU Simulator

This document describes the GitHub Actions workflows set up for automated building, testing, and releasing of the Ternuino CPU simulator C implementation.

## Workflow Overview

### 1. **Build Workflow** (`.github/workflows/build.yml`)
**Triggers:** Push to `main`/`develop`/`feature/*` branches, Pull Requests, Releases

**Purpose:** Build the project on multiple platforms and create artifacts

**Platforms:**
- **Ubuntu Latest** - Linux x86_64 build
- **Windows Latest** - Windows x86_64 build (using MSYS2/MinGW)
- **macOS Latest** - macOS Universal build

**Features:**
- Cross-platform compilation
- Automatic dependency installation
- Build artifact creation
- Basic functionality testing
- Automatic release asset uploading

**Artifacts Created:**
- `ternuino-linux` - Linux executable
- `ternuino-windows` - Windows executable  
- `ternuino-macos` - macOS executable

### 2. **CI Workflow** (`.github/workflows/ci.yml`)
**Triggers:** Push to `main`/`develop`, Pull Requests

**Purpose:** Quick continuous integration checks

**Features:**
- Fast build verification
- Basic program execution tests
- Code quality checks with strict compiler warnings
- Static analysis with cppcheck

**Tests Run:**
- `arithmetic_demo.asm`
- `fibonacci_demo.asm`
- `logic_demo.asm`

### 3. **Release Workflow** (`.github/workflows/release.yml`)
**Triggers:** Git tags matching `v*` pattern (e.g., `v1.0.0`)

**Purpose:** Create official GitHub releases with downloadable binaries

**Process:**
1. Creates a GitHub release from the tag
2. Builds binaries for all platforms
3. Uploads binaries as release assets
4. Generates release notes with usage instructions

**Release Assets:**
- `ternuino-linux` - Linux binary
- `ternuino-windows.exe` - Windows binary
- `ternuino-macos` - macOS binary

### 4. **Nightly Build Workflow** (`.github/workflows/nightly.yml`)
**Triggers:** Daily at 2 AM UTC, Manual dispatch

**Purpose:** Comprehensive testing and performance monitoring

**Features:**
- Debug builds with symbols
- Memory leak testing (Linux with Valgrind)
- Extended program testing
- Performance benchmarking
- 7-day artifact retention

**Benefits:**
- Early detection of regressions
- Performance trend monitoring
- Memory safety verification

### 5. **Code Quality Workflow** (`.github/workflows/code-quality.yml`)
**Triggers:** Push to branches, Pull Requests

**Purpose:** Maintain code quality and security standards

**Checks:**
- **Static Analysis:** cppcheck for potential bugs
- **Code Formatting:** clang-format style verification
- **Strict Compilation:** Maximum warning levels
- **Security Scanning:** flawfinder for security issues
- **Code Review Helpers:** TODO/FIXME detection, file size checks
- **Line Ending Consistency**

## How to Use

### For Development

1. **Regular Development:** 
   - Push to feature branches triggers build and code quality checks
   - Pull requests trigger full CI validation

2. **Code Quality:**
   - Check the "Code Quality" workflow results for formatting and static analysis
   - Address any warnings or suggestions before merging

3. **Testing:**
   - CI workflow provides quick feedback
   - Nightly builds catch longer-term issues

### For Releases

1. **Create a Release:**
   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```

2. **Automatic Process:**
   - Release workflow builds all platforms
   - Creates GitHub release with binaries
   - Users can download pre-compiled executables

### For Monitoring

1. **Build Status:**
   - Check workflow badges in README
   - Monitor build failures in Actions tab

2. **Performance:**
   - Review nightly build performance benchmarks
   - Track execution time trends

## Workflow Dependencies

### Required Secrets
- `GITHUB_TOKEN` - Automatically provided by GitHub for releases

### External Dependencies
**Linux (Ubuntu):**
- `gcc`, `make` - Core build tools
- `cppcheck` - Static analysis
- `clang-format` - Code formatting
- `valgrind` - Memory testing
- `flawfinder` - Security scanning

**Windows:**
- MSYS2 with MinGW64 GCC - Automatically installed via action

**macOS:**
- Xcode Command Line Tools - Pre-installed on runners

## Customization

### Adding New Tests
Edit `.github/workflows/ci.yml` or `.github/workflows/nightly.yml`:

```yaml
- name: Test New Program
  working-directory: src
  run: |
    echo "Testing new_program.asm..."
    timeout 10s "./build/ternuino" programs/new_program.asm
```

### Changing Build Flags
Edit the build steps in workflows:

```yaml
- name: Build with Custom Flags
  working-directory: src
  run: |
    CFLAGS="-O3 -march=native" make
```

### Adding New Platforms
Add to the `strategy.matrix` in `build.yml`:

```yaml
- os: ubuntu-20.04
  name: Ubuntu-20.04
  artifact: ternuino-ubuntu20
  executable: ternuino
```

## Artifact Management

### Build Artifacts
- **Retention:** 30 days for regular builds
- **Retention:** 7 days for nightly builds
- **Contents:** Executable + sample programs + documentation

### Release Assets
- **Permanent:** Attached to GitHub releases
- **Naming:** `ternuino-{platform}[.exe]`
- **Contents:** Just the executable (users download sample programs separately)

## Troubleshooting

### Common Build Failures

1. **Windows MSYS2 Issues:**
   - Check MSYS2 setup action version
   - Verify MinGW package names

2. **macOS Compilation:**
   - Usually works out-of-box
   - Check Xcode version if issues arise

3. **Linux Dependencies:**
   - Verify package installation commands
   - Check for missing libraries

### Debugging Workflows

1. **Enable Debug Logging:**
   ```yaml
   - name: Debug Step
     run: |
       set -x  # Enable verbose output
       your_commands_here
   ```

2. **Add Diagnostic Steps:**
   ```yaml
   - name: Environment Info
     run: |
       gcc --version
       make --version
       ls -la src/
   ```

### Manual Testing
You can manually trigger workflows:
- Go to "Actions" tab in GitHub
- Select workflow
- Click "Run workflow" for workflows with `workflow_dispatch`

## Performance Considerations

### Build Speed Optimization
- Parallel builds where possible
- Cached dependencies (could be added)
- Minimal artifact content

### Resource Usage
- Workflows run on GitHub's shared runners
- Build time typically 2-5 minutes per platform
- Reasonable resource consumption for open source projects

## Future Enhancements

1. **Caching:** Add dependency caching for faster builds
2. **Matrix Expansion:** Add more OS versions/architectures
3. **Integration Testing:** Add tests with external tools
4. **Documentation:** Auto-generate docs from builds
5. **Performance Tracking:** Store and trend performance metrics

This comprehensive setup ensures reliable, cross-platform builds with good quality control and easy release management.
