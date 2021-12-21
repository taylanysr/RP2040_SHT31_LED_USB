cmake_minimum_required(VERSION 3.12)

# initialize the SDK based on PICO_SDK_PATH
include(pico_sdk_import.cmake)
project(my_task)

# initialize the Pico SDK
pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_executable(my_task
    src/main.c
	src/SHT31.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(my_task pico_stdlib hardware_gpio hardware_clocks)
add_library(FreeRTOS STATIC 
${FreeRTOS_src}
FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
FreeRTOS-Kernel/portable/MemMang/heap_4.c
FreeRTOS-Kernel/croutine.c
FreeRTOS-Kernel/event_groups.c
FreeRTOS-Kernel/list.c
FreeRTOS-Kernel/queue.c
FreeRTOS-Kernel/stream_buffer.c
FreeRTOS-Kernel/tasks.c
FreeRTOS-Kernel/timers.c FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c FreeRTOS-Kernel/portable/MemMang/heap_4.c)
target_include_directories(FreeRTOS PUBLIC FreeRTOS-Kernel/include inc/ FreeRTOS-Kernel/portable/GCC/ARM_CM0)


FILE(GLOB FreeRTOS_src FreeRTOS-Kernel/*.c)


target_include_directories( FreeRTOS PUBLIC
		FreeRTOS-Kernel/include
		include/
		FreeRTOS-Kernel/portable/GCC/ARM_CM0
	)

target_link_libraries( my_task 
	pico_stdlib 
	hardware_gpio 
	FreeRTOS
	)

target_link_libraries(my_task pico_stdlib pico_bootrom)
target_link_libraries(my_task pico_stdlib hardware_gpio hardware_i2c)

pico_add_extra_outputs(my_task)

pico_enable_stdio_usb(my_task 1)
pico_enable_stdio_uart(my_task 0)
#6. Add extra outputs to the blink project
pico_add_extra_outputs(my_task)
