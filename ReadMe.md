# **Cmake Game Workflow Trial Run**

## General Notes

### Markdown

markdown is used for this readme file denoted by .md

https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax

Here is a link to a github page that shows the syntax on markdown formatting and you can do some pretty neat stuff with it

### Git
**This section will be for writing down some notes as i learn git through the terminal**

If you use `git log --all --decorate --oneline --graph` this will create a git graph with pretty colors to show your branch history
 `--all`:       Uses all branches for the log, not just the current branch
 `--decorate`:  Displays extra information about commits, like if the branch is checked out (Denoted by HEAD)
 `--oneline`:   This removes some unecessary info to get each commit on to one line
 `--graph`:     This is what shows the path each of the branches take to create the git graph

 This is slightly commical but i changed the default editor in git to nano just because i didnt want to figure out how to exit Vim, you can exit nano by Ctrl+X

 `git push --set-upstream origin "branch name"` will initialize the upstream for that branch
 `git rebase -i "commit hash"` will combine the commits from the current to the provided commit

 ### Cmake Notes
 #### Navigation
**Just some basic terminal navigation stuff**

`mkdir "dir"` makes a new directory
`cd ..` Goes up a directory 

**Build and Run in Windows**
1. `mkdir` to make a build directory and navigate to that directory with `cd`
2. `cmake -G "MinGW Makefiles" ..` builds the cmake files in the current directory if the compiler is MinGW and the CMakeLists.txt is one level above
3. If this builds succesfully you can run the exe in the current directory using `./filename.exe`

This is just for some background on what you would have to do without the vscode plugin, i would recomend just using this and it will give you a build, debug and run button for cmake in the bottom left of the vscode window