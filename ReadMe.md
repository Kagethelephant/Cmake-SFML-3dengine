### **Cmake Game Workflow Trial Run**

## General Notes

# Git

**Here is just some random things i have learned about git and im not sure if they are really accurate but this is a learning process**

If you use 'git log --all --decorate --oneline --graph' this will create a git graph with pretty colors to show your branch history
 '--all':       Uses all branches for the log, not just the current branch
 '--decorate':  Displays extra information about commits, like if the branch is checked out (Denoted by HEAD)
 '--oneline':   This removes some unecessary info to get each commit on to one line
 '--graph':     This is what shows the path each of the branches take to create the git graph

 This is slightly commical but i changed the default editor in git to nano just because i didnt want to figure out how to exit Vim, you can exit nano by Ctrl+X

 'git push --set-upstream origin "branch name"' will initialize the upstream for that branch
 'git rebase -i "commit hash"' will combine the commits from the current to the provided commit