$patt = "gregory.sedykh.noah.munz.tp"

rm rendus -Recurse -ErrorAction SilentlyContinue
rm "$patt*" -Recurse -ErrorAction SilentlyContinue

for ($i = 2; $i -lt 8; $i += 1) {
	$dest = "$patt$i"
	$archName = "$dest.zip"
	$src = "TP0$i"
	$destCode = "$dest/$src"
	
	mkdir $dest > $null; mkdir $destCode > $null
	
	cp $src/* $destCode  #NB: No recurse
	$toDel =".*", "Slides", "instructions*", "*.json", "*test*", "example*"
	$toDel | % { $nm = $_; rm "$destCode/$nm" -Recurse -ErrorAction SilentlyContinue }

	cp $src/out/* $destCode/out -Recurse
	
	if (Test-Path "$src/res") { cp "$src/res/*" $destCode/res/ -Recurse }
	
	
	cp "Slides/$src*.pdf" $dest/
	cp "$src/instructions-report/*rapport*.pdf" $dest/ 

	zip $dest $archName #WARN: adding pdfs directly doesnt work smh
}

mkdir rendus -ErrorAction SilentlyContinue

mv "$patt*" rendus/ -Exclude "*.zip" -ErrorAction SilentlyContinue

