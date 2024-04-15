import sys
from PIL import Image

def png_to_lua_array(image_path):
    # Open the image
    with Image.open(image_path) as img:
        # Ensure the image is in 'P' mode for palette-based (indexed)
        if img.mode != 'P':
            raise ValueError("Image is not indexed (palette-based).")

        # Get pixel data
        pixels = list(img.getdata())

        # Convert pixel data to Lua table format
        lua_array = "{ " + ", ".join(map(str, pixels)) + " }"

        return lua_array

def main():
    if len(sys.argv) != 2:
        print("Usage: python script.py <path_to_indexed_image.png>")
        sys.exit(1)
    
    image_path = sys.argv[1]
    try:
        lua_array = png_to_lua_array(image_path)
        print("Lua array of color indexes:")
        print(lua_array)
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()

