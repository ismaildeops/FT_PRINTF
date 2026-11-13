#include<unistd.h>
int ft_putnbr_base(unsigned long nbr, char *base)
{
	int	count;
	count = 0;
	if(nbr >= 16)
	{	
		ft_putnbr_base(nbr / 16, base);
		count++;
	}
	int n = nbr % 16;
	write(1,&base[n],1);
	return (count + 1);	
}

