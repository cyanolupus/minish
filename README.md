# minish
incomplete shell

## 概要
最低限の機能だけを実装したシェル．
具体的に`exec`，`cd`，`help`，`version`が動く．

## 実行例
```
#version
$ ./minish -v
minish, version: 0.0.1

#help
$ ./minish -h
minish is incomplete shell.
        -h: help
        -v: version
        -c: exec command

#exec
$ ./minish
/home/user/to/current/directory
$ ls
minish  minish.cpp
/home/user/to/current/directory
$ 

#command
$ ./minish -c ls
minish  minish.cpp
```
