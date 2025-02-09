
import pandas as pd
import matplotlib.pyplot as plt
import os
import numpy as np
import sys

# Get CSV file path and output directory from arguments
if len(sys.argv) < 3:
    print("Usage: python script.py <csv_file> <output_directory>")
    sys.exit(1)

csv_file = sys.argv[1]
output_dir = sys.argv[2]

# Ensure output directory exists
os.makedirs(output_dir, exist_ok=True)

# Read CSV file
df = pd.read_csv(csv_file, delimiter=";")  

# Group data by TraitFolderID
trait_groups = df.groupby("TraitFolderID")

# Define Excel output path
excel_filename = os.path.join(output_dir, "traits_with_charts.xlsx")
writer = pd.ExcelWriter(excel_filename, engine="openpyxl")

# Save the raw data as the first sheet
df.to_excel(writer, sheet_name="Trait Data", index=False)

# Folder for storing charts inside the output directory
chart_folder = os.path.join(output_dir, "charts")
os.makedirs(chart_folder, exist_ok=True)

# Iterate through each trait group
for folder_id, group in trait_groups:
    # Extract TraitID and Counts, sorted by descending count
    group = group.sort_values(by="Count", ascending=False)
    labels = group["TraitID"]
    counts = group["Count"]
    num_traits = len(labels)

    # Determine dynamic figure size based on number of traits
    fig_width = max(6, min(12, num_traits * 0.4))  # Scale width dynamically
    fig_height = max(4, min(8, num_traits * 0.25))  # Scale height dynamically
    
    # Font sizes remain unchanged
    font_size = max(8, min(14, 12 - num_traits * 0.1))
    label_font_size = max(6, min(12, 10 - num_traits * 0.1))

    # Create figure with dynamic size
    plt.figure(figsize=(fig_width, fig_height))

    # Generate color map
    colors = plt.cm.viridis(np.linspace(0, 1, len(labels)))

    # Create a horizontal bar chart
    bars = plt.barh(labels, counts, color=colors, edgecolor='black')
    plt.xlabel("Count", fontsize=font_size, fontweight='bold')
    plt.ylabel("Trait", fontsize=font_size, fontweight='bold')
    plt.title(f"Trait Distribution: {folder_id}", fontsize=font_size + 2, fontweight='bold')
    plt.gca().invert_yaxis()  # Flip order so highest count is on top
    
    # Add count labels to the right of each bar
    for bar, count in zip(bars, counts):
        plt.text(bar.get_width() + 0.5, bar.get_y() + bar.get_height()/2, str(count), 
                 fontsize=label_font_size, verticalalignment='center')
    
    plt.tight_layout()

    # Save chart as an image
    chart_filename = os.path.join(chart_folder, f"{folder_id}.png")
    plt.savefig(chart_filename, bbox_inches="tight", dpi=300)
    plt.close()

    # Create a new sheet for each category and insert only the image
    sheet_name = str(folder_id)[:31]  # Excel sheet name limit
    wb = writer.book
    ws = wb.create_sheet(title=sheet_name)

    # Insert image into the sheet at the top-left corner (A1)
    from openpyxl.drawing.image import Image
    img = Image(chart_filename)
    img.width = img.width // 2  # Reduce image width
    img.height = img.height // 2  # Reduce image height
    ws.add_image(img, "A1")  # Insert image at cell A1

# Save final Excel file
writer.close()
print(f"Excel file saved at: {excel_filename}")
