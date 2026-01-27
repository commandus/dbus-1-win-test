# A D-Bus example that works on Windows and Linux

## Dependencies

### Linux

```shell
sudo apt install libdbus-1-dev
```

### Windowa

Install vcpkg if not already.

Install dbus package via vcpkg

```shell
vcpkg install --triplet x64-windows dbus
```

Provide vcpkg options to CMake:

```
-DCMAKE_TOOLCHAIN_FILE=C:\git\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
```

## Run

### Windows

Set OS environment variable for system bus:

```
DBUS_SYSTEM_BUS_ADDRESS=tcp:host=127.0.0.1,port=12434
```

### Check signal reciever

Uncomment receiveSignals():

```c++
receiveSignals(dbus_conn, &dbus_error);
```

Send signal from console:

```shell
dbus-send --system --type=signal /com/commandus/greeting com.commandus.greeting.GreetingSignal string:"hello world" int32:47
```

### Check signal sender

Run monitor:

```shell
dbus-monitor
```

Uncomment sendSignal()

```c++
sendSignal(dbus_conn, &dbus_error);
```

## References

- [Matthew Johnson. Using the DBUS C API](http://www.matthew.ath.cx/misc/dbus)
