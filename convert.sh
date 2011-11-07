#/bin/bash
find ./  -type f -name "*.[Bb][Mm][Pp]" | while read name; 
do
	new_name=$(echo -n "${name%.*}");
	if [ x = x$1 ]; then
		convert "$name" "$new_name".jpg;
	else
		convert "$name" "$new_name".$1;
	fi
	rm -f "$name"
done
