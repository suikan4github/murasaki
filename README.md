# murasaki
STM32 HAL class library

## Description
Murasaki is a class library for the SM32 HAL. 
By using Murasaki, programmer can use SM32 HAL with FreeRTOS as multi-task aware style. 
A set of classes provides the abstraction of the following peripherals. 
 * UART(Serial)
 * SPI Master
 * SPI Slave
 * I2C Master
 * I2C Master
Note that above peripheral abstraction provides the thread-safe, interrupt/DMA based, and blocking IO.

The thread safe is important under the RTOS environment. 
Because of themulti-tasking, multiple controller algorithm may access one peripheral at once. 
If a task switch happesn at the middle of the communication over certain peripheral, 
the program may be broken.
To prevent this problem, Murasaki guard the important portion of contoro program by critical section.
Then, programmer doesn't need to care the excrusive access to peripheral.

The interrupt/DMA access is also important to utilize the CPU under RTOS environment.
STM32 HAL provide polling version of API. These API has waiting loop inside. 
As aresult, such the API will occupy the CPU while it wait for an envent.
The interrupt/DMA based peripheral access utilize the CPU with high efficiency under RTOS environement.

The blocking IO guarantees the end of communication when program returns from one IO function.
Then, once return from function, for example, received data is ready to use in a RX buffer.

In addition to the above peripheral, a set of classes serve the following functionalities :
 * Task
 * Synchronizer ( Wait-Signal )
 * Critical Section
 * Message output
 
## Sample
```C++
murasaki::debugger->Printf("Hello, World!");
```

```C++
murasaki::debugger->Printf("Coounter value is : %d\n", counter);
```

```C++
uint8_t data[5] = { 1, 2, 3, 4, 5 };
murasaki::UartStatus stat;

stat = murasaki::platform.uart->Transmit(
                                         data,
                                         5);

```

```C++
uint8_t data[5] = { 1, 2, 3, 4, 5 };
murasaki::I2cStatus stat;

stat = murasaki::platform.i2cMaster->Transmit(
                                              127,
                                              data,
                                              5);
```


```C++
 // For demonstration of FreeRTOS task.
 murasaki::platform.task1 = new murasaki::Task(
                                               "Master",
                                               256,
                                               (( configMAX_PRIORITIES > 1) ? 1 : 0),
                                               nullptr,
                                               &TaskBodyFunction
                                               );
```
```
    murasaki::platform.task1->Start();

```



## VS. 
## Requirement
## Install
## License
[MIT](https://github.com/suikan4github/murasaki/blob/master/LICENSE)
## Authuor 
[Seiichi "Suikan" Horie](https://github.com/suikan4github)