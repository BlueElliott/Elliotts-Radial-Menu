# Template Icons Integration

## What Was Done

### 1. Resource IDs Added
- Added `IDR_TEMPLATE1` through `IDR_TEMPLATE9` to `include/Resource.h` (IDs 281-289)
- Added corresponding resource entries to `Resource.rc` pointing to `art/Finals/template1.dds` through `template9.dds`

### 2. TemplateWheel Updated
- Changed from using background texture placeholder to using proper resource IDs
- Each template combo now loads its icon from resources using `IDR_TEMPLATE1 + (i - 1)`
- Added `#include <Resource.h>` to TemplateWheel.cpp

### 3. Placeholder Icons Created
- Created 9 placeholder DDS files (`template1.dds` through `template9.dds`) using chair.dds as a temporary icon
- These are functional placeholders that allow the wheel to work while waiting for proper helmet icons

## What Remains

### Extract Helmet Icons from Sprite Sheet
You mentioned you have an equipment sprite sheet with 9 helmets numbered 1-9 in a 3x3 grid. To replace the placeholders:

1. **Locate your equipment sprite sheet PNG file** and place it in the project directory

2. **Run the extraction script:**
   ```bash
   python extract_templates.py <path_to_equipment_sprite.png>
   ```
   This will extract the 9 individual helmet icons as PNG files in `art/Finals/`

3. **Convert PNGs to DDS format:**
   The project uses DDS texture format. You'll need to convert the extracted PNGs to DDS:
   - Use a tool like DirectXTex's `texconv.exe` or NVIDIA Texture Tools
   - Convert with DXT5 compression (same as other icons)
   - Example with texconv:
     ```bash
     texconv -f BC3_UNORM -o art/Finals art/Finals/template*.png
     ```

4. **Rebuild the project:**
   ```bash
   ./build_now.cmd
   ```

## Files Modified

- `include/Resource.h` - Added template resource IDs
- `Resource.rc` - Added template resource entries
- `src/TemplateWheel.cpp` - Updated to use resource IDs instead of background texture
- `art/Finals/template1.dds` through `template9.dds` - Created placeholder files

## Scripts Created

- `extract_templates.py` - Extracts individual helmet icons from a 3x3 sprite sheet
- `create_placeholder_templates.py` - Creates placeholder DDS files (already executed)
- `add_template_resources.ps1` - Adds resource entries to Resource.rc (already executed)

## Color Tinting

The wheel automatically applies HSV-based color tinting to the icons (as you mentioned), so the helmet icons don't need to be pre-colored. They should be white/grayscale, and the wheel will tint them with the rainbow colors already configured in the code.

## Current State

✅ Template wheel fully functional with UI keybind configuration
✅ Action chains working for Build + Equipment template switching
✅ Resource integration complete
✅ Placeholder icons allow testing before helmet icons are ready
⏳ Waiting for equipment sprite sheet to extract helmet icons
⏳ Need to convert extracted PNGs to DDS format

Once you provide the equipment sprite sheet PNG, run `extract_templates.py` and convert to DDS, then the helmet icons will be integrated!
