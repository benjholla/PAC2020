# README

## Setup
- Compile with `gcc toy.c -Wno-format-security -o toy`
- Run with `./toy` and enter a string or write a string to a file and run `./toy < file`

## Manual Analysis
This is a simple example. Let's see if we can force a program crash. The first challenge is to get past the branch conditions. If the start of out input data that is read begins with `hack` we will satisfy all conditions. Check that you can get the program to echo `hack` using the `printf`.

Next, try to see if you can derive and input that crashes the `printf`. This program is vulnerable to Format String attacks so we can crash the program by passing the string `%s%s%s%s%s%s%s%s%s%s%s%s` to the `printf` (remember we must prefix the input with `hack` still). For more details on Format String attacks see [http://www.cis.syr.edu/~wedu/Teaching/cis643/LectureNotes_New/Format_String.pdf](http://www.cis.syr.edu/~wedu/Teaching/cis643/LectureNotes_New/Format_String.pdf). Try entering the string `hack%s%s%s%s%s%s%s%s%s%s%s%s` to crash the program.

## Fuzzing
Optionally, before we start fuzzing, let's make it easier to recognize the crash by inserting a `__builtin_trap()` call to trigger a program crash right before the `printf` is executed. This will make the task of crashing the program easier for the fuzzer. Once we learn (via the fuzzer) that all inputs must be prefixed with `hack` then we can remove the forced exception.

Compile the vulnerable program and instrument it for fuzzing by AFL with `afl-gcc toy.c -Wno-format-security -o toy`.

Before we move on let's take at look at how AFL instrumented the program. We will run Radare2 on the binary using the web UI. Run `r2 -c='H' toy`. At the Overview screen check all Analysis Options checkboxes and press the Analyze button. Then select the Functions page from the navigation pane on the left. Under the function table select `main` and click the memory address hyperlink.  When viewing the main function, select the Other Representations hamburger icon at the top and select Graph to view the Control Flow Graph. Examine the control flow graph and try to see what you can recognize from the source. You should see the conditional checks for the `HACK` string. Notice that in each block AFL has added a call to `__afl_maybe_log` with a random initialization of the RCX register (e.g. `mov rcx, 0xc20e`) to identify the path that is taken at runtime.

To start fuzzing we need to provide AFL a directory of initial input examples and an output directory to save its findings and queue of mutated inputs to test. Remember that by default AFL is looking for program crashes and that AFL expects all program inputs to consist of a single binary file! This toy example already meets both of those requirements, but for other programs you have to create a fuzzer test harness by modifying the source to accept a file as the primary input and if the behavior of interest does not translate to a crash you may have to insert a condition that checks the behavior and throws an exception to trigger a crash.

Create an input and output directory by running `mkdir input` and `mkdir output` in the `lab6` directory. Add an example input to the `input` directory by running `echo AAAA > input/example`. This adds an input named `example` that consists of 4 `A` characters. Later you may want to experiment with adding examples that meet more of the conditions in the program (e.g. `echo haha > input/example2` or `echo hac > input/example2`). How does the affect the fuzzing time? Start AFL by running `afl-fuzz -i input -o output ./toy`. Given *enough* time, AFL should find the vulnerability without any assistance by crashing the program, but is that the most efficient use of your time and money (think of your electric bill!)? How can you improve your odds of finding the vulnerability and how much human effort does it take?

## References
Note that this tutorial was based off of an excellent university lecture tutorial available at: [https://tc.gts3.org/cs6265/2019/tut/tut10-01-fuzzing.html](https://tc.gts3.org/cs6265/2019/tut/tut10-01-fuzzing.html). This tutorial covers many more topics on fuzzing and well worth a read.
