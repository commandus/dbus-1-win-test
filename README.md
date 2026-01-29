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

### Check method expose

Check tree

```shell
busctl tree com.commandus.greeting
```

```shell
busctl introspect com.commandus.greeting /com/commandus/greeting
NAME                   TYPE      SIGNATURE RESULT/VALUE FLAGS
com.commandus.greeting interface -         -            -
.hello                 method    s         -            -
```

Call method from the console:

Does not work

```shell
dbus-send --system --print-reply --type=method_call /com/example/Object com.commandus.greeting.hello string:"hello world"
```

Works

```shell
busctl call com.commandus.greeting /com/commandus/greeting com.commandus.greeting hello s "color"
```

### Assign access rights to the service

```
org.freedesktop.DBus.Error.AccessDenied Connection ":1.39" is not allowed to own the service "hello.response.service" due to security policies in the configuration file
```

```shell
sudo vi /etc/dbus-1/system.d/com.commandus.greeting.conf
```

Enter

```xml
<?xml version="1.0" encoding="UTF-8"?> <!-- -*- XML -*- -->
<!DOCTYPE busconfig PUBLIC "-//freedesktop//DTD D-BUS Bus Configuration 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/busconfig.dtd">
<busconfig>
  <policy user="root">
    <allow own="org.freedesktop.PackageKit"/>
  </policy>
  <policy context="default">
    <allow send_destination="com.commandus.greeting" send_interface="com.commandus.greeting"/>
  </policy>
</busconfig>
```

Restart service:

```shell
sudo systemctl restart dbus
```

### Add service file to autostart service

```shell
sudo vi /usr/share/dbus-1/system-services/com.commandus.greeting.service
```

Enter

```
[D-BUS Service]
Name=com.commandus.greeting
Interface=com.commandus.greeting
Exec=/home/andrei/git/dbus-1-win-test/build/dbus-1-win-test
User=andrei
```

Restart service:

```shell
sudo systemctl restart dbus
```

### Put interface file into D-Bus

```shell
sudo vi /usr/share/dbus-1/interfaces/com.commandus.greeting.xml
```

Enter

```
<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
    <interface name="com.commandus.greeting">
    <method name="hello">
        <arg name="your_name" direction="in" type="s"/>
    </method>
    </interface>
</node>
```

Restart service:

```shell
%
```

## Read D-Bus log

```shell
journalctl -u dbus
```

## Code generation

```shell
gdbus-codegen --generate-c-code gen-greeting --c-namespace greetingApp --interface-prefix com.commandus. /usr/share/dbus-1/interfaces/com.commandus.greeting.xml
```

## References

- [A sample code illustrating basic use of D-BUS](https://github.com/fbuihuu/samples-dbus/tree/master)
- [Matthew Johnson. Using the DBUS C API](http://www.matthew.ath.cx/misc/dbus)
