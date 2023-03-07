::***************************************************************************
:: add_sha1.bat:  Batch file to execute the bash script add_sha1.sh to
:: 	write the current Git SHA1 id to a text file
::
:: Written by:
:: 	Sandra Mercer
:: 	Engineer / Software Developer
:: 	eSonar Inc.
::
:: Date:
:: 	November 2014
::***************************************************************************

%windir%\system32\cmd.exe /c ""C:\Program Files\Git\bin\sh.exe" --login -i" add_sha1.sh

