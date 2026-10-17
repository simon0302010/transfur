if command -v magick >/dev/null 2>&1; then
  IM=magick
elif command -v convert >/dev/null 2>&1; then
  IM=convert
else
  echo "ImageMagick not found" >&2
  exit 1
fi

for f in assets/*.png; do
  # Replace spaces with underscores
  clean_name="${f// /_}"
  # Convert to lowercase and change extension to .bmp
  out_name="${clean_name,,}"
  out_name="${out_name%.png}.bmp"

  "$IM" "$f" "$out_name"
done
