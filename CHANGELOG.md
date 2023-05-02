# Change log
Record of the modification in project development.

## [Unreleased]

### Added
- [Issue 130 : Need description of the Stack headroom.](https://github.com/suikan4github/murasaki/issues/130)
- [Issue 139 : Add Si5351A Support.](https://github.com/suikan4github/murasaki/issues/139)
- [Issue 143 : Conditional compile on debug port ](https://github.com/suikan4github/murasaki/issues/143)
- [Issue 145 : Add Nucleo G0B1RE Support to murasaki_platform.cpp.tmp ](https://github.com/suikan4github/murasaki/issues/145)
- [Issue 147 : Support new FreeRTOS installation by CubeIDE.](https://github.com/suikan4github/murasaki/issues/147)

### Changed
- [Issue 142 : Split the thirdparty module as separate project.](https://github.com/suikan4github/murasaki/issues/142)
- [Issue 144 : Change extensions of templates.](https://github.com/suikan4github/murasaki/issues/144)
- [Issue 146 : Re-struct the directories](https://github.com/suikan4github/murasaki/issues/146)

### Deprecated
### Removed
### Fixed
- [Issue 117 : STM32G0 misses EXTI because of HAL incompatibility.](https://github.com/suikan4github/murasaki/issues/117)
- [Issue 126 : STM32L1 fails to compile the Exti class.](https://github.com/suikan4github/murasaki/issues/126)
- [Issue 129 : SetSyslogSeverityLevel() is misspelled.](https://github.com/suikan4github/murasaki/issues/129)
- [Issue 132 : Compile failure of adau1361.cpp when No debug, No syslog option](https://github.com/suikan4github/murasaki/issues/132)
- [Issue 133 : Removing the L1/F0 I2C workaround](https://github.com/suikan4github/murasaki/issues/133)
- [Issue 141 : Cycle counter works only when a program is loaded by debugger.](https://github.com/suikan4github/murasaki/issues/141)

### Security
### Known Issue


## [3.0.0] - 2020-04-24
The issue 119, 121 changes the file structure of the existing project. The issue 120 change the class name. So, this release has incompatibility to the past release. 

### Added
### Changed
- [Issue 119 : Make interrupt callback program automatic.](https://github.com/suikan4github/murasaki/issues/119)
- [Issue 121 : Make ESTI callback program automatic.](https://github.com/suikan4github/murasaki/issues/121)

### Deprecated
### Removed
### Fixed
- [Issue 120 : I2CMasterStrategy must be renamed.](https://github.com/suikan4github/murasaki/issues/120)
- [Issue 123 : STM32G0 EXTI cannot release the task by interrupt](https://github.com/suikan4github/murasaki/issues/123)
- [Issue 124 : Early EXTI call system failure](https://github.com/suikan4github/murasaki/issues/124)
- [Issue 125 : I2cStrategy and SpiStrategy are not appropriate name](https://github.com/suikan4github/murasaki/issues/125)

### Security
### Known Issue
- [Issue 117 : STM32G0 misses EXTI because of HAL incompatibility.](https://github.com/suikan4github/murasaki/issues/117)
- [Issue 126 : STM32L1 fails to compile the Exti class.](https://github.com/suikan4github/murasaki/issues/126)

## [2.0.0] - 2020-03-31
The Issue 91 fix brings incompatibility to the past. Class name changed.

### Added
- [Issue 51 : Add I2S audio adapter ](https://github.com/suikan4github/murasaki/issues/51)
- [Issue 53 : Add EXTI control class](https://github.com/suikan4github/murasaki/issues/53)
- [Issue 54 : Add a rotary encoder class](https://github.com/suikan4github/murasaki/issues/54)
- [Issue 94 : Add ADC class](https://github.com/suikan4github/murasaki/issues/94)
- [Issue 104 : Add sample code to the interrupt handler.](https://github.com/suikan4github/murasaki/issues/104)

### Changed
- [Issue 95 : Update the readme.rd ](https://github.com/suikan4github/murasaki/issues/95)
- [Issue 100 : Refactor the word size and shift of the DuplexAudio. ](https://github.com/suikan4github/murasaki/issues/100)
- [Issue 101 : Add I2S and ADC callback handler to the template. ](https://github.com/suikan4github/murasaki/issues/101)
- [Issue 102 : I2sSearch() should be in separate file ](https://github.com/suikan4github/murasaki/issues/102)
- [Issue 103 : Restructure the document module hierarchy ](https://github.com/suikan4github/murasaki/issues/103)
- [Issue 107 : Reject STM32H7 from I2sPortAdapter class by assertion ](https://github.com/suikan4github/murasaki/issues/107)
- [Issue 109 : Add more parameter check to the I2S](https://github.com/suikan4github/murasaki/issues/109)
- [Issue 110 : Drop 8bit support from DuplexAudio](https://github.com/suikan4github/murasaki/issues/110)
- [Issue 111 : Add more parameter check to the SAI](https://github.com/suikan4github/murasaki/issues/111)
- [Issue 112 : Add more parameter check to the SPI master and slave](https://github.com/suikan4github/murasaki/issues/112)
- [Issue 115 : ADD DMA checking for Uart and SPI.](https://github.com/suikan4github/murasaki/issues/115)

### Deprecated
### Removed
### Fixed
- [Issue 91 : Class SaiPortAdaptor must be renamed ](https://github.com/suikan4github/murasaki/issues/91)
- [Issue 92 : SaiPortAdaptor::GetSampleWordSizeTx() check coverage is not complete ](https://github.com/suikan4github/murasaki/issues/92)
- [Issue 93 : kfaAll and kfaNone is not handled at all.](https://github.com/suikan4github/murasaki/issues/93)
- [Issue 96 : Test code is left in I2sPortAdapter::StartTransferTx and StartTransferRx](https://github.com/suikan4github/murasaki/issues/96)
- [Issue 97 : 24bit I2S data is not correctly scaled. ](https://github.com/suikan4github/murasaki/issues/97)
- [Issue 98 : AudioPortAdapterStrategy::GetSampleDataSizeTx() / GetSampleDataSizeRx() should return the size by bits ](https://github.com/suikan4github/murasaki/issues/98)
- [Issue 99 : AudioPortAdapterStrategy::GetSampleWordsSizeTX() / GetSampleWordsSizeRX() returns 3 for 24bit word ](https://github.com/suikan4github/murasaki/issues/99)
- [Issue 105 : Build fail if EXTI_LINE_## is equal to the ##th power of 2](https://github.com/suikan4github/murasaki/issues/105)
- [Issue 106 : Assertion fail at DebugUart when a terminal emulator is connected. ](https://github.com/suikan4github/murasaki/issues/106)
- [Issue 108 : template/murasaki_platform.cpp has unnecessary LED control ](https://github.com/suikan4github/murasaki/issues/108)
- [Issue 114 : Saturation on the DuplexAudio transmitting.](https://github.com/suikan4github/murasaki/issues/114)

### Security
### Known Issue
- [Issue 117 : STM32G0 cause compile error because of HAL incompatibility.](https://github.com/suikan4github/murasaki/issues/117)


## [1.0.1] - 2020-01-07
### Added
### Changed
### Deprecated
### Removed
### Fixed
- [Issue 88 : MURASAKI_SYSLOG usage is wrong ](https://github.com/suikan4github/murasaki/issues/88)

### Security
### Known Issue
- The SPI slave transfer is not tested.

## [1.0.0] - 2019-12-31
Large release. 
- Added DuplexAudio to support audio in-out.
- Performance is drastically incresed by fixing bug of the Syncronizer class.

Note that issue #46, #56 and #77 made incompatibility from the past code. 
### Added
- [Issue 22 : Add Stereo Audio Class](https://github.com/suikan4github/murasaki/issues/22)
- [Issue 42 : Print the context information on the hard fault.](https://github.com/suikan4github/murasaki/issues/42)
- [Issue 44 : Support STM32CubeIDE startup code](https://github.com/suikan4github/murasaki/issues/44)
- [Issue 47 : Add Match() method to peripheral class](https://github.com/suikan4github/murasaki/issues/47)
- [Issue 49 : Add SAI interrupt handler template](https://github.com/suikan4github/murasaki/issues/49)
- [Issue 62 : Add the I2C Search function to the template](https://github.com/suikan4github/murasaki/issues/62)
- [Issue 66 : Add audio description to the user's guide](https://github.com/suikan4github/murasaki/issues/66)

### Changed
- [Issue 46 : Task priority is passed incorrectly](https://github.com/suikan4github/murasaki/issues/46)
- [Issue 56 : The Inc-tp and Src-tp should be moved to under Inc and Src, respectively](https://github.com/suikan4github/murasaki/issues/56)
- [Issue 61 : Change timeout parameter as integer](https://github.com/suikan4github/murasaki/issues/61)
- [Issue 69 : Step-by-Step installation guide should be removed](https://github.com/suikan4github/murasaki/issues/69)
- [Issue 77 : Add objectpointer parameter to MURASKI_SYSLOG](https://github.com/suikan4github/murasaki/issues/77)
- [Issue 82 : Rename the AudioAdapterStrategy and SaiAudioAdaptor](https://github.com/suikan4github/murasaki/issues/82)

### Deprecated
### Removed
### Fixed
- [Issue 41 : Confirm context check between ARMv6M and ARMv7M](https://github.com/suikan4github/murasaki/issues/41)
- [Issue 45 : Synchronizer::Wait() is incorrectly handling the timeout parameter](https://github.com/suikan4github/murasaki/issues/45)
- [Issue 48 : README.md has old Task based sample. Need to update](https://github.com/suikan4github/murasaki/issues/48)
- [Issue 55 : Adau1361::SetAuxInputGain seems to by buggy](https://github.com/suikan4github/murasaki/issues/55)
- [Issue 57 : DuplexAudio::TransmitAndReceive is incorrectly private member function](https://github.com/suikan4github/murasaki/issues/57)
- [Issue 58 : Adau1361::SendCommand and SendCommandTable are incorrectly private member funcitons](https://github.com/suikan4github/murasaki/issues/58)
- [Issue 59 : DuplexAudio visibility must be back](https://github.com/suikan4github/murasaki/issues/59)
- [Issue 63 : The sleep function doesn't handle the indefinite wait correctly](https://github.com/suikan4github/murasaki/issues/63)
- [Issue 64 : The scale constant of DuplexAudio is incorrectly calculated](https://github.com/suikan4github/murasaki/issues/64)
- [Issue 65 : murasaki_platform.hpp have to be updated](https://github.com/suikan4github/murasaki/issues/65)
- [Issue 67 : Adau1361::Set*Gain() functions overwrite the non-related field](https://github.com/suikan4github/murasaki/issues/67)
- [Issue 68 : README has wrong link to the LICENSE](https://github.com/suikan4github/murasaki/issues/68)
- [Issue 70 : "Blocking" should be changed to "Synchronous"](https://github.com/suikan4github/murasaki/issues/70)
- [Issue 72 : The word "CubeMX" should be substituted "CubeIDE"](https://github.com/suikan4github/murasaki/issues/72)
- [Issue 73 : Build failure for the CORTEX-M0](https://github.com/suikan4github/murasaki/issues/73)
- [Issue 74 : Build failure for STM32L152](https://github.com/suikan4github/murasaki/issues/74)
- [Issue 78 : Update install script](https://github.com/suikan4github/murasaki/issues/78)
- [Issue 79 : The core clock counter initialization have to be moved.](https://github.com/suikan4github/murasaki/issues/79)
- [Issue 80 : The sample peripheral name of the template has camel format](https://github.com/suikan4github/murasaki/issues/80)
- [Issue 81 : DuplexAudio::DMACallback() should use match()](https://github.com/suikan4github/murasaki/issues/81)
- [Issue 83 : Refactoring request of template. ](https://github.com/suikan4github/murasaki/issues/83)
- [Issue 84 : Change CODEC interface. ](https://github.com/suikan4github/murasaki/issues/84)
- [Issue 85 : Build failure for STM32F091 ](https://github.com/suikan4github/murasaki/issues/85)

### Security
### Known Issue
- The SPI slave transfer is not tested.

## [0.4.0] - 2019-04-07
Major change is the renaming Task class to SimpleTask. It loses backword compatibility. Also, severe problem bug of the cache control was fixed.

### Added
- [Issue 35 : Add getMinStackHeadroom() to TaskStrategy class](https://github.com/suikan4github/murasaki/issues/35)
^ [Issue 39 : Raise Assert failure at the DebugUart error handler.](https://github.com/suikan4github/murasaki/issues/39)

### Changed
- [Issue 34 : Refactor SYSLOG message from "Return" to "Leave" ](https://github.com/suikan4github/murasaki/issues/34)
- [Issue 37 : Rename Task class to SimpleTask](https://github.com/suikan4github/murasaki/issues/37)

### Deprecated
### Removed
### Fixed
- [Issue 33 : \_\_VA_ARGS__ have problem if there are variable parameters.](https://github.com/suikan4github/murasaki/issues/33)
- [Issue 36 : "Sample" section is missed in the TOC of README.md](https://github.com/suikan4github/murasaki/issues/36)
- [Issue 40 : Cache in-coherency crashes the program ](https://github.com/suikan4github/murasaki/issues/40)

### Security
### Known Issue
- The SPI slave transfer is not tested.

## [0.3.0] - 2019-03-09
Major change at the MURASAKI_ASSERTION to enforce the hard fault. This makes debug easier. The HAL header file include mechanism is now eaiser to catch up the ST Micro's new processor. 

### Added
- [Issue 27 : ADD STM32F4xx support](https://github.com/suikan4github/murasaki/issues/27)

### Changed
- [Issue 24 : SpiSlaveSpecifier class name should be refactored](https://github.com/suikan4github/murasaki/issues/24)
- [Issue 23 : Change MURASAKI_ASSERT to raise an exception](https://github.com/suikan4github/murasaki/issues/23)
- [Issue 26 : InitPlatform needs to check the result of new](https://github.com/suikan4github/murasaki/issues/26)
- [Issue 28 : uint should be changed unsigned int](https://github.com/suikan4github/murasaki/issues/28)
- [Issue 29 : Update Inc/murasaki_defs.hpp](https://github.com/suikan4github/murasaki/issues/29)
- [Issue 31 : Update document with new HAL include scheme](https://github.com/suikan4github/murasaki/issues/31)

### Deprecated
### Removed
### Fixed
- [Pull Request 21 : Typo](https://github.com/suikan4github/murasaki/pull/21). Thanks osaboh.
- [Issue 25 : "Takemasa" is wrong signature](https://github.com/suikan4github/murasaki/issues/25)

### Security
### Known Issue
- The SPI slave transfer is not tested.

## [0.2.1] - 2019-02-24

### Added
### Changed
### Deprecated
### Removed
### Fixed
- [Issue 19 ; Clone method is wrong](https://github.com/suikan4github/murasaki/issues/19)

### Security
### Known Issue
- The SPI slave transfer is not tested.

## [0.2.0] - 2019-02-23

### Added
- [Issue 6 : Add CubeMX configuration note to the porting guide.](https://github.com/suikan4github/murasaki/issues/6)
- [Issue 10 : Write an install script](https://github.com/suikan4github/murasaki/issues/10)
- [Issue 12 : Step-By-Step porting sample](https://github.com/suikan4github/murasaki/issues/12)
- [Issue 13 : Add post mortem debugging description](https://github.com/suikan4github/murasaki/issues/13)

### Changed
- [Issue 8 : ADD link to murasaki_samples to README.md](https://github.com/suikan4github/murasaki/issues/8)
- [Issue 16 : Substitute \n\r by \n ](https://github.com/suikan4github/murasaki/issues/16)
- [Issue 17 : README.md refinement for DMA Interrupt and required memory](https://github.com/suikan4github/murasaki/issues/17)

### Deprecated
### Removed
### Fixed
- [Issue 5 : HAL_GPIO_EXTI_Callback() needs to avoid the null pointer access](https://github.com/suikan4github/murasaki/issues/5)
- [Issue 7 : README.md doesn't have link to the "where to get" at ToC](https://github.com/suikan4github/murasaki/issues/7)
- [Issue 11 : Link to refman.pdf in README.md is wrong](https://github.com/suikan4github/murasaki/issues/11)
- [Issue 15 : Write a Program flow walk through](https://github.com/suikan4github/murasaki/issues/15)

### Security
### Known Issue
- The SPI slave transfer is not tested.

## [0.1.0] - 2019-02-11

### Added
 - README.md file
 - This CHANGELOG.md file

### Changed
- [Issue 1 : Convert debug tasks from derived class to the simple task.](https://github.com/suikan4github/murasaki/issues/1)
- [Issue 2 : Refactoring of the Debugger::AutoRePrint](https://github.com/suikan4github/murasaki/issues/2)
- [Issue 3 : Porting guide must follow the new directory.](https://github.com/suikan4github/murasaki/issues/3)
- [Issue 4 : Extern declaration is too much](https://github.com/suikan4github/murasaki/issues/4)

### Deprecated
### Removed
### Fixed
### Security

[Unreleased]: https://github.com/suikan4github/murasaki/compare/v3.0.0...develop
[3.0.0]: https://github.com/suikan4github/murasaki/compare/v2.0.0...v3.0.0
[2.0.0]: https://github.com/suikan4github/murasaki/compare/v1.0.1...v2.0.0
[1.0.1]: https://github.com/suikan4github/murasaki/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/suikan4github/murasaki/compare/v0.4.0...v1.0.0
[0.4.0]: https://github.com/suikan4github/murasaki/compare/v0.3.0...v0.4.0
[0.3.0]: https://github.com/suikan4github/murasaki/compare/v0.2.1...v0.3.0
[0.2.1]: https://github.com/suikan4github/murasaki/compare/v0.2.0...v0.2.1
[0.2.0]: https://github.com/suikan4github/murasaki/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/suikan4github/murasaki/compare/v0.0.0...v0.1.0
