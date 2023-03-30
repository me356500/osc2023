#include "cpio.h"
#include "uart.h"
#include "helper.h"

void init_cpio(char *address)
{
    cpio_base = address;
}

char check_magic_number(char *address)
{
    int size = string_length(CPIO_MAGIC_NUMBER_STRING);
    for (int i = 0; i < size; i++)
    {
        if (address[i] != CPIO_MAGIC_NUMBER_STRING[i])
        {
            return 0;
        }
    }
    return 1;
}

unsigned int get_number_from_ascii(char *address)
{
    unsigned int filesize = 0;
    for (int i = 0; i < SIZE_OF_FEILD; i++)
    {
        char value;
        if (address[i] >= '0' && address[i] <= '9')
        {
            value = address[i] - '0';
        }
        else if (address[i] >= 'A' && address[i] <= 'F')
        {
            value = address[i] - 'A' + 10;
        }
        else // should not happen but for safety
        {
            value = address[i];
        }

        filesize |= value << ((7 - i) * 16);
    }

    return filesize;
}

unsigned int get_remaining_padding(unsigned int size, unsigned int paddingSize)
{
    unsigned int remainder = size % paddingSize;
    if (remainder == 0)
    {
        return 0;
    }

    return paddingSize - remainder;
}

void list_files()
{
    char *base = cpio_base;

    while (check_magic_number(base))
    {
        base += SIZE_OF_MAGIC + SIZE_OF_FEILD * 6; // skip not used parts

        unsigned int fileSize = get_number_from_ascii(base);

        // padding NULL
        char remainingPadding = get_remaining_padding(fileSize, PADDING_MULTIPLE);
        fileSize += remainingPadding;

        base += SIZE_OF_FEILD * 5; // skip not used parts

        unsigned int nameSize = get_number_from_ascii(base);

        // padding NULL
        remainingPadding = get_remaining_padding((nameSize + SIZE_OF_MAGIC), PADDING_MULTIPLE);
        nameSize += remainingPadding;

        base += SIZE_OF_FEILD * 2; // skip not used parts

        if (string_compare(END_IDENTIFIER, base))
        {
            break;
        }

        // print pathname
        uart_puts(base);
        uart_newline();

        base += nameSize;
        base += fileSize;
    }
}

void print_file(char *command)
{
    char found = 0;
    char *filename = 0x0;
    string_split(command, ' ', filename);

    if (string_compare(NOW_DIRECTORY, filename) || string_compare(PREVIOUS_DIRECTORY, filename))
    {
        uart_puts("Could not print directory!\n");

        return;
    }

    char *base = cpio_base;

    while (check_magic_number(base))
    {
        base += SIZE_OF_MAGIC + SIZE_OF_FEILD * 6; // skip not used parts

        unsigned int fileSize = get_number_from_ascii(base);

        // padding NULL
        char remainingPadding = get_remaining_padding(fileSize, PADDING_MULTIPLE);
        fileSize += remainingPadding;

        base += SIZE_OF_FEILD * 5; // skip not used parts

        unsigned int nameSize = get_number_from_ascii(base);

        // padding NULL
        remainingPadding = get_remaining_padding((nameSize + SIZE_OF_MAGIC), PADDING_MULTIPLE);
        nameSize += remainingPadding;

        base += SIZE_OF_FEILD * 2; // skip not used parts

        if (string_compare(END_IDENTIFIER, base))
        {
            if (!found)
            {
                uart_puts("Could not find file: ");
                uart_puts(filename);
                uart_newline();
            }
            break;
        }

        if (!string_compare(filename, base))
        {
            base += nameSize;
            base += fileSize;
            continue;
        }

        found = 1;

        uart_puts("Filename: ");
        uart_puts(base);
        uart_newline();
        base += nameSize;

        uart_puts(base);
        uart_newline();
        base += fileSize;
    }
}