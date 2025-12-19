from PIL import Image

def process_background():
    input_path = "Paper/background.png"
    output_path = "Paper/background_content.png"
    
    try:
        img = Image.open(input_path).convert("RGBA")
        width, height = img.size
        
        # Determine cut-off (Top 20% usually covers the header banner)
        # The user wants to remove the upper part (header) but keep the footer.
        # We will make the top 18% transparent.
        cut_height = int(height * 0.18)
        
        # Create a new image with transparent top
        # Or just paste a transparent block?
        # Actually simplest is to make pixel data modification or compositing.
        
        # Create a blank (transparent) image of same size
        new_img = Image.new("RGBA", (width, height), (0, 0, 0, 0))
        
        # Crop the bottom part of the original image
        # crop box: (left, upper, right, lower)
        bottom_part = img.crop((0, cut_height, width, height))
        
        # Paste it into the new image at the correct offset
        new_img.paste(bottom_part, (0, cut_height))
        
        # Save
        new_img.save(output_path, "PNG")
        print(f"Created {output_path} with top {cut_height}px removed.")
        
    except Exception as e:
        print(f"Error processing image: {e}")

if __name__ == "__main__":
    process_background()
