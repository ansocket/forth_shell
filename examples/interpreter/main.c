#include "stdio.h"
#include "forth.h"
#include "vm.h"
#include "forth_dict.h"
#include "forth_inter.h"

#if __WIN32
#include "conio.h"
#else
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO
#endif
uint8_t ram[8192];
size_t stack[256/sizeof(size_t)];
size_t rstack[256/sizeof(size_t)];
vm_t vm;
void forth_emit(vm_t* vm)
{
    putchar((char)*vm->sp++);
}
void forth_key(vm_t* vm)
{
#if __WIN32
    *(--vm->sp) = getch();
#else
    *(--vm->sp) = getchar();
#endif
}

void custom_function(vm_t* vm)
{
    vm_ops_t output = (vm_ops_t)*(forth_dict_get_text_ptr(forth_search(vm, "OUTPUT")) + 1);
    char buffer[] = "Я вообще не связанная ни с чем функция\n";
    *--vm->sp =  sizeof(buffer);
    *--vm->sp =  (size_t)buffer;
    output(vm);
}
int main()
{
#if !__WIN32
    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    /*now the settings will be copied*/
    newt = oldt;

    /*ICANON normally takes care that one line at a time will be processed
    that means it will return if it sees a "\n" or an EOF or an EOL*/
    newt.c_lflag &= ~(ICANON | ECHO);          

    /*Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately. */
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
#endif
    vm_init(&vm,ram,8192,stack,256/sizeof(size_t),rstack,256/sizeof(size_t));
    
    forth_interpreter_init(&vm,forth_key,forth_emit);
    forth_add_custom_function(&vm, "CUSTOM", custom_function);
    forth_error_t err = FORTH_ERR_OK;
    while(err == FORTH_ERR_OK)
    {
        err = forth_interpreter_process(&vm);
    }
#if !__WIN32
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
#endif
}