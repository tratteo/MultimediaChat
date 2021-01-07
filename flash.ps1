Remove-Item $env:ubuntu_home\Documents\MultimediaChat\ -Recurse -ErrorAction ignore
New-Item "$env:ubuntu_home\Documents\MultimediaChat" -Itemtype directory
$Source	= "$env:HOMEPATH\Documents\Projects\Progetti VS\MultimediaChat\*"
$Dest	= "$env:ubuntu_home\Documents\MultimediaChat"
$Exclude= @(".vs", ".vscode")
Copy-Item -Path $Source -Destination $Dest -Exclude $Exclude -Recurse -Force