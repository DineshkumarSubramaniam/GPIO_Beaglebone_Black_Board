/* Setting the gpio_60 to blink using an user level program */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


int main()
{
	int direction_fd, value_fd;
	char buffer[2];

	// Open direction file for LED pin
	direction_fd = open("/sys/class/gpio/gpio60/direction", O_WRONLY);
	if(direction_fd < 0)
	{
		perror("Error opening direction file");
		return 1;
	}

	// Set direction of LED pin as output
	write(direction_fd, "out", 3);
	close(direction_fd);

	// Open value file for LED pin
	value_fd = open("/sys/class/gpio/gpio60/value", O_WRONLY);
	if(value_fd < 0)
	{
		perror("Error opening value file");
		return 1;
	}
for (int i=0; i<10 ; i++)
{
	// Set value of LED pin as high
	buffer[0] = '1';
	write(value_fd, buffer, 1);
        sleep(3);
	
	// Set value of LED pin as low
	buffer[0] = '0';
	write(value_fd, buffer, 1);
        sleep(3);
}

	close(value_fd);
	return 0;
}
