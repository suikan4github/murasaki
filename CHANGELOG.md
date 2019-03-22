# Change log
Record of the modification in project development.
## [Unreleased]

### Added
-[Issue 35 : Add getMinStackHeadroom() to TaskStrategy class](https://github.com/suikan4github/murasaki/issues/35)

### Changed
-[Issue 34 : Refactor SYSLOG message from "Return" to "Leave" ](https://github.com/suikan4github/murasaki/issues/34)

### Deprecated
### Removed
### Fixed
-[Issue 33 : __VA_ARGS__ have problem if there are variable parameters.](https://github.com/suikan4github/murasaki/issues/33)

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

[Unreleased]: https://github.com/suikan4github/murasaki/compare/v0.3.0...develop
[0.3.0]: https://github.com/suikan4github/murasaki/compare/v0.2.1...v0.3.0
[0.2.1]: https://github.com/suikan4github/murasaki/compare/v0.2.0...v0.2.1
[0.2.0]: https://github.com/suikan4github/murasaki/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/suikan4github/murasaki/compare/v0.0.0...v0.1.0
