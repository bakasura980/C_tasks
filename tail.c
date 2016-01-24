#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h> 
#include <string.h>
#include <stdio.h>

//Write buffer to stdout
bool write_buffer(char* buffer,int offsets){
	while(offsets < strlen(buffer)){
		char* print_buffer = malloc(1);
		print_buffer[0] = buffer[offsets];
		int status = write(STDOUT_FILENO, print_buffer, 1);
		if(status < 0)
		{
			return false;
		}
		offsets++;
	}
	free(buffer);
	return true;
}
//Count offsets from first symbol(if rows are more than ten),because i have to know how
//many rows i have to skip
int offsets(int rows, char* buffer){
	int i, index = 0;
	if(rows <= 10)
		return index;
	else{
		for(i = 0; i < rows - 10;)
		{
			index++;
			if(buffer[index] == '\n')
			{
				i++;
			}
		}
		return index + 1;
	}
}
//Make a dinamic buffer(resize buffer every time,when there is one more symbol)
char *resize_buffer(char* helper_buffer,  int counter, char* buffer){
	helper_buffer[counter] = buffer[0];
	char *old_buffer = helper_buffer;
	char *new_buffer = malloc(strlen(helper_buffer)+1);
	strcpy(new_buffer, old_buffer);
	helper_buffer = new_buffer;
	free(old_buffer);
	return helper_buffer;
}
//Call the error message
void exist_permission_error(char* argv){
	char string [50] = "tail: cannot open '";
	char string1 [] =  "' for reading";
	strcat(string, argv);
	strcat(string, string1);
	perror(string);
}

int main(int argc, char *argv[]){
	
	int i = 0;
	if(argc == 1){
			perror("No file");
			return 0;
	}
	
	for (i = 1; i < argc ; i++){
		if(access( argv[i], F_OK ) == -1 && strcmp(argv[i],"-") != 0) {
			exist_permission_error(argv[i]);
			return 0;
		}else if(access(argv[i], R_OK) != 0 && strcmp(argv[i],"-") != 0)
		{
			exist_permission_error(argv[i]);
			return 0;
		}else {
			char *buffer = malloc(1);
			
			char *helper_buffer = malloc(1);
			
			int row_counter = 0, counter = 0;
			int cycle_counter = 0;		
			if(strcmp(argv[i],"-") != 0){
				int fd = open(argv[i], O_RDONLY); 	
				ssize_t read_return_value =  read(fd, buffer, 1);
				while(read_return_value > 0)
				{
						cycle_counter++;
						if(buffer[0] == '\n')
						{
							row_counter++;
						}
						helper_buffer = resize_buffer(helper_buffer, counter, buffer);
						counter++;
						read_return_value =  read(fd, buffer, 1);
				}
				helper_buffer[cycle_counter - 1] = '\0';
				if(close(fd) < 0)
				{
					perror("File does not close");
					return 0;
				}
				free(buffer);
			}else{
				while(read(STDIN_FILENO, buffer, 1) > 0)
				{
					cycle_counter++;
					if(buffer[0] == '\n')
					{
						row_counter++;
					}
					helper_buffer = resize_buffer(helper_buffer, counter, buffer);
					counter++;
				}
				helper_buffer[cycle_counter - 1] = '\0';
			}
			if(argc > 2)
			{	
				char label[20] = "==> ";
				char label1[] = " <==\n";
				strcat(label, argv[i]);
				strcat(label, label1);
				write(STDOUT_FILENO, label, strlen(label));
			}		

			if(write_buffer(helper_buffer, offsets(row_counter, helper_buffer)) == false)
			{
				perror("You cant write");
				return 1;
			}
			puts("");
		}
	}	
	return 0;
}
