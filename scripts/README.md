# Scripts

This folder contains utility scripts for development:

## Deployment
- **deploy.bat** - Deploy the built DLL to Guild Wars 2 addons folder
  - Usage: Run from project root or scripts folder
  - Checks if GW2 is running before deploying
  - Copies Debug build to GW2 installation

## Art/Asset Processing
- **convert_to_dds.py** - Convert PNG images to DDS format for game textures
- **create_placeholder_templates.py** - Generate placeholder template icons
- **extract_templates.py** - Extract helmet icons from equipment sprite sheets

All Python scripts should be run from the scripts folder, as they use relative paths to `../art/Finals/`.
