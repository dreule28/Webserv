
# Git Wokrflow:

## Always Work in Your Own Branch - Never Push Directly to Master

1. git checkout -b IntraName_TypeOfBranch --> z.B iziane_fix/parser

2. only push here


## You have worked on your branch and now want to merge it into the master branch

1. git checkout master

2. git pull origin master

3. git merge feature-branch

    1. if merge conflicts appear, solve with concerned member

4. git push origin master


## Context: (mostly after you merged your branch with master, you do the following) —> Get your Branch up to date (after peers added feature)

1. go to branch



    ```bash

    git checkout <name of feature branch>

    ```

2. git fetch origin

3. git merge origin/master

    1. if merge conflicts appear, solve with concern member