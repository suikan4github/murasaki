---
name: Release preparation issue template
about: 'To prepare a release, use this template. '
title: Release vX.Y.Z preparation
labels: ''
assignees: suikan4github

---

- [ ] Check all issues are listed in CHANGELOG.md
- [ ] Check doc/refman.pdf is updated
- [ ] Check doc/refman.pdf is "vX.Y.Z" in the front page.
- [ ] Check CHANGELOG.md has vX.Y.Z entry
- [ ] Check all entry in Unreleased is moved to vX.Y.Z

```
git tag
git add tag vX.Y.Z
git push origin vX.Y.Z
```
