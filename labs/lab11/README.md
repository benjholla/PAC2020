# Lab 11

Lab 11 is to repeat labs 0-10 but utilize AI!

The following AIs all have free tiers that can be utilized for this lab.

- [ChatGPT](https://chatgpt.com)
- [Gemini](https://gemini.google.com/app)
- [Copilot](https://copilot.microsoft.com)
- [Claude](https://claude.ai)

In addition, `claude` code is installed in this virtual machine. For usage
help, run `claude --help`.

## Lab 0
- Can you teach the LLM the semantics of the tape machine language (`+`, `-`, `>`, `<`, `[`, `]`) instructions?
    - Ask it to generate hello world?
    - Validate the generated program with the simulator / refine the description of the language semantics
- Can you derive a Brainf*ck generator for arbitrary string outputs?
- Can you derive an analyzer for the language?
    - Can we identify trivial cases of programs halting?
    - Can we decompile C to Brainf*ck compiled programs back to C?

## Lab 1-3
- Provide the source of basic_vuln.c and describe your understanding of how it can be exploited (in the style of lab 1 and lab 2).
    - Does the LLM agree with your description? Refine your understanding by challenging responses and demanding evidence of claims (LLMs can hallucinate / forget facts, do not assume it is correct!)
- Chose your favorite language and iteratively prompt the LLM to develop an exploit generator (repeat for lab 1 and 2 exploits)
    - Identify the hard constraints on how inputs are generated so that generator can produce multiple valid exploits (e.g. variable sizes/contents allowed  for padding, NOP sleds, payloads, etc.)
- Validate the developed generator by testing exploits in the VMs
    - Note that Lab 3 moved from a 32bit to 64bit architecture

## Lab 4
- Describe what you are doing (remember this is an ethical exercise to learn exploit development).
    - Note your VM is running in an isolated environment.
    - For the sake of learning, do not mention `MiniShare` (avoid known solutions)
- Iteratively develop the MiniShare exploit by noting observations to the LLM and asking for help if the suggestion is unclear

## Lab 5
- Provide source code snippets of MiniShare with the goal of looking for network exploitable memory corruptions
    - Paste serve2.c and ask for the taint flows from network to vulnerable functions
- Ask for explanations of code snippets you didn't understand earlier
    - Look for other means to exploit MiniShare (e.g. can you locally control filename lengths to crash MiniShare?)
- Note this lab may be easier to achieve by exploring developer oriented LLMs which often support agentic workflows (e.g. Copilot has VSCode integrations captures context of code more naturally)

## Lab 6
- Provide snippets of key data structures to fuzz and prompt to generate a test harness for AFL that populates the data structure with a custom main
- Using LLM prompt for interesting edge cases to seed AFL fuzzer
- Explore fuzzing more interesting programs
    - MiniShare requires WinAFL fork and recompiling sources with MinGW toolkit
    - GNU Core Utils are more easily accessible for fuzzing on Linux
- Evaluate whether crash results of locally targeted data structures are constrained out by larger program context (i.e. is crash reachable?)

## Lab 7
- Provide source snippets of vulnerable programs
- Prompt LLM to generate inputs based on the path constraints on inputs necessary to exploit the programs
- Which one is more effective / expensive?
- How do the two approaches scale with problem complexity?
- Repeat using binary (can we disassemble the binary and reason effectively on artifacts?)

## Lab 8
- Lab 8 is effectively an exercise in code refactoring
- Can LLMs refactor code while preserving the key invariants of the exploit?
- Iteratively guide the refactoring development to explore antivirus capabilities

## Lab 9
- Provide source code context of ConnectBotBad
    - Can we hypothesis threat models for application concerns?
    - Can we effectively rule out or refine generated hypothesis into quickly checkable code queries?
    - Does the LLM need additional context (e.g. the Android permission mapping)?
- Provide source code context of FlashBang
    - Can we effectively summarize the capabilities of the malware sample?
    - This sample was a real same that was `defanged` to make it safe for education use for this course. How do your findings compare to the real sample `Stels` (https://www.sophos.com/en-us/research/stels-android-trojan-malware-analysis)?

## Lab 10
- Using the simulator try to teach the LLM the semantics of the simplified zip program language.
    - Define PRINT(N)
    - Define REPEAT(D,N)
    - Ask for example programs to test on simulator to refine program semantics
- Define a quine zip program (the output program string is exactly the source program string)
    - Attempt to derive a quine generator (or at least produce a new quine instance)
- Note: This is very hard. After an hour-long session, I quit in frustration...
    - Hit a wall in the problem complexity. Current AI was able to help refine the semantics of the language, but continued to either hallucinate a quine was impossible (despite counter examples) or regressed and forgot import program semantics
    - I succeeded at getting ChatGPT to produce one new quine instance, but failed to make a generator
