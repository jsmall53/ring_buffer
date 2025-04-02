#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

typedef struct ring_buffer_t
{
	size_t size;
	size_t write_pos;
	size_t read_pos;
	char* data;
} ring_buffer_t;


ring_buffer_t* init_ring_buffer(size_t size)
{
	ring_buffer_t* rb = (ring_buffer_t*)malloc(sizeof(ring_buffer_t));
	rb->size = size;
	rb->write_pos = 0;
	rb->read_pos = SIZE_MAX;
	rb->data = (char*)malloc(sizeof(char) * size);
	return rb;
}

void delete_ring_buffer(ring_buffer_t* buf)
{
	if (buf == NULL)
		return;

	if (buf->data != NULL)
	{
		free(buf->data);
		buf->data = NULL;
	}

	buf->write_pos = 0;
	buf->read_pos = 0;
	buf->size = 0;
}

size_t insert_ring_buffer(ring_buffer_t* buf, char* pBytes, size_t count)
{
	if (buf == NULL || buf->data == NULL || pBytes == NULL || count == 0)
		return 0; // probably need some kind of error
	
	if (buf->read_pos == SIZE_MAX)
		buf->read_pos = 0;

	int i = 0;
	while(count--)
	{
		buf->data[buf->write_pos++] = pBytes[i];
		buf->write_pos %= buf->size;
		// ring buffer needs to be serviced! dont overwrite data already existing in the buffer.
		if (buf->write_pos == buf->read_pos)
			break;
		i++;
	}
	return i;
	
}

size_t read_ring_buffer(ring_buffer_t* buf, char* pByte)
{
	assert(pByte != NULL);
	if (buf == NULL || buf->data == NULL || buf->size == 0)
		return 0;

	if (buf->read_pos == buf->write_pos || buf->read_pos == SIZE_MAX) // nothing to read yet
		return 0;
	
	*pByte = buf->data[buf->read_pos++];
	buf->read_pos %= buf->size;
	return 1;
}

void print_ring_buffer(ring_buffer_t* buf)
{
	if (buf == NULL)
		return;
	printf("Ring Buffer:\n\tsize: %i\n\twrite_pos: %i\n\tread_pos: %i\n\n", buf->size, buf->write_pos, buf->read_pos);
}

char* generate_char_string(size_t size)
{
	if (size == 0)
		return NULL;

	char* string = (char*)malloc(sizeof(char) * size + 1);	
	for (int i = 0; i < size; i++)
	{
		string[i] = 'a' + (i % 26);
	}
}

int main(int argc, char* argv[])
{
	ring_buffer_t* buf = init_ring_buffer(26);
	char* to_write = (char*)malloc(sizeof(char) * 4);
	to_write = "abc\t";
	for (int i = 0; i < 26; i++)
	{
		size_t bytes_written = insert_ring_buffer(buf, to_write, 3);
		size_t bytes_read = 0;
		char byte;
		printf("================\n");
		print_ring_buffer(buf);
		while(read_ring_buffer(buf, &byte))
		{
			//print_ring_buffer(buf);
			bytes_read++;
			printf("%c ", byte);
		}
		printf("\n");
		print_ring_buffer(buf);
		if (bytes_written != bytes_read)
		{
			printf("%d %d\n", bytes_written, bytes_read);
			fflush(stdout);
			assert(bytes_written == bytes_read);
		}
	}

	delete_ring_buffer(buf);
	return 0;
}
