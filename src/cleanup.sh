find . -type f \( -name '*.c' -o -name '*.h' \) -print0 |
while IFS= read -r -d '' f; do
    sed -E 's/^( {4})+/\t/g' "$f" > "$f.tmp" && mv "$f.tmp" "$f"
done
