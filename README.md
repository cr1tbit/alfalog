# Alfalog

For too long have I struggled with no legit arduino logging library.


## Features

* Log levels
* Formatted printing
* Adding several backends, with configurable verbosity


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

## Writing custom backend

User can easily create their own target for logging (like a display, network socket, etc.)

All that's needed is to inherit from `AlfaBackend` class, overriding 2 methods.

``` c++
virtual void log(alog_level_t level, const std::string& msg);
virtual void begin();
```

An example for OLED logger can be seen in `/alfaBackends.h` file.

## #undef B1

That's kinda funny. https://github.com/fmtlib/fmt/issues/3559