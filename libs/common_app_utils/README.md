# Common Application Utilities
This repository includes custom utilities for applications.

## logging
Framework for tracing to UART and retargeting printf into UART.

## profiling
Framework for measuring execution time for a short code segments.

## fault handler
Custom faulthandler that prints the fault reason, register values and
stack dump when a fault happens. Also includes a python script that can
be used to look up function names from elf file based on the stack values.
