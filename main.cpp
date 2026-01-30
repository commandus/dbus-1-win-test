#include <iostream>
#include <cstring>
#include <dbus/dbus.h>

#ifdef _MSC_VER
#include <Windows.h>
#define sleep(x) _sleep(x)
#else
#include <unistd.h>
#endif

const char* DBUS_INTF_NAME = "com.commandus.greeting";

static int callMethod(
    DBusConnection *dbus_conn,
    DBusError *dbus_error
) {
    // Compose remote procedure call
    DBusMessage *dbus_msg = dbus_message_new_method_call("org.freedesktop.DBus", "/",
        "org.freedesktop.DBus.Introspectable", "Introspect");
    if (!dbus_msg) {
        dbus_connection_unref(dbus_conn);
        std::cerr << "Unable to allocate memory for the message" << std::endl;
        exit(-2);
    }

    // Invoke remote procedure call, block for response
    DBusMessage *dbus_reply = dbus_connection_send_with_reply_and_block(dbus_conn, dbus_msg, DBUS_TIMEOUT_USE_DEFAULT,
                                                                        dbus_error);
    if (!dbus_reply) {
        dbus_message_unref(dbus_msg);
        dbus_connection_unref(dbus_conn);
        std::cerr << dbus_error->name << " " << dbus_error->message << std::endl;
        exit(-3);
    }
    const char *dbus_result = nullptr;
    // Parse response
    if (!dbus_message_get_args(dbus_reply, dbus_error, DBUS_TYPE_STRING, &dbus_result, DBUS_TYPE_INVALID)) {
        dbus_message_unref(dbus_msg);
        dbus_message_unref(dbus_reply);
        dbus_connection_unref(dbus_conn);
        std::cerr << dbus_error->name << " " << dbus_error->message << std::endl;
        exit(-3);
    }
    std::cout << std::endl << dbus_result << std::endl << std::endl;
    dbus_message_unref(dbus_msg);
    dbus_message_unref(dbus_reply);
    return 0;
}

static int sendSignal(
    DBusConnection *conn,
    DBusError *dbus_error
) {
    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage* msg;
    DBusMessageIter args;

    // create a signal and check for errors
    msg = dbus_message_new_signal("/com/example/Object", DBUS_INTF_NAME, "GreetingSignal");
    if (!msg) {
        std::cerr << "Message Null\n";
        exit(-4);
    }

    // append arguments onto signal
    dbus_message_iter_init_append(msg, &args);
    const char *sigvalue = "sssiggal sstring val";
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &sigvalue)) {
        std::cerr << "Out Of Memory\n";
        exit(-5);
    }
    // send the message and flush the connection
    if (!dbus_connection_send(conn, msg, &serial)) {
        std::cerr << "Out Of Memory\n";
        exit(-6);
    }
    dbus_connection_flush(conn);
    // free the message
    dbus_message_unref(msg);
    return 0;
}

static int receiveSignals(
    DBusConnection *conn,
    DBusError *err
) {
    dbus_bus_add_match(conn, "type='signal',interface='com.commandus.greeting',member='GreetingSignal'", err);
    dbus_connection_flush(conn);

    while (1) {
        dbus_connection_read_write(conn, 0);
        DBusMessage *msg = dbus_connection_pop_message(conn);

        if (!msg) {
            sleep(1);
            continue;
        }

        if (dbus_message_is_signal(msg, DBUS_INTF_NAME, "GreetingSignal")) {
            DBusMessageIter args;
            std::cout << "Received signal: 'GreetingSignal'\n";
            if (!dbus_message_iter_init(msg, &args)) {

            } else if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_STRING) {
                char *value;
                dbus_message_iter_get_basic(&args, &value);
                std::cout <<  "Signal payload (string): " << value << std::endl;
            }
        }
        dbus_message_unref(msg);
    }
    return 0;
}

void reply_to_method_call_1(
    DBusMessage* msg,
    DBusConnection* conn,
    DBusMessage* reply
)
{
    char* param = nullptr;

    // read the arguments
    DBusMessageIter args;
    if (dbus_message_iter_init(msg, &args))
        if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_STRING)
            dbus_message_iter_get_basic(&args, &param);

    // add the arguments to the reply
    dbus_message_iter_init_append(reply, &args);
    const char *answer = "World";
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &answer)) {
        exit(1);
    }
}

static const char *server_introspection_xml = {
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
    "<node>\n"
    "    <interface name=\"com.commandus.greeting\">\n"
    "        <method name=\"hello\">\n"
    "            <arg name=\"your_name\" direction=\"in\" type=\"s\"/>\n"
    "            <arg name=\"retval\" direction=\"out\" type=\"s\"/>\n"
    "        </method>\n"
    "        <property name=\"Version\" type=\"s\" access=\"readwrite\" />\n"
    "    </interface>\n"
    "</node>"
};

static DBusHandlerResult server_get_properties_handler(
    const char *property,
    DBusConnection *conn,
    DBusMessage *reply
)
{
    const char *version = "0.01";
    if (!strcmp(property, "Version"))
        dbus_message_append_args(reply, DBUS_TYPE_STRING, &version, DBUS_TYPE_INVALID);
    else
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult server_get_all_properties_handler(
    DBusConnection *conn,
    DBusMessage *reply
)
{
    DBusHandlerResult result;
    DBusMessageIter array, dict, iter, variant;
    const char *property = "Version";
    result = DBUS_HANDLER_RESULT_NEED_MEMORY;

    dbus_message_iter_init_append(reply, &iter);
    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &array);

    property = "Version";
    dbus_message_iter_open_container(&array, DBUS_TYPE_DICT_ENTRY, nullptr, &dict);
    dbus_message_iter_append_basic(&dict, DBUS_TYPE_STRING, &property);
    dbus_message_iter_open_container(&dict, DBUS_TYPE_VARIANT, "s", &variant);
    const char *version = "0.01";
    dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &version);
    dbus_message_iter_close_container(&dict, &variant);
    dbus_message_iter_close_container(&array, &dict);
    dbus_message_iter_close_container(&iter, &array);
    return DBUS_HANDLER_RESULT_HANDLED;
}

static int exposeMethod1(
    DBusConnection *conn,
    DBusError *err
) {
    dbus_bus_request_name(conn, DBUS_INTF_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING , err);
    if (dbus_error_is_set(err))
        return -6;
    dbus_connection_flush(conn);
    
    while (true) {
        // non blocking read of the next available message
        dbus_connection_read_write(conn, 0);
        DBusMessage *msg = dbus_connection_pop_message(conn);

        // loop again if we haven't got a message
        if (!msg) {
            sleep(1);
            continue;
        }
        DBusMessage *reply = nullptr;

        // check this is a method call for the right interface and method
        if (dbus_message_is_method_call(msg, DBUS_INTF_NAME, "hello")) {
            if (!(reply = dbus_message_new_method_return(msg))) {
            }
            reply_to_method_call_1(msg, conn, reply);
        }

        if (dbus_message_is_method_call(msg, DBUS_INTERFACE_INTROSPECTABLE, "Introspect")) {
            std::cout << "Introspect " << std::endl;
            if (!(reply = dbus_message_new_method_return(msg))) {
            }
            dbus_message_append_args(reply, DBUS_TYPE_STRING, &server_introspection_xml, DBUS_TYPE_INVALID);
        }

        if (dbus_message_is_method_call(msg, DBUS_INTERFACE_PROPERTIES, "Get")) {
            std::cout << "Get properties" << std::endl;
            const char *intface, *property;
            if (!dbus_message_get_args(msg, err, DBUS_TYPE_STRING, &intface, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID)) {
            }
            if (!(reply = dbus_message_new_method_return(msg))) {
            }
            auto result = server_get_properties_handler(property, conn, reply);
            if (result != DBUS_HANDLER_RESULT_HANDLED) {

            }
        }
        if (dbus_message_is_method_call(msg, DBUS_INTERFACE_PROPERTIES, "GetAll")) {
            std::cout << "Get all properties" << std::endl;
            if (!(reply = dbus_message_new_method_return(msg))) {
            }
            auto result = server_get_all_properties_handler(conn, reply);
            if (result !=DBUS_HANDLER_RESULT_HANDLED) {

            }
        }
        if (reply) {
            bool rr = dbus_connection_send(conn, reply, nullptr);
            dbus_message_unref(reply);
            if (!rr)
                return DBUS_HANDLER_RESULT_NEED_MEMORY;
        }
        // free the message
        dbus_message_unref(msg);
    }

    if (dbus_error_is_set(err)) {
        std::cerr << "--- " << err->name << " " << err->message << std::endl;
        exit(-6);
    }
}

int main() {
#ifdef _MSC_VER
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    DBusError dbus_error;
    DBusConnection *dbus_conn;
    // Initialize D-Bus error
    dbus_error_init(&dbus_error);
    // Connect to D-Bus
    dbus_conn = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
    if (!dbus_conn) {
        std::cerr << dbus_error.name << " " << dbus_error.message << std::endl;
        exit(-1);
    }
    std::cout << "Connected to D-Bus as \"" << ::dbus_bus_get_unique_name(dbus_conn) << "\"." << std::endl;
    // callMethod(dbus_conn, &dbus_error);
    // receiveSignals(dbus_conn, &dbus_error);
    // sendSignal(dbus_conn, &dbus_error);
    exposeMethod1(dbus_conn, &dbus_error);

    // Applications must not close shared connections -see dbus_connection_close() docs. This is a bug in the application.
    // dbus_connection_close(dbus_conn);
    // When using the System Bus, unreference the connection instead of closing it
    dbus_connection_unref(dbus_conn);
#ifdef _MSC_VER
    WSACleanup();
#endif
    return 0;
}
