# cando
A library to aid programming with can sockets.

Create a virtual can socket for testing with the script `createVcan.sh`

`  >$ createVcan.sh`

Compile `cando` with `make` and test writing with

`  >$ candump -tz vcan0`

and reading (don't forget to change used function in `maine.c`) with

`  >$ cansend vcan0 123#12345678`
