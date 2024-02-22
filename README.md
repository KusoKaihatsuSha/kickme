# KickMe

> Simple application to log off RDP sessions from a list of servers. If you are lazy searching your sessions with `quser` and `powershell`.  

> Warning: Windows Server used only, because app using the standard console commands:
>   `quser`
>   `logoff`


### Notes: 

For build app with GUI use Qt (install first and check ENV qmake and mingw32-make). For correct build Not use '-j16'

```sh
$ mingw32-make clean -j16 & qmake & mingw32-make
```

#### Example txt file:
```
server-RDP-001
server-RDP-002
server-RDP-003
server-RDP-004
server-RDP-005
```

#### Screenshots:

<div style="width:50%">
<img src="/files/image.png" >
</div>