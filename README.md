# Murasaki
Easy to use [STM32Cube HAL](https://www.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32cube-mcu-packages/stm32cubef7.html#sw-tools-scroll) class library with FreeRTOS.

## Talbe of Contents
 * [Description](#description)
 * [Sample](#Sample)
 * [VS. Naked STM32Cube HAL](#vs-naked-stm32cube-hal)
 * [Requirement](#requirement)
 * [Where to get Murasaki](#where-to-get-murasaki)
 * [Install](#install)
 * [License](#license)
 * [Author](#author)

## Description
Murasaki is a C++ class library for the [STM32Cube HAL](https://www.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32cube-mcu-packages/stm32cubef7.html#sw-tools-scroll).
By using Murasaki, a programmer can use STM32Cube HAL with FreeRTOS as a multi-task aware style.
A set of classes provides the abstraction of the following peripherals.
 * UART(Serial)
 * SPI master
 * SPI slave
 * I2C master
 * I2C slave
 * SAI
 * I2S
 * ADC
 * GPIO
 * EXTI

Note that these peripheral abstraction provides the thread-safe, interrupt/DMA based, and blocking IO.

The thread-safe is necessary under the RTOS environment.
Because of the multi-tasking, multiple controller algorithms may access one peripheral at once.
If a task switch happens at the middle of the communication of the specific peripheral, 
and the next task accesses that peripheral, the program may be broken.
To prevent this problem, Murasaki guards the critical portion of the control program by a critical section.
Thus, the programmer doesn't need to care about exclusive access to the peripheral.

The Murasaki peripheral classes use DMA/Interrupt transfer.
The interrupt/DMA access is also essential to utilizing the CPU under an RTOS environment.
While STM32Cube HAL provides a polling version of API, these API has a waiting loop inside.
As a result, such the polling API will occupy the CPU while it waits for an event.
The interrupt/DMA based peripheral access doesn't have such the waiting loop, thus RTOS
can utilize CPU for other tasks, while peripheral doesn't need CPU's help.

The blocking IO guarantees the end of communication when the program returns from one IO function.
Then, once returned from the function, for example, received data is ready to use in an RX buffer.

In addition to the above peripheral, a set of classes serve the following functionalities :
 * Task
 * Synchronizer ( Wait-Signal )
 * Critical Section
 * Message output

You can find a set of the sample in the [murasaki_samples](https://github.com/suikan4github/murasaki_samples) project.

The API documentation can be found in murasaki/doc directory. Also, related documentation can be found in the [project wiki](https://github.com/suikan4github/murasaki/wiki).

## Sample
With the Murasaki library, the STM32Cube HAL IO becomes easy to write.

The text output to a serial console is straightforward. Programmer can use a printf() style member function.
Then, a programmer can output text with a well-formatted style.
This Printf() is Non-blocking and Asynchronous function while the other Murasaki IO is blocking and Synchronous IO.
That means the program doesn't need to wait for the end of transmission through the IO port.
This behavior allows a programmer can use printf() with the minimum impact the real-time execution. In addition to these characteristics,
a programmer can use printf() in both tasks and interrupt context.   

```C++
murasaki::debugger->Printf("Hello, World!");
murasaki::debugger->Printf("Coounter value is : %d\n", counter);
```

Like printf(), almost functions are implemented as a member function of some classes.

You can see a sample of the UART transmission in Murasaki. This is also easy to program.
As opposed to the Printf(), the Transmit() member function of the Uart class is Blocking and Synchronous function.
Then, Transmit() returns when the transmission is complete.

```C++
uint8_t data[5] = { 1, 2, 3, 4, 5 };
murasaki::UartStatus stat;

stat = murasaki::platform.uart->Transmit(
                                         data,  // pointer to data
                                         5);    // number of data to transmit

```
If the transmission takes a long time, the task may be switched to the other task to utilize CPU time.

The transmission through the I2C master peripheral is very similar to the transmission through the UART peripheral.
One difference is, the I2C master transmission needs a slave device address.  

```C++
uint8_t data[5] = { 1, 2, 3, 4, 5 };
murasaki::I2cStatus stat;

stat = murasaki::platform.i2cMaster->Transmit(
                                              127,    // I2C slave device address in 7bit
                                              data,   // pointer to data
                                              5);     // number of data to transmit
```

The class also encapsulates the task.
```C++
// For demonstration of FreeRTOS task.
    murasaki::platform.task1 = new murasaki::SimpleTask(
                                                        "task1",                /* Task name */
                                                        2048,                   /* Stack size */
                                                        murasaki::ktpNormal,    /* Task priority */
                                                        nullptr,                /* Stack must be allocated by system */
                                                        &TaskBodyFunction       /* Pointer to the task body */
                                                        );
// Start a task
murasaki::platform.task1->Start();

```

## VS. Naked STM32Cube HAL
### Ease of control
The STM32 series HAL is designed to cover broad functions as far as possible.

As a result, it can run in either :
 * Polling
 * Interrupt
 * DMA

transfer styles. And its design is free from the specific RTOS.
This design added the HAL large number of functions and required a programmer to build complicated sequences to transmit short data.   

The Murasaki library is designed to support only an RTOS environment.
This limited use case makes the function set smaller. And allows class library encapsulate the complex sequence of using non-blocking
asynchronous HAL APIs to utilize the CPU with the interrupt / DMA transfers. Then, Murasaki's function is easier to use compared to the naked HAL.

### Strongly controlled namespace
The STM32Cube HAL is designed to not only C++ but also C compiler. So, all functions are in the global namespace, and constants are macro.
This too flat namespace prevents the IDE's completion helps programmers. In addition to this, the wrong macro values are quickly passed to the incorrect parameters.

Murasaki's design encapsulates the functions into the dedicate class. 
Then, a programmer can use the help of input completion of IDE.

Each function parameter has a dedicated enumeration type. These design triggers compile error if the wrong type constant is passed.
So, the compiler can help a programmer to write a correct program.

## Requirement
The Murasaki library is developed with the following environment :
 * [Ubuntu 16.04 LTS](http://releases.ubuntu.com/16.04/), [Ubuntu 18.04 LTS](http://releases.ubuntu.com/18.04/)
 * [STM32CubeIDE](https://www.st.com/ja/development-tools/stm32cubeide.html) 1.3.0

The tested target is following :
 * [Nucleo H743ZI](https://www.st.com/en/evaluation-tools/nucleo-h743zi.html)(Cortex-M7)
 * [Nucleo F746ZG](https://www.st.com/en/evaluation-tools/nucleo-f746zg.html)(Cortex-M7)
 * [Nucleo F722ZE](https://www.st.com/en/evaluation-tools/nucleo-f722ze.html)(Cortex-M7)
 * [Nucleo F446RE](https://www.st.com/en/evaluation-tools/nucleo-f446re.html)(Cortex-M4)
 * [Nucleo G431RB](https://www.st.com/en/evaluation-tools/nucleo-g431rb.html)(Cortex-M4)
 * [Nucleo L412RB-P](https://www.st.com/en/evaluation-tools/nucleo-l412rb-p.html)(Cortex-M4)
 * [Nucleo L152RE](https://www.st.com/en/evaluation-tools/nucleo-l152re.html)(Cortex-M3)
 * [Nucleo F091RC](https://www.st.com/en/evaluation-tools/nucleo-f091rc.html)(Cortex-M0)
 * [Nucleo G070RB](https://www.st.com/en/evaluation-tools/nucleo-g070rb.html)(Cortex-M0+)

Pins, clocks, peripherals, and FreeRTOS have to be configured by CubeMX.

The required memory resources for the Nucleo F722 demo application (2 tasks, 1 LED,
  1 UART , 1 I2C and 1 EXTI) is following:

|Section|Memory|Size|
|-------|:----:|---:|
|.text|Flash|48kB|
|.data & .bss|RAM|16kB|


## Where to get Murasaki
The Murasaki library can be obtained from [Github project page](https://github.com/suikan4github/murasaki/)

## Install
The Murasaki library is an add-on to the STM32 application skeleton generated by CubeMX. 
Once CubeMx generated the code, a programmer can run the install script to set up the project.
 1. Generate the C++ based CubeHAL application script by CubeIDE
 1. Clone Murasaki under the project
 1. Run the murasaki/install script
 1. Add murasaki/Inc as G++ include path
 1. Add murasaki/Src as G++ source path

The detail of the install procedure is described in the
[doc/refman.pdf](https://github.com/suikan4github/murasaki/blob/master/doc/refman.pdf)
## License
The Murasaki library is distributed under [MIT License](LICENSE)
## Author
[Seiichi "Suikan" Horie](https://github.com/suikan4github)
