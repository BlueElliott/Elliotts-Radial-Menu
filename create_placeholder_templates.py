#!/usr/bin/env python3
"""
Create placeholder template icons by copying an existing icon
"""

import shutil
import os

# Use the chair icon as a placeholder for now
source = "art/Finals/chair.dds"
output_dir = "art/Finals"

for i in range(1, 10):
    dest = os.path.join(output_dir, f"template{i}.dds")
    if not os.path.exists(dest):
        shutil.copy(source, dest)
        print(f"Created placeholder: {dest}")
    else:
        print(f"Already exists: {dest}")

print("\nPlaceholder template icons created.")
print("Replace these with actual helmet icons once extracted from the sprite sheet.")
