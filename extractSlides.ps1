ls "*0?.zip" | % {
	$name = $_.Name
	$nameNoExt = $name.replace(".zip", "")
	$num = $nameNoExt.Substring($nameNoExt.Length - 2)
	$foldName = "TP$num"
	$slideFoldName = "$foldName/Slides"
	if (Test-Path $slideFoldName) {
		rm $slideFoldName -Recurse -Force
	}
	unzip $_ "./$foldName/Slides" -Force
}
