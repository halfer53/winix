# Playing with winix

You can set an alarm, and specify the number of seconds
```
alarm 3
```

Type the following commands to create 4 child shell process

```
fork
fork
fork
fork
```

Now type 
```
ps
```

What did you see?

Now 
```
exit
exit
exit
exit
```

What did you see?
Read wait(2) and fork() to understand what's going on

Now let's test malloc
```
malloc
```

You can also spawn 100 threads
```
thread 100
```

now 
```
fork
ps
```

what's the parent of the current shell? (HINT type ```id``` to get the pid of the current shell)

Now what happens if you kill its parent? Think about it before you go forward

```
kill 2
```

You can also test signal
```
signal 2
```



