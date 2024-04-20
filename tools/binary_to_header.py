import os
import sys

def binary_to_header_and_source(input_file_path, output_folder, array_name):
    header_file_path = os.path.join(output_folder, f"{array_name}.h")
    source_file_path = os.path.join(output_folder, f"{array_name}.c")
    
    try:
        # Read the binary file
        with open(input_file_path, 'rb') as file:
            binary_data = file.read()
        
        # Convert binary data to a C-compatible array
        hex_array = ', '.join(f'0x{byte:02X}' for byte in binary_data)

        # Prepare the content for the header file
        header_content = f"#ifndef {array_name.upper()}_H\n#define {array_name.upper()}_H\n\n"
        header_content += f"extern unsigned char {array_name.upper()}[];\n"
        header_content += f"extern unsigned int {array_name.upper()}_SIZE;\n\n"
        header_content += f"#endif // {array_name.upper()}_H\n"

        # Prepare the content for the source file
        source_content = f'#include "{os.path.basename(header_file_path)}"\n\n'
        source_content += f"unsigned char {array_name.upper()}[] = {{\n    {hex_array}\n}};\n"
        source_content += f"unsigned int {array_name.upper()}_SIZE = sizeof({array_name.upper()});\n"

        # Write to the header file
        with open(header_file_path, 'w') as file:
            file.write(header_content)
        
        # Write to the source file
        with open(source_file_path, 'w') as file:
            file.write(source_content)
        
        print(f"Header file '{header_file_path}' and source file '{source_file_path}' have been created successfully.")
    
    except Exception as e:
        print(f"An error occurred: {e}")

def main():
    if len(sys.argv) != 4:
        print("Usage: python script.py <input_file> <output_folder> <array_name>")
        return

    input_file_path = sys.argv[1]
    output_folder = sys.argv[2]
    array_name = sys.argv[3]

    binary_to_header_and_source(input_file_path, output_folder, array_name)

if __name__ == "__main__":
    main()
