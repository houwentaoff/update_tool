DIM ws 
i=1
do
set ws=Wscript.CreateObject("WScript.Shell")
WScript.Sleep 10000
ws.Run "fiwatchdogUpdate.bat", 0, TRUE
loop until i=2
