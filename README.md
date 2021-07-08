# luarime

> simple implements, call librime with lua 

## Completion

- [x] call with lua5.3 
  - [x] function version
  - [x] function initial
  - [x] function finalize
  - [x] function process_key
  - [x] function context
  - [ ] function get_commit
  - [ ] function get_option
  - [ ] function set_option
- [ ] call with luajit(failed)


## Build Tool

- xmake

## Depend on

- lua/luajit
- librime

### ArchLinux

```
sudo pacman -Sy lua librime
```

## Build

```
xmake
```

## Test

```
make
```
