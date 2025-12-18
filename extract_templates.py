#!/usr/bin/env python3
"""
Extract individual helmet icons from the equipment sprite sheet.
The sprite sheet contains 9 helmets numbered 1-9 in a 3x3 grid.
"""

from PIL import Image
import os

def extract_helmets(sprite_path, output_dir):
    """Extract 9 helmet icons from sprite sheet."""

    # Load the sprite sheet
    img = Image.open(sprite_path)
    width, height = img.size

    print(f"Sprite sheet size: {width}x{height}")

    # Calculate cell size (assuming 3x3 grid)
    cell_width = width // 3
    cell_height = height // 3

    print(f"Cell size: {cell_width}x{cell_height}")

    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)

    # Extract each helmet
    helmet_num = 1
    for row in range(3):
        for col in range(3):
            # Calculate crop coordinates
            left = col * cell_width
            top = row * cell_height
            right = left + cell_width
            bottom = top + cell_height

            # Crop the cell
            helmet = img.crop((left, top, right, bottom))

            # Save as PNG
            output_path = os.path.join(output_dir, f"template{helmet_num}.png")
            helmet.save(output_path)
            print(f"Saved template{helmet_num}.png")

            helmet_num += 1

if __name__ == "__main__":
    import sys

    if len(sys.argv) < 2:
        print("Usage: python extract_templates.py <equipment_sprite.png>")
        sys.exit(1)

    sprite_path = sys.argv[1]
    output_dir = "art/Finals"

    extract_helmets(sprite_path, output_dir)
    print(f"\nExtracted 9 template icons to {output_dir}/")
