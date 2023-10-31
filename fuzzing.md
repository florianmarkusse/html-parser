```
afl-clang-fast -Iinclude -fsanitize=address,undefined -g3 -o fuzzer fuzzing.c src/**/*.c build/libhtml-parser-Release.a
```

```
afl-fuzz -i tests/src/inputs -o fuzz-output ./fuzzer
afl-fuzz -i benchmarks/inputs -o fuzz-output ./fuzzer
```


dump notifications to local:
```
sudo sh -c 'echo core >/proc/sys/kernel/core_pattern'
```

Have to change CPU scaling:
```
cd /sys/devices/system/cpu
echo performance | sudo tee cpu*/cpufreq/scaling_governor
```

Change it back after done to not fry CPU:

```
cd /sys/devices/system/cpu
echo ondemand | sudo tee cpu*/cpufreq/scaling_governor
```
