#!/usr/bin/env python3
"""
Convert sprite-sheet-main.png to raw RGB565 C arrays for Sir Potato OS

This script:
1. Loads the sprite sheet PNG
2. Slices it into 16 frames (120x120 each)
3. Converts colors to Big-Endian RGB565 format
4. Outputs flat C arrays for Assets.h (no RLE compression)
"""

from PIL import Image
import sys

def rgb_to_rgb565(r, g, b):
    """Convert RGB (0-255) to RGB565 (16-bit, Big-Endian)"""
    # Convert to 5-6-5 bit format
    r5 = (r >> 3) & 0x1F
    g6 = (g >> 2) & 0x3F
    b5 = (b >> 3) & 0x1F
    
    # Combine into 16-bit value (big-endian for TFT_eSPI)
    return (r5 << 11) | (g6 << 5) | b5

def process_sprite_sheet(input_file, output_file):
    """Process the sprite sheet and generate C arrays"""
    
    try:
        # Load the image
        img = Image.open(input_file)
        print(f"Loaded image: {img.size} pixels, mode: {img.mode}")
        
        # Convert to RGB if needed
        if img.mode != 'RGB':
            img = img.convert('RGB')
        
        # Get dimensions
        img_width, img_height = img.size
        
        # Calculate frame dimensions (assuming 4x4 grid for 16 frames)
        frame_width = img_width // 4
        frame_height = img_height // 4
        
        print(f"Original frame size: {frame_width}x{frame_height}")
        
        # Resize frames to 120x120 for our display
        target_width = 120
        target_height = 120
        print(f"Target frame size: {target_width}x{target_height}")
        print(f"Total frames: 16")
        print(f"Pixels per frame: {target_width * target_height}")
        print(f"Bytes per frame: {target_width * target_height * 2}")
        
        # Open output file
        with open(output_file, 'w') as f:
            f.write("// Auto-generated raw RGB565 sprite data\n")
            f.write("// Generated from sprite-sheet-main.png\n")
            f.write("// Format: Flat RGB565 arrays (no compression)\n")
            f.write("// Each frame: 120x120 = 14400 pixels = 28800 bytes\n\n")
            
            # Process each frame
            for frame_idx in range(16):
                # Calculate frame position (4x4 grid)
                row = frame_idx // 4
                col = frame_idx % 4
                
                x = col * frame_width
                y = row * frame_height
                
                # Extract frame
                frame = img.crop((x, y, x + frame_width, y + frame_height))
                
                # Resize to target dimensions
                frame = frame.resize((target_width, target_height), Image.LANCZOS)
                
                # Convert to RGB565
                pixels = []
                for py in range(target_height):
                    for px in range(target_width):
                        r, g, b = frame.getpixel((px, py))
                        rgb565 = rgb_to_rgb565(r, g, b)
                        pixels.append(rgb565)
                
                print(f"Frame {frame_idx}: {len(pixels)} pixels ({len(pixels) * 2} bytes)")
                
                # Write C array
                f.write(f"// Frame {frame_idx}: 120x120 RGB565\n")
                f.write(f"const uint16_t spriteFrame{frame_idx}[14400] PROGMEM = {{\n")
                
                # Write pixel data (12 values per line for readability)
                line_length = 0
                for pixel in pixels:
                    if line_length >= 12:
                        f.write("\n")
                        line_length = 0
                    
                    f.write(f"0x{pixel:04X}, ")
                    line_length += 1
                
                f.write("\n};\n\n")
            
            # Write sprite sheet array
            f.write("// Sprite sheet array (pointers to each frame)\n")
            f.write("const uint16_t* spriteSheet[16] PROGMEM = {\n")
            for i in range(16):
                f.write(f"  spriteFrame{i},\n")
            f.write("};\n")
        
        print(f"\n✅ Success! Output written to: {output_file}")
        print(f"📊 Total frames: 16")
        print(f"📏 Frame size: {target_width}x{target_height}")
        print(f"💾 Total size: {target_width * target_height * 2 * 16} bytes")
        
    except FileNotFoundError:
        print(f"❌ Error: File not found: {input_file}")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    input_file = "test/sprite-sheet-main.png"
    output_file = "test/sprite_data.h"
    
    print("🥔 Sir Potato OS - Sprite Sheet Converter")
    print("=" * 50)
    print(f"Input:  {input_file}")
    print(f"Output: {output_file}")
    print("=" * 50)
    
    process_sprite_sheet(input_file, output_file)
