# Alfalog

For too long have I struggled with no legit arduino logging library.


## Features

* Log levels (with colors!)
* Hexdumping
* Formatted printing
* Adding several backends, with configurable verbosity
* Example of an advanced OLED backend, combining logging and other info


This library has been written with ESP32 family in mind, where ram is not that big of an issue. I'd not use it with low-end targets.

The fmtlib is used as a backend, which has not been written with embedded targets in mind.

## Using the library

See `/examples`

For the basic serial printing, user must supply their own print handle, which can be as easy as:

``` c++
void uartPrintAlogHandle(const char* str){
  Serial.println(str);
}
```

This also allows the user to log with more exotic targets:

``` c++
AsyncWebServer server(80);
AsyncEventSource events("/events");

server.addHandler(&events);

void socketAlogHandle(const char* str){
    events.send(str,"log",millis());
}
```

## Writing custom backend

User can easily create their own target for logging (like a display, network socket, etc.)

All that's needed is to inherit from `AlfaBackend` class, overriding 2 methods.

``` c++
virtual void log(alog_level_t level, 
  const char* file, int line, 
  const std::string& msg);

virtual void begin();
```

An example for OLED logger can be seen in `advancedOledLogger.h` file.

## #undef B1

That's kinda funny. https://github.com/fmtlib/fmt/issues/3559