# ushortid-server
2-byte / 8-byte address exchange server (plus GA routung rules) in OpenSim

## Platform
**Linux Only**  
Windows will **not work**(due to header files)  
**untested** in Mac OS X

## Compile

```
make
```
*That is.*

## Run

```
./svr [root mote addr1] [root mote addr2] [root mote addr3] ...
```

e.g.  

```
./svr 14:15:92:cc:00:00:00:01
```
