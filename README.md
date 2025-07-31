# FORTH_SHELL

Another FORTH interpreter for embedded systems. Main goals:

- Easy porting 
- Easy using
- Low RAM usage

Main idea is to introduce a shell tool with scripting for microcontroller systems. No malloc, no hardware-dependences. 

### FORTH

FORTH - programming language, which born in 1971 as an alternative for programming in assembler. Here we have NOT-STANDART FORTH. It's just a script language which looks like FORTH. I think. I hope. Just my vision of it.

### Hello world

``` 
Forth_shell by ansocket.
 
> ." Hello world"
Hello world OK 
> 
```

### Using
I realize this project just as shell, like bash, for microcontrollers. So here we can add custom functions on c-language for general purposes. Easy and painless.
```
void custom_function(vm_t* vm)
{
    vm_ops_t output = (vm_ops_t)*(forth_dict_get_text_ptr(forth_search(vm, "OUTPUT")) + 1);
    char buffer[] = "I'm the custom function\n";
    *--vm->sp =  sizeof(buffer);
    *--vm->sp =  (size_t)buffer;
    output(vm);
}

forth_add_custom_function(&vm, "CUSTOM", custom_function);
```
This functions can work with main stacks to get arguments, put some values and output text. No restrictions.

You can explore all words (build-in and run-time created) by using `WORDS`.
### Porting

Look in examples/interpreter.

You just need to make two io functions: 
- KEY - get a new character from input stream
- EMIT - put character in output stream