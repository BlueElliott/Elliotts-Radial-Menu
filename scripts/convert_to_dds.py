#!/usr/bin/env python3
"""
Convert PNG template icons to DDS format.
Uses a simple uncompressed DDS format that Direct3D can load.
"""

from PIL import Image
import struct
import os

def write_dds_header(f, width, height, has_alpha=True):
    """Write DDS file header."""
    # DDS magic number
    f.write(b'DDS ')

    # DDS_HEADER
    # dwSize
    f.write(struct.pack('<I', 124))

    # dwFlags (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT | DDSD_LINEARSIZE)
    f.write(struct.pack('<I', 0x1 | 0x2 | 0x4 | 0x1000 | 0x20000 | 0x80000))

    # dwHeight, dwWidth
    f.write(struct.pack('<I', height))
    f.write(struct.pack('<I', width))

    # dwPitchOrLinearSize (for uncompressed, pitch = width * bytes_per_pixel)
    pitch = width * 4
    f.write(struct.pack('<I', pitch))

    # dwDepth
    f.write(struct.pack('<I', 0))

    # dwMipMapCount
    f.write(struct.pack('<I', 1))

    # dwReserved1[11]
    f.write(b'\x00' * 44)

    # DDS_PIXELFORMAT
    # dwSize
    f.write(struct.pack('<I', 32))

    # dwFlags (DDPF_RGB | DDPF_ALPHAPIXELS for RGBA)
    f.write(struct.pack('<I', 0x40 | 0x1))

    # dwFourCC (0 for uncompressed)
    f.write(struct.pack('<I', 0))

    # dwRGBBitCount
    f.write(struct.pack('<I', 32))

    # dwRBitMask, dwGBitMask, dwBBitMask, dwABitMask
    f.write(struct.pack('<I', 0x00FF0000))  # R
    f.write(struct.pack('<I', 0x0000FF00))  # G
    f.write(struct.pack('<I', 0x000000FF))  # B
    f.write(struct.pack('<I', 0xFF000000))  # A

    # dwCaps
    f.write(struct.pack('<I', 0x1000))  # DDSCAPS_TEXTURE

    # dwCaps2, dwCaps3, dwCaps4
    f.write(struct.pack('<I', 0))
    f.write(struct.pack('<I', 0))
    f.write(struct.pack('<I', 0))

    # dwReserved2
    f.write(struct.pack('<I', 0))

def png_to_dds(png_path, dds_path):
    """Convert PNG to uncompressed DDS format."""
    # Load PNG
    img = Image.open(png_path)

    # Convert to RGBA if needed
    if img.mode != 'RGBA':
        img = img.convert('RGBA')

    width, height = img.size

    # Get pixel data as BGRA (DDS uses BGRA order)
    pixels = img.tobytes()

    # Convert RGBA to BGRA
    bgra_pixels = bytearray()
    for i in range(0, len(pixels), 4):
        r, g, b, a = pixels[i:i+4]
        bgra_pixels.extend([b, g, r, a])

    # Write DDS file
    with open(dds_path, 'wb') as f:
        write_dds_header(f, width, height, has_alpha=True)
        f.write(bytes(bgra_pixels))

    print(f"Converted {os.path.basename(png_path)} -> {os.path.basename(dds_path)}")

def main():
    input_dir = "../art/Finals"

    for i in range(1, 10):
        png_path = os.path.join(input_dir, f"template{i}.png")
        dds_path = os.path.join(input_dir, f"template{i}.dds")

        if os.path.exists(png_path):
            png_to_dds(png_path, dds_path)
        else:
            print(f"Warning: {png_path} not found")

    print("\nConversion complete! Helmet icons are ready for use.")

if __name__ == "__main__":
    main()
