@echo off
findstr /m /i "%2" *.%1 > output.txt
tar -a -cf output.zip output.txt
del output.txt