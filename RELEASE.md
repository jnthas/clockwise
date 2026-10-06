# Release Checklist & Guide for Clockwise

This document details the complete, step-by-step procedure for releasing a new firmware version of Clockwise.

---

## 1. Pre-Release Verification (Local)

Always verify tests and compilation locally before bumping versions:

1. **Run native unit tests**:
   ```bash
   pio test -d firmware/ -e native
   # or with virtualenv:
   ~/.platformio/penv/bin/pio test -d firmware/ -e native
   ```
2. **Verify embedded build with PlatformIO**:
   Test compile at least one standard clockface (e.g. `cw-cf-0x01`) and the canvas engine (`cw-cf-0x07`):
   ```bash
   ln -sf ../clockfaces/cw-cf-0x01 firmware/lib/cw-cf-0x01
   pio run -d firmware/ -e esp32dev
   rm -f firmware/lib/cw-cf-0x01

   ln -sf ../clockfaces/cw-cf-0x07 firmware/lib/cw-cf-0x07
   pio run -d firmware/ -e esp32dev
   rm -f firmware/lib/cw-cf-0x07
   ```

---

## 2. Version Bump & Documentation Updates

Update the following 4 files in the repository:

1. **`firmware/platformio.ini`**:
   - Update `CW_FW_VERSION` to the new version:
     ```ini
     build_flags = 
         -D CW_FW_VERSION="\"X.Y.Z\""
     ```
2. **`main/CMakeLists.txt`**:
   - Update ESP-IDF build version:
     ```cmake
     target_compile_options(${COMPONENT_TARGET} PUBLIC -DCW_FW_VERSION="X.Y.Z" -DCW_FW_NAME="Clockwise")
     ```
3. **`CHANGELOG.md`**:
   - Add new release section following [Keep a Changelog](https://keepachangelog.com/):
     ```markdown
     ## [X.Y.Z] - YYYY-MM-DD

     ### Added
     - ...

     ### Changed
     - ...

     ### Fixed
     - ...
     ```
4. **`README.md`**:
   - Update top announcement banner link:
     ```markdown
     ![News GIF](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/news.gif) **[Latest version X.Y.Z released!](https://github.com/jnthas/clockwise/releases/tag/vX.Y.Z)** | [See full change log](https://github.com/jnthas/clockwise/blob/main/CHANGELOG.md#xyz---yyyy-mm-dd)
     ```
   - Update the configuration table or feature list if new parameters were introduced.

---

## 3. Commit and Push to `main`

1. Create a branch, commit the release files, and push:
   ```bash
   git checkout -b release/X.Y.Z
   git add CHANGELOG.md README.md firmware/platformio.ini main/CMakeLists.txt
   git commit -m "release: version X.Y.Z"
   ```
2. Merge into `main` and push:
   ```bash
   git checkout main
   git merge release/X.Y.Z
   git push origin main
   ```
3. Check [GitHub Actions](https://github.com/jnthas/clockwise/actions) to ensure **ESP-IDF CI Actions** passes on `main`.

---

## 4. Trigger CI/CD Cloud Binary Builds

The GitHub Actions workflow `.github/workflows/clockwise-ci.yml` triggers automatically on branches matching `releases/**`:

1. **Create and push the release branch from `main`**:
   ```bash
   git branch releases/X.Y.Z main
   git push origin releases/X.Y.Z
   ```
2. **Monitor the workflow execution**:
   - Open [Actions -> Clockwise CI/CD](https://github.com/jnthas/clockwise/actions/workflows/clockwise-ci.yml).
   - The workflow compiles all 7 clockfaces in parallel matrix jobs (`cw-cf-0x01` through `cw-cf-0x07`).
   - For each clockface, it publishes `firmware.bin` and updates `manifest.json` directly into the `gh-pages` branch.
   - **Wait for all clockface matrix jobs to finish successfully before proceeding to the next step** to avoid git push conflicts on `gh-pages`.

---

## 5. Update Website on GitHub Pages (`gh-pages`)

Once the CI/CD workflow has published the binaries to `gh-pages`:

1. Check out or create a temporary git worktree for `gh-pages`:
   ```bash
   git fetch origin gh-pages
   git worktree add ../clockwise-gh-pages origin/gh-pages
   ```
2. In `../clockwise-gh-pages/index.md`, update line 1:
   ```markdown
   > ![News 90s GIF](https://github.com/jnthas/clockwise/raw/gh-pages/static/images/news.gif)[YYYY-MM-DD] [Version X.Y.Z released!](https://github.com/jnthas/clockwise/releases/tag/vX.Y.Z) Check the [change log](https://github.com/jnthas/clockwise/blob/main/CHANGELOG.md#xyz---yyyy-mm-dd) to see the fixes and new features added. Be part of the [Clock Club](https://github.com/jnthas/clock-club) and create your own clockface using Canvas.
   ```
3. Commit, push, and clean up the worktree:
   ```bash
   git -C ../clockwise-gh-pages commit -am "docs: update release announcement to vX.Y.Z"
   git -C ../clockwise-gh-pages push origin HEAD:gh-pages
   git worktree remove ../clockwise-gh-pages
   ```

---

## 6. Create Git Tag & GitHub Release

> [!IMPORTANT]
> A git tag alone does **not** create a GitHub Release. Both the git tag and the GitHub Release object must be created for the release to appear on the repository's Releases page and sidebar.

1. **Create and push the annotated git tag** pointing to the release commit on `main`:
   ```bash
   git tag -a vX.Y.Z -m "Release vX.Y.Z

   <Paste changelog summary here>"
   git push origin vX.Y.Z
   ```
2. **Publish the GitHub Release**:
   - **Option A (GitHub Web UI)**:
     - Navigate to [New Release](https://github.com/jnthas/clockwise/releases/new).
     - Select existing tag: `vX.Y.Z`.
     - Release title: `vX.Y.Z`.
     - Description: Paste the release notes from `CHANGELOG.md`.
     - Click **Publish release**.
   - **Option B (GitHub REST API)**:
     ```bash
     curl -X POST \
       -H "Authorization: token $GITHUB_TOKEN" \
       -H "Accept: application/vnd.github.v3+json" \
       https://api.github.com/repos/jnthas/clockwise/releases \
       -d '{"tag_name":"vX.Y.Z","name":"vX.Y.Z","body":"<CHANGELOG_BODY>","draft":false,"prerelease":false}'
     ```

---

## 7. Adding a New Clockface (If Applicable)

When introducing an entirely new clockface submodule (e.g. `cw-cf-0x08`):

- [ ] Add the clockface submodule under `firmware/clockfaces/<clockface_name>`.
- [ ] Ensure the clockface repository has a valid `CMakeLists.txt`.
- [ ] Add the clockface identifier to the matrix in `.github/workflows/clockwise-ci.yml`.
- [ ] On `gh-pages`, create `static/firmware/<clockface_name>/` containing initial `manifest.json`.
- [ ] In `main/CMakeLists.txt` and `main/Kconfig.projbuild`, register the new clockface if ESP-IDF support is desired.

---

## 8. Post-Release Verification Checklist

Verify everything is live and functioning:

- [ ] [GitHub Releases](https://github.com/jnthas/clockwise/releases) shows `vX.Y.Z` labeled as **Latest**.
- [ ] [Clockwise Web Portal](https://jnthas.github.io/clockwise/) displays the updated announcement banner.
- [ ] Web self-service flasher loads the newly compiled binaries with manifest date `CW_YYYYMMDD`.
- [ ] [ESP-IDF CI Actions](https://github.com/jnthas/clockwise/actions/workflows/esp-idf.yml) shows green checkmark for `main` and tag `vX.Y.Z`.
