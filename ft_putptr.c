#include<unistd.h>
int ft_putnbr_base(unsigned long nbr, char *base);
int ft_putptr(void *ptr)
{
	unsigned long address;
	address = (unsigned long)ptr;
	write(1, "0x", 2);
	int count = ft_putnbr_base(address , "0123456789ABCDEF");
	return (count + 2);
}


#include <unistd.h>
#include<stdio.h>

int main(void)
{
    int x = 42;
    char c = 'A';
    char *str = "Hello";
    int n = ft_putptr(str);
    printf("\n");
    printf("%d", n);
    return (0);
}

