@echo off
setlocal enabledelayedexpansion

set yyyymmdd=%date:~0,4%%date:~5,2%%date:~8,2% 

md "%1\%yyyymmdd% doc"
md "%1\%yyyymmdd% xls"
md "%1\%yyyymmdd% ppt"

for %%f in (%1\*) do (
  set "extension=%%~xf"
  if !extension!==.doc move /y %%f "%1\%yyyymmdd% doc"
  if !extension!==.xls move /y %%f "%1\%yyyymmdd% xls"
  if !extension!==.ppt move /y %%f "%1\%yyyymmdd% ppt"
)