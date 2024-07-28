for dir in $1/*; do
  test -d "$dir" || continue
  echo "$dir"
#  ./gendoc "$dir"
done
