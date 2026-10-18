Get-ChildItem "assets\*.png" | ForEach-Object {
    $cleanName = $_.BaseName.ToLower().Replace(" ", "_") + ".bmp"
    # Combines the original folder path with the new filename
    $outputPath = Join-Path $_.DirectoryName $cleanName
    
    magick $_.FullName -define bmp:format=bmp3 -define bmp3:alpha=true $outputPath
}
