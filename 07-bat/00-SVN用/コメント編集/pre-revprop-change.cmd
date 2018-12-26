if "%5"=="M" (
  if "%4"=="svn:log" (
    if "%3"=="aau88010" (
      exit 0
    )
    if "%3"=="aau08189" (
      exit 0
    )
  )
)
echo "Changing revision properties other than svn:log is prohibited" >&2
exit 1