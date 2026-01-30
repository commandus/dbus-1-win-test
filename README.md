# A D-Bus example that works on Windows and Linux

## Dependencies

### Linux

Install dbus-1 library and headers:

```shell
sudo apt install libdbus-1-dev
```

### Windowa

Install vcpkg if not already.

Install dbus package via vcpkg:

```shell
vcpkg install --triplet x64-windows dbus
```

Each time you build project you must provide vcpkg options to the CMake:

```
-DCMAKE_TOOLCHAIN_FILE=C:\git\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
```

## Run

### Windows

On Windows you need install D-Bus binaries from the [WinDbusBinary](https://github.com/WangTingMan/WinDbusBinary).

Please read README.md how to run dbus-daemon.exe.

Run dbus-daemon.exe.

Set OS environment variable for system bus:

```
DBUS_SYSTEM_BUS_ADDRESS=tcp:host=127.0.0.1,port=12434
```

and run compiled example. 

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
.hello                 method    s         s            -
.Version               property  s         "0.01"       emits-change writable```

Call method from the console:

Does not work

```shell
dbus-send --system --print-reply --type=method_call /com/example/Object com.commandus.greeting.hello string:"hello world"
```

It works:

```shell
busctl call com.commandus.greeting /com/commandus/greeting com.commandus.greeting hello s "color"
```

### Assign access rights to the service

In case of error:

```
org.freedesktop.DBus.Error.AccessDenied Connection ":1.39" is not allowed to own the service "hello.response.service" due to security policies in the configuration file
```

provide rights to the service.

Create and edit configuration file com.commandus.greeting.conf:

```shell
sudo vi /etc/dbus-1/system.d/com.commandus.greeting.conf
```

Enter in the com.commandus.greeting.conf file:

```xml
<?xml version="1.0" encoding="UTF-8"?> <!-- -*- XML -*- -->
<!DOCTYPE busconfig PUBLIC "-//freedesktop//DTD D-BUS Bus Configuration 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/busconfig.dtd">
<busconfig>
    <policy user="andrei">
        <allow own="com.commandus.greeting"/>
    </policy>
    <policy context="default">
        <allow send_destination="com.commandus.greeting" send_interface="com.commandus.greeting"/>
        <allow send_destination="com.commandus.greeting" send_interface="org.freedesktop.DBus.Introspectable"/>
        <allow send_destination="com.commandus.greeting" send_interface="org.freedesktop.DBus.Properties"/>
    </policy>
</busconfig>
```

Restart service (usually service automatically reload config after file has been saved):

```shell
sudo systemctl restart dbus
```

### Add service file to autostart service

If service is tom running, D-Bus daemon can start service on demand.

Create and edit configuration file com.commandus.greeting.service:

```shell
sudo vi /usr/share/dbus-1/system-services/com.commandus.greeting.service
```

Enter path to the executable in the configuration file com.commandus.greeting.service:

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

```xml
<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
    <interface name="com.commandus.greeting">
        <method name="hello">
            <arg name="your_name" direction="in" type="s"/>
        </method>
        <property name="Version" type="s" access="read">
        </property>
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

- [Windows D-bus binary](https://github.com/WangTingMan/WinDbusBinary)
- [A sample code illustrating basic use of D-BUS](https://github.com/fbuihuu/samples-dbus/tree/master)
- [Matthew Johnson. Using the DBUS C API](http://www.matthew.ath.cx/misc/dbus)
