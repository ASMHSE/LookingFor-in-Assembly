# LookingFor-in-Assembly
A Masm64 tool to find strings in files.

[![](https://img.shields.io/badge/Assembler-ML64-blue.svg?style=flat-square&logo=visual-studio-code&logoColor=white&colorB=000093)](https://learn.microsoft.com/en-us/cpp/assembler/masm/masm-for-x64-ml64-exe) 

Framework : [Masm64 SDK](https://masm32.com/board/index.php?topic=53.0)

This is a little tool with [FindOnDisk](https://masm32.com/board/index.php?msg=30731) general concept, wich is a very useful tool.

In this case, tool is limited to ANSI strings, and will fail with very long name files or non latin characters in directory names.

The file system exploration is a classical example from [Masm32 SDK](https://masm32.com/), obviously translated to 64 bits.

Boyer-Moore implementations are from [mineiro's code](https://masm32.com/board/index.php?msg=109928). A little modification was critical, because that implementation crash when file size is an exact multiple of page size (like probably happen in many others similar tools that fail).

Original discussion can be found in [Masm32 Forum](https://masm32.com/board/index.php?topic=11888.0)

Any sugestion or improvement is welcome!
