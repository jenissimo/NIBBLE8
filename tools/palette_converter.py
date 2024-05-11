import os
import argparse
import urllib.request

def read_palette_file(file_path):
    with open(file_path, 'r') as f:
        return [line.strip() for line in f]

def read_palette_from_url(url):
    with urllib.request.urlopen(url) as response:
        return [line.decode('utf-8').strip() for line in response.readlines()]

def luminance(rgb):
    r, g, b = rgb
    return 0.2126 * r + 0.7152 * g + 0.0722 * b

def convert_palette(palette, file_name):
    base_name = os.path.splitext(os.path.basename(file_name))[0]
    output = [f'[{base_name}]']

    colors = []

    for line in palette:
        if line.startswith(';') or not line.strip():
            continue  # Skip comments and empty lines
        color_hex = line.strip()
        if color_hex.startswith('FF'):
            r = int(color_hex[2:4], 16)
            g = int(color_hex[4:6], 16)
            b = int(color_hex[6:8], 16)
            colors.append((r, g, b))

    sorted_colors = sorted(colors, key=luminance)

    for i, color in enumerate(sorted_colors, start=1):
        r, g, b = color
        output.append(f'Color{i}={r},{g},{b}')

    return output

def main():
    parser = argparse.ArgumentParser(description='Convert color palette from hexadecimal to RGB format.')
    parser.add_argument('palette_file', nargs='?', type=str, help='File or URL containing color palette in hexadecimal format')

    args = parser.parse_args()

    if args.palette_file:
        if args.palette_file.startswith('http://') or args.palette_file.startswith('https://'):
            palette = read_palette_from_url(args.palette_file)
        else:
            palette = read_palette_file(args.palette_file)

        output = convert_palette(palette, args.palette_file)

        for line in output:
            print(line)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
