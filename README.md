# WISE Application

[//]: # "make a table with two columns"

|Application| Testing|
|:-------------:| :-------------:|
| <img src="https://wise-developers.github.io/public_wise_assets/splash/wiserepo.png" width="250px"> | [![cmake badge][Cmake Badge 2]][Cmake Link] <br> [![smoke test 20.04 Badge][SmokeTest 20.04]][Smoke 2004 Link] <br> [![smoke test 22.04 Badge][SmokeTest 22.04]][Smoke 2204 Link] <br> [![deb file validation Badge][Deb Validate Badge]][Latest Releases Link] |
| [![release badge][Release Badge]][Latest Releases Link] |  |
| [![badge][Build Badge]][Latest Build Link] | [using the CLI](WISE_CLI_HELP.md)  |



The WISE Executable

WISE_Application formerly known as WISE
This is the main repo from which the application is built.

## Contributing

This project actively discourages forking of our repositories.
Contributors do not use forks, they use Branches and Pull Requests.
Please review the following before any forking or to understand our contributions paradigm.

[Contribution guidelines][contributing]

[contributing]: https://github.com/WISE-Developers/Project_issues/blob/main/CONTRIBUTING.md

[Latest Releases Link]: https://github.com/WISE-Developers/WISE_Application/releases/latest

[Release Badge]: https://badgen.net/github/release/WISE-Developers/WISE_Application?icon=github&scale=2

[Build Badge]: https://badgen.net/github/checks/WISE-Developers/WISE_Application?scale=2&icon=github&label=Application%20Build

[Latest Build Link]: https://github.com/WISE-Developers/WISE_Application/actions

[Cmake Badge]: https://github.com/WISE-Developers/WISE_Application/actions/workflows/cmake.yml/badge.svg?branch=main

[Cmake Link]: https://github.com/WISE-Developers/WISE_Application/actions/workflows/cmake.yml

[SmokeTest 20.04]: https://img.shields.io/github/actions/workflow/status/WISE-Developers/WISE_Application/smoketest_2004.yml?label=Smoke%20Test%20Ubuntu%2020.04%20&logo=github

[Smoke 2004 Link]: https://github.com/WISE-Developers/WISE_Application/blob/main/.github/workflows/smoketest_2004.yml

[SmokeTest 22.04]: https://img.shields.io/github/actions/workflow/status/WISE-Developers/WISE_Application/smoketest_2204.yml?label=Smoke%20Test%20Ubuntu%2022.04%20&logo=github

[Smoke 2204 Link]: https://github.com/WISE-Developers/WISE_Application/blob/main/.github/workflows/smoketest_2204.yml

[Deb Validate Badge]:https://img.shields.io/github/actions/workflow/status/WISE-Developers/WISE_Application/deb_test.yml?label=.deb%20validation&logo=github&style=plastic

[Cmake Badge 2]: https://img.shields.io/github/actions/workflow/status/WISE-Developers/WISE_Application/cmake.yml?label=CMake%20Build&logo=github
