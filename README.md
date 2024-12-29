# CSP

The program csp (cs program) is intended for compression of .wav and .txt in .cs format (coincidentally coincided with the C# extension) and decompression of .cs format to the original. It has the possibility of expansion, ie adding new formats that it can compress and decompress. WAV files are compressed up to 25% of the original size, by adjusting the parameters (constants) DELTA_DB and
DELTA_PHASE (17 and 18 lines in csw.c) can be further reduced in size, but quality is lost. TXT depends on the file size and character probability distribution, in the worst case it increases the size.
The csp program is called from the terminal with certain arguments. The rules for accepting arguments are defined below.

The csp program was created in the Visual Studio Code text editor. The Make tool was used to automate compiling, building and linking.

Used shell scripts for additional automation, which were set to not perfect, due to emailing.

The gprof tool was used to test the execution speed. The time_test.sh script was used to automate the testing process. NOTE (uncomment line 18 in main function to call proba2() f used for speed testing)

The build.sh script was used for compiling and linking

To run the run.sh script without arguments

Test files are located in the test_files folder

## HELP for use

!!! Install celluloid (audio player) on linux

- `sudo apt install celluloid'

----------------CSP-cs program for manipulating cs compressed formats----------

Available options:
-h{help option}
-c{to compress the file, take the following string as the input file to compile}
-d{to decompress the file, take the following string as the input file to decompile}
-o{optionally, the desired output file name is added to this}
-p{option to open compressed cs formats, optional option -o cannot be used with this one}

Usage examples:

1. Compressing
   1. csp -c {FILE_NAME}
   2. csp -c {FILE_NAME} -o {OUTPUT_FILE_NAME}
2. Decompression
   1. csp -d {FILE_NAME}
   2. csp -d {FILE_NAME} -o {OUTPUT_FILE_NAME}
3. Opening the compressed file
   csp -p {FILE_NAME}
4. Help
   csp - h

\*\*\* There are certain bugs:

1. freeing memory after decompressing lines 429-432 in csw.c
2. -p option does not work for TXT files
3. An additional problem is cutting off a few seconds from the end of the wav file due to the division of the file into blocks of size 2^16 due to fft, and the last block, which is smaller, is discarded. It is possible to upgrade to fill with zeros and then fft and discard the zeros during decompression.
4. IMPORTANT - Bug detected while compressing second audio channel in .wav file, so it is set to compress only first channel (search for //! commennts in csw.c file)
5. ...

\*\*\* Some of my improvised version of fft was used ------> Possible significant acceleration by using some fast version.

\*\*\* It is possible to upgrade so that the opening of the file is multi-level because it is decompressed block by block with the call of a certain function that has the possibility of playing a string on the speaker. That way, there would be no waiting for the opening.

\*\*\* I'm not sure I've found a complete standard for wav files, so there's a possibility that it marks the wav format as unsupported.
