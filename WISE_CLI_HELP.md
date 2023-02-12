<img src="https://wise-developers.github.io/public_wise_assets/splash/wiserepo.png" width="250px">


[![release badge][Release Badge]][Latest Releases Link]

usage: WISE [OPTIONS]... [FILENAME]
W.I.S.E. supports multiple file formats with different extensions:
        FGMJ: W.I.S.E. inputs stored in JSON format
        FGMB: W.I.S.E. inputs stored in binary format:
____
| Argument                       | Output                                                                                                                                                                                                                                     |
|--------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| -h [ --help ]                  | print this message                                                                                                                                                                                                                                     |
| -v [ --version ]               | print the W.I.S.E. version                                                                                                                                                                                                                             |
| -i [ --include-shared-memory ] | include shared memory block recording used cores                                                                                                                                                                                                       |
| -o [ --output ] arg            | optional output for the conversion function. Must use a supported file extension. If the same file extension is specified as the input type no conversion will be carried out                                                                          |
| -c [ --convert ]               | convert the input file to a different format, the job will not be run. Both the input and output files must use one of the supported file extensions                                                                                                   |
| -V [ --convertversion ] arg    | if converting, then this specifies the version of the FGMJ output file                                                                                                                                                                                 |
| -u [ --summary ]               | output summary files for the scenario(s) in the job file in TXT format then quit. A summary file will be created for all scenarios but none will be run                                                                                                |
| -j [ --jsummary ]              | output summary files for the scenario(s) in the job file in JSON format then quit. A summary file will be created for all scenarios but none will be run                                                                                               |
| -b [ --bsummary ]              | output summary files for the scenario(s) in the job file in binary format then quit. A summary file will be created for all scenarios but none will be run                                                                                             |
| -s [ --sysinfo ]               | prints system information (cores, memory, etc.) then quits                                                                                                                                                                                             |
| -p [ --password ] arg          | an optional password for the job file                                                                                                                                                                                                                  |
| -n [ --new-password ] arg      | an optional password for the converted job file                                                                                                                                                                                                        |
| -m [ --manager ] arg           | an optional W.I.S.E. Manager ID for W.I.S.E. to communicate with                                                                                                                                                                                       |
| -t [ --stats ]                 | output run statistics including start time and peak memory usage                                                                                                                                                                                       |
| -r [ --cores ] arg             | an optional override for the number of cores to use to run the job. Not to be used at the same time as mask. cores and offset combined will define which cores the process is locked to  |
| -f [ --offset ] arg            | an optional offset into the existing cores (disregarding NUMA nodes and processor groups) to lock the cores that will run the job. Not to be used at the same time as mask. cores and offset combined will define which cores the process is locked to |
| -a [ --mask ] arg              | an optional bit mask to specify which cores should be used to run the job. Not to be used at the same time as offset. The maximum number of bits that will be used is the same as the number of cores in the system, extra bits will be ignored        |
| --validate                     | validate the FGM but don't run it. The results will be written to validation.json unless a different file is specified using the `vn` option                                                                                                           |
| --vn arg (=validation.json)    | an optional name for the exported validation file. Defaults to validation.json                                                                                                                                                                         |
| -l [ --balance ] arg           | select which scenario indices should be run if multiple are available. Will override any load balancing specified in the file |

___


[Latest Releases Link]: https://github.com/WISE-Developers/WISE_Application/releases/latest

[Release Badge]: https://badgen.net/github/release/WISE-Developers/WISE_Application?icon=github&scale=2