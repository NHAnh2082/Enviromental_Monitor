#include "DataConvert.h"

void reverse_string(char *str, int length)
{
	int i = 0, j = length - 1;
	
	while (i<j)
	{
		char tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
		i++;
		j--;
	}
}

int int_to_str(int32_t x, char *str, int is_signed)
{
	int i = 0;
	int isNegative = 0;

	if (x == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return i;
	}

	if (is_signed && (x<0))
	{
		isNegative = 1;
		x = -x;
	}

	while (x != 0)
	{
		int r = x % 10;
		str[i++] = r + '0';
		x /= 10;
	}

	if (isNegative)
	{
		str[i++] = '-';
	}
	else if (i == 1)  // N?u ch? có 1 ch? s? và là s? duong ? thêm '0'
	{
		str[i++] = '0';
	}

	str[i] = '\0';
	reverse_string(str, i);
	return i;
}

int float_to_str(float n, char *str, int afterpoint)
{
	int i = 0;
	
	if (n < 0)
	{
		str[i++] = '-';
		n = -n;
	}

	int ipart = (int)n;
	float fpart = n - (float)ipart;

	i += int_to_str(ipart, str + i, 0);
	str[i++] = '.';

	for (int j=0; j<afterpoint; j++) fpart *= 10;
	i += int_to_str((int)fpart, str + i, 0);

	str[i] = '\0';
	return i;
}

void convert(char *buffer, const char *format, ...)
{
	va_list args;
	va_start(args, format);

	char *p = buffer;
	const char *s;

	while (*format)
	{
		if (*format == '%')
		{
			format++;
		
			switch (*format)
			{
				case 'd': 
				{
					int val = va_arg(args, int);
					p += int_to_str(val, p, 1);
					break;
				}
				
				case 'u': 
				{
					unsigned int val = va_arg(args, unsigned int);
					p += int_to_str(val, p, 0);
					break;
				}
				
				case 'f': 
				{
					float val = va_arg(args, double); // float promoted to double
					p += float_to_str(val, p, 2);
					break;
				}
				
				case 's': 
				{
					s = va_arg(args, char*);
					while (*s) *p++ = *s++;
					break;
				}
				
				case 'c': 
				{
					char c = (char)va_arg(args, int);
					*p++ = c;
					break;
				}
				
				default:
				{
					*p++ = '%';
					*p++ = *format;
				}
			}
		}
		else
		{
			*p++ = *format;
		}
		format++;
	}

	*p = '\0';
	va_end(args);
}	
