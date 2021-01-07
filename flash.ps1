<<<<<<< HEAD
﻿Remove-Item $env:ubuntu_home\Documents\MultimediaChat\ -Recurse -ErrorAction ignore
New-Item "$env:ubuntu_home\Documents\MultimediaChat" -Itemtype directory
$Source	= "$env:HOMEPATH\Documents\Projects\Progetti VS\MultimediaChat\*"
$Dest	= "$env:ubuntu_home\Documents\MultimediaChat"
$Exclude= @(".vs", ".vscode")
Copy-Item -Path $Source -Destination $Dest -Exclude $Exclude -Recurse -Force
=======
﻿Remove-Item $env:ubuntu_home\Documents\MultimediaChat -Recurse -ErrorAction Ignore
Copy-Item "$env:HOMEPATH\Documents\Projects\Progetti VS\MultimediaChat" -Destination $env:ubuntu_home\Documents\ -Recurse
>>>>>>> 857cfa0d7bbe44fd74c36df1f5cee4da5cb0ece9
