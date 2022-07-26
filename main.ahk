#MaxThreadsPerHotkey 5
#MaxHotKeysPerInterval 100
#SingleInstance,Force 
Looping:=1
Start:=A_TickCount
while(1)
{     
     mousegetpos,x,y  
     tooltip, %1%,x+20,y+20,1
     If GetKeyState("RButton") 
     {
	Looping:=!Looping
     }
     If(!Looping)
     {
	Break
     }
     If((A_TickCount-Start)>10000) 
     {
        Looping:=0
	ExitApp
     }
}
tooltip

