# Setting Up Your GW2Radial Repository

## Step 1: Create a New Repository on GitHub

1. Go to [GitHub](https://github.com) and sign in
2. Click the **"+"** button in the top right → **"New repository"**
3. Fill in the details:
   - **Repository name**: `GW2Radial` (or `GW2Radial-Custom`, `GW2Radial-Personal`, etc.)
   - **Description**: "Personal fork of GW2Radial - Guild Wars 2 radial menu addon with custom modifications"
   - **Visibility**:
     - ✅ **Public** - if you want to share your modifications
     - ⚠️ **Private** - if you want to keep it private
   - **DO NOT** initialize with README, .gitignore, or license (we already have these)
4. Click **"Create repository"**

## Step 2: Update Your Local Repository

After creating the GitHub repository, you'll see a page with instructions. Use these commands in Git Bash or Command Prompt:

### Option A: If you created a completely new repo (recommended)

```bash
cd C:\Users\Aurora\Projects\GW2Radial

# Remove the old remote (Friendly0Fire's repository)
git remote remove origin

# Add your new repository as origin
git remote add origin https://github.com/YOUR_USERNAME/GW2Radial.git

# Verify the remote
git remote -v

# Push to your new repository
git push -u origin master
```

### Option B: If you want to keep the original as a reference

```bash
cd C:\Users\Aurora\Projects\GW2Radial

# Rename the original remote to 'upstream'
git remote rename origin upstream

# Add your new repository as 'origin'
git remote add origin https://github.com/YOUR_USERNAME/GW2Radial.git

# Verify remotes
git remote -v

# Push to your new repository
git push -u origin master
```

## Step 3: Push Submodules

The project uses GW2Common as a submodule. You have two options:

### Option A: Keep the original submodule reference (simpler)
No action needed! The submodule will continue pointing to Friendly0Fire's GW2Common repository.

### Option B: Fork GW2Common too (if you want full independence)
1. Go to [GW2Common repository](https://github.com/Friendly0Fire/GW2Common)
2. Click "Fork" to create your own fork
3. Update the submodule reference:
   ```bash
   # Edit .gitmodules to point to your fork
   git config -f .gitmodules submodule.extern/GW2Common.url https://github.com/YOUR_USERNAME/GW2Common.git

   # Commit the change
   git add .gitmodules
   git commit -m "Update GW2Common submodule to personal fork"
   git push
   ```

## Step 4: Add Build Scripts and Documentation

Add the new files we created:

```bash
cd C:\Users\Aurora\Projects\GW2Radial

# Stage the new files
git add build_simple.bat
git add build_debug.bat
git add DEVELOPMENT_SETUP.md
git add GITHUB_SETUP.md
git add .gitignore

# Commit
git commit -m "Add development setup and build scripts"

# Push
git push
```

## Step 5: Update Repository Description (Optional)

1. Go to your repository on GitHub
2. Click the **gear icon** next to "About"
3. Add:
   - **Description**: Short description of your modifications
   - **Website**: Your personal site or the original project URL
   - **Topics**: `guild-wars-2`, `gw2`, `addon`, `radial-menu`, `cpp`, `directx`

## Step 6: Create a Custom README (Optional)

You might want to modify the README to reflect that this is your personal fork:

Add a section at the top of README.md:

```markdown
# GW2Radial - Personal Fork

> This is a personal fork of [GW2Radial by Friendly0Fire](https://github.com/Friendly0Fire/GW2Radial) with custom modifications.

## My Modifications
- (List your custom features here as you add them)

---

# Original README

(Original content follows...)
```

## Useful Git Commands

### Check current remotes
```bash
git remote -v
```

### Pull updates from original repository (if you kept 'upstream')
```bash
git fetch upstream
git merge upstream/master
```

### Check status
```bash
git status
```

### Commit and push changes
```bash
git add .
git commit -m "Your commit message"
git push
```

### View commit history
```bash
git log --oneline -10
```

## Next Steps

After setting up your repository:

1. ✅ Repository is on GitHub under your account
2. ✅ Development environment is configured
3. ⏳ **Build the project** to verify everything works
4. ⏳ Start making your custom modifications!
5. ⏳ Commit and push your changes regularly

## Backup Your Changes

Since this is for personal modifications, remember to:
- **Commit frequently** - Don't lose your work!
- **Push regularly** - GitHub is your backup
- **Write descriptive commit messages** - Future you will thank you

## License Compliance

The original GW2Radial is MIT licensed. This means:
- ✅ You can modify and distribute freely
- ✅ You can use it privately or commercially
- ✅ You must include the original copyright notice
- ✅ You can add your own copyright for your modifications

The LICENSE file is already included. If you make significant modifications, you can add:

```
Copyright (c) 2025 BlueElliott (Your modifications)
Copyright (c) [Original Year] Friendly0Fire (Original work)
```
