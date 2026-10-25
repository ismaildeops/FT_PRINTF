#include <stdarg.h>
#include <unistd.h>
#include<stdlib.h>
#include "Libft/libft.h"

static int count_len(int n)
{
    int len = 0;
    long nb = n;

    if (nb == 0)
        return (1);
    if (nb < 0)
    {
        len++;
        nb = -nb;
    }
    while (nb > 0)
    {
        nb /= 10;
        len++;
    }
    return (len);
}
int ft_printf(const char *format, ...)
{
    va_list ap;
    int count = 0;
    int i = 0;
    int nb;
    char *s;
    char c;
    if (!format)
        return (-1);
    va_start(ap, format);
    while (format[i])
    {
        if (format[i] == '%')
        {
            if (format[i + 1] == '\0')
                break;
            if (format[i + 1] == 'd' || format[i + 1] == 'i')
            {
                nb = va_arg(ap, int);
                ft_putnbr_fd(nb, 1);
                count = count + count_len(nb);
            }
            if(format[i + 1] == 'c')
            {
                c = va_arg(ap, int);
                write(1,&c,1);
                count++;
            }
            if(format[i+1] == 's')
            {
                s = va_arg(ap, char *);
                ft_putstr_fd(s,1);
                count = count + ft_strlen(s);
            }
            if(format[i + 1] == '%')
            {
                write(1,"%",1);
                count++;
            }
            if(format[i+1] == 'u')
            {
                nb = (unsigned int)nb;
                nb = va_arg(ap,unsigned int);

            }
            i = i + 2;
        }
        else
        {
            write(1, &format[i], 1);
            count++;
            i++;
        }
    }
    va_end(ap);
    return (count);
}

int main()
{
    ft_printf("my name %s and my age is %d\n", "ismail", 19);
}

/*
• %p The void * pointer argument has to be printed in hexadecimal format.
• %u Prints an unsigned decimal (base 10) number.
• %x Prints a number in hexadecimal (base 16) lowercase format.
• %X Prints a number in hexadecimal (base 16) uppercase format.
*/