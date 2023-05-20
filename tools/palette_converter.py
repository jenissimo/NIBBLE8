import os
import argparse

def read_palette_file(file_path):
    with open(file_path, 'r') as f:
        return [line.strip() for line in f]

def convert_palette(palette, file_name):
    base_name = os.path.splitext(os.path.basename(file_name))[0]
    output = [f'[{base_name}]']

    for i, color in enumerate(palette, start=1):
        r = int(color[0:2], 16)
        g = int(color[2:4], 16)
        b = int(color[4:6], 16)
        output.append(f'Color{i}={r},{g},{b}')

    return output

def main():
    parser = argparse.ArgumentParser(description='Convert color palette from hexadecimal to RGB format.')
    parser.add_argument('palette_file', type=str, help='File containing color palette in hexadecimal format')

    args = parser.parse_args()

    palette = read_palette_file(args.palette_file)
    output = convert_palette(palette, args.palette_file)

    for line in output:
        print(line)

if __name__ == "__main__":
    main()