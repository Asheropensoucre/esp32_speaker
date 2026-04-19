#!/usr/bin/env python3
"""
Convert MP3 audio files to C header file with audio data arrays using ffmpeg.
Converts audio to: Mono, 22050 Hz, 16-bit PCM (s16le)
Requires: ffmpeg (command-line tool)
Uses only standard library: os, subprocess
"""

import os
import subprocess
import sys

# Audio file mappings: (input_filename, array_name)
AUDIO_FILES = [
    ("on.mp3", "sound_boot"),
    ("Connected.mp3", "sound_connected"),
    ("Disconnected.mp3", "sound_disconnected"),
    ("Max Volume - Error Sound.mp3", "sound_error"),
]

# Target audio format via ffmpeg
SAMPLE_RATE = 22050
CHANNELS = 1
BIT_DEPTH = 16


def find_test_directory():
    """
    Find the test directory by checking common locations.
    """
    # Check if test directory exists in current directory
    if os.path.exists("test") and os.path.isdir("test"):
        return os.path.abspath("test")

    # Check if we're already in the test directory
    if os.path.basename(os.getcwd()) == "test":
        return os.getcwd()

    # Check if test directory exists in parent directory
    if os.path.exists(os.path.join("..", "test")) and os.path.isdir(
        os.path.join("..", "test")
    ):
        return os.path.abspath(os.path.join("..", "test"))

    # Check common project structure
    script_dir = os.path.dirname(os.path.abspath(__file__))
    if os.path.exists(os.path.join(script_dir, "test")):
        return os.path.join(script_dir, "test")

    return None


def convert_mp3_with_ffmpeg(mp3_path):
    """
    Convert MP3 file to mono, 16000 Hz, 16-bit PCM using ffmpeg.
    Returns raw bytes.
    """
    print(f"  Loading: {os.path.basename(mp3_path)}")

    try:
        # Run ffmpeg command to convert audio
        # ffmpeg -i input.mp3 -f s16le -ac 1 -ar 16000 -
        # -f s16le: output format is signed 16-bit little-endian
        # -ac 1: 1 audio channel (mono)
        # -ar 22050: 22050 Hz sample rate
        # -: output to stdout (raw binary data)
        result = subprocess.run(
            [
                "ffmpeg",
                "-i",
                mp3_path,
                "-f",
                "s16le",
                "-ac",
                "1",
                "-ar",
                "22050",
                "-",
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )

        if result.returncode != 0:
            error_msg = result.stderr.decode("utf-8", errors="ignore")
            print(f"    ✗ ffmpeg error: {error_msg[:200]}")
            return None

        raw_data = result.stdout
        samples = len(raw_data) // 2
        duration_s = samples / SAMPLE_RATE
        print(f"    ✓ {len(raw_data)} bytes ({samples} samples, {duration_s:.2f}s)")

        return raw_data

    except FileNotFoundError:
        print("    ✗ Error: ffmpeg not found. Please install ffmpeg.")
        return None
    except Exception as e:
        print(f"    ✗ Error: {e}")
        return None


def bytes_to_hex_array(data, line_width=16):
    """
    Convert bytes to hexadecimal string suitable for C array.
    """
    hex_lines = []
    for i in range(0, len(data), line_width):
        chunk = data[i : i + line_width]
        hex_values = ", ".join(f"0x{b:02x}" for b in chunk)
        hex_lines.append(hex_values)

    return ",\n    ".join(hex_lines)


def main():
    print("MP3 to C Header Audio Converter (ffmpeg)")
    print("=" * 50)
    print(f"Target Format: Mono, {SAMPLE_RATE} Hz, {BIT_DEPTH}-bit PCM\n")

    # Find test directory
    test_dir = find_test_directory()

    if not test_dir:
        print("Error: Could not find test directory")
        print(
            "Please run this script from the project root or from within the test/ directory"
        )
        return False

    print(f"Test directory: {test_dir}\n")

    # Dictionary to store converted audio data
    audio_data = {}

    # Convert each MP3 file
    for input_filename, array_name in AUDIO_FILES:
        input_path = os.path.join(test_dir, input_filename)

        if not os.path.exists(input_path):
            print(f"⚠ {input_filename} not found")
            continue

        print(f"Converting: {input_filename}")
        raw_data = convert_mp3_with_ffmpeg(input_path)

        if raw_data is not None:
            audio_data[array_name] = raw_data
        else:
            print(f"✗ Failed to convert {input_filename}\n")
            return False

    if not audio_data:
        print("Error: No audio files were successfully converted")
        return False

    # Generate C header file
    print(f"\nGenerating audio_data.h...")

    output_path = os.path.join(test_dir, "audio_data.h")

    with open(output_path, "w") as f:
        # Write header guard and includes
        f.write("#ifndef AUDIO_DATA_H\n")
        f.write("#define AUDIO_DATA_H\n\n")
        f.write("#include <Arduino.h>\n\n")

        # Write comment about audio specifications
        f.write(
            f"// Audio data converted to: Mono, {SAMPLE_RATE} Hz, {BIT_DEPTH}-bit PCM\n"
        )
        f.write("// Auto-generated by convert_audio.py using ffmpeg\n\n")

        # Write audio data arrays and sizes
        for array_name, data in audio_data.items():
            # Write array size constant
            f.write(f"const unsigned int {array_name}_LENGTH = {len(data)};\n\n")

            # Write array data
            f.write(f"const unsigned char {array_name}[] PROGMEM = {{\n    ")
            f.write(bytes_to_hex_array(data))
            f.write("\n};\n\n")

        # Write audio configuration
        f.write("// Audio configuration\n")
        f.write(f"#define AUDIO_SAMPLE_RATE {SAMPLE_RATE}\n")
        f.write(f"#define AUDIO_CHANNELS {CHANNELS}\n")
        f.write(f"#define AUDIO_BIT_DEPTH {BIT_DEPTH}\n\n")

        # Write footer
        f.write("#endif // AUDIO_DATA_H\n")

    print(f"✓ Successfully generated: {output_path}\n")
    print("Generated audio constants:")
    for array_name, data in audio_data.items():
        samples = len(data) // 2
        duration_s = samples / SAMPLE_RATE
        print(f"  - {array_name}")
        print(f"      Length: {array_name}_LENGTH = {len(data)}")
        print(f"      Samples: {samples}")
        print(f"      Duration: {duration_s:.2f}s\n")

    return True


if __name__ == "__main__":
    try:
        success = main()
        sys.exit(0 if success else 1)
    except KeyboardInterrupt:
        print("\n\nCancelled by user")
        sys.exit(1)
