for f in assets/*.png; do
  # Replace spaces with underscores
  clean_name="${f// /_}"
  # Convert to lowercase and change extension to .bmp
  out_name="${clean_name,,}"
  out_name="${out_name%.png}.bmp"
  
  magick "$f" "$out_name"
done
