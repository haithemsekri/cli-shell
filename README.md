# cli-shell
interactive python cli shell

# Arguments types
┌──────────┬─────────────────────────┬─────────────────┬─────────────────┐
│Specifier │ Constant                │ Description     │ Expected C Type │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"y"       │ SD_BUS_TYPE_BYTE        │ 8bit unsigned   │ uint8_t *       │
│          │                         │ integer         │                 │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"b"       │ SD_BUS_TYPE_BOOLEAN     │ boolean         │ int * (NB: not  │
│          │                         │                 │ bool *)         │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"n"       │ SD_BUS_TYPE_INT16       │ 16bit signed    │ int16_t *       │
│          │                         │ integer         │                 │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"q"       │ SD_BUS_TYPE_UINT16      │ 16bit unsigned  │ uint16_t *      │
│          │                         │ integer         │                 │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"i"       │ SD_BUS_TYPE_INT32       │ 32bit signed    │ int32_t *       │
│          │                         │ integer         │                 │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"u"       │ SD_BUS_TYPE_UINT32      │ 32bit unsigned  │ uint32_t *      │
│          │                         │ integer         │                 │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"x"       │ SD_BUS_TYPE_INT64       │ 64bit signed    │ int64_t *       │
│          │                         │ integer         │                 │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"t"       │ SD_BUS_TYPE_UINT64      │ 64bit unsigned  │ uint64_t *      │
│          │                         │ integer         │                 │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"d"       │ SD_BUS_TYPE_DOUBLE      │ IEEE 754 double │ double *        │
│          │                         │ precision       │                 │
│          │                         │ floating-point  │                 │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"s"       │ SD_BUS_TYPE_STRING      │ UTF-8 string    │ const char **   │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"o"       │ SD_BUS_TYPE_OBJECT_PATH │ D-Bus object    │ const char **   │
│          │                         │ path string     │                 │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"g"       │ SD_BUS_TYPE_SIGNATURE   │ D-Bus signature │ const char **   │
│          │                         │ string          │                 │
├──────────┼─────────────────────────┼─────────────────┼─────────────────┤
│"h"       │ SD_BUS_TYPE_UNIX_FD     │ UNIX file       │ int *           │
│          │                         │ descriptor      │                 │
└──────────┴─────────────────────────┴─────────────────┴─────────────────┘

# example
python generate_cli_shell.py
python echo_server.py # terminal window1 ctrl+c to exit
python cli_shell.py 127.0.0.1 50008 # terminal window2 ctrl+d to exit
