#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include <assert.h>

#define MAX_BUF 500
char teststr[] = "The quick brown fox jumped over the lazy dog.";
char buf[MAX_BUF];

int
main(int argc, char * argv[])
{
	/* Notes:
         * Do not print to the console (ever!) Except:
         *   1. A test needs to be done by the eye. If so print what it is expected before, doing the test.
         *   2. An error has occurred. It should exit(1) after the test failed.
         *   3. ALL test are done.
         * 
         * The following is planning for the tests that we should do.
         * Planning was done on the basis of Edwin and Shanush's code. Testcases may not have been covered
         * So please add more.
         *
         * Make sure to delete test.file in your root folder before starting the test
         */


        int fd, r, i, j , k;
	(void) argc;
	(void) argv;

        /* Planning
         * Write to the stdout, stderr - we should see output on the screen
         */
	printf("\n**********\n* File Tester\n");

        printf("\nTEST BY EYE: should say ... \"Output to stdout\"\n");
	snprintf(buf, MAX_BUF, "Output to stdout\n");
	r = write(1, buf, strlen(buf));
        assert(r == 17 && strerror(errno));
        printf("\nTEST BY EYE: should say ... \"Output to stderr\"\n");
	snprintf(buf, MAX_BUF, "Output to stderr\n");
	r = write(2, buf, strlen(buf));
        assert(r == 17 && strerror(errno));

        /*

open()
Checking errors:
- open non-existent file (without O_CREAT)
- EFAULT - filename was a bad pointer - give NULL to open's filename argument.
*/

        fd = open("test.file", O_WRONLY);
        assert(fd == -1);
        assert(errno == ENOENT && strerror(errno));

        fd = open(NULL, O_RDONLY);
        assert(fd == -1);
        assert(errno == EFAULT && strerror(errno) );

        /*

open with write (and create)
write()
- same as given test
- zero buffer length
- null buffer
- invalid fd pointer

*/
	fd = open("test.file", O_WRONLY | O_CREAT );
	if (fd < 0) {
		printf("ERROR opening file: %s\n", strerror(errno));
		exit(1);
	}

	r = write(fd, teststr, strlen(teststr));
	assert(r == (int)strlen(teststr) && strerror(errno));
	if (r < 0) {
		printf("ERROR writing file: %s\n", strerror(errno));
		exit(1);
	}

	// Zero buffer length
        r = write(fd, teststr, 0);
	assert(r == 0 && strerror(errno));
        if (r < 0) {
		printf("ERROR writing file: %s\n", strerror(errno));
		exit(1);
	}

        // NULL buffer
        r = write(fd, NULL, strlen(teststr));
        assert(r == -1);  
        assert(errno == EFAULT && strerror(errno));

        // invalid fd pointer
        r = write(23, teststr, strlen(teststr));
        assert(r == -1);
        /*

close file

open same file with write (and O_EXCL)
- get error

close file

open file with append
- add to the file and see if the whole thing is there.

close

open with read
read()
- write to it and fail
- same as given test.
- null buffer, zero buffer length, invalid fd pointer

close

open same file with write, create, trunc
- all writing above should be gone
- replace with new text

close

open a file read & write
- be able to seek, read, write new stuff from beginish, read again.
- SEEK_SET for above
- SEEK_SET with negative pos : fail with EINVAL
- SEEK_CUR, add 5 and read two characters
        with negative pos
- SEEK_END, negative position - should fail

- SEEK_END, positive pos - write to it
    check for zeros gaps

- SEEK_NOEXISTANT - should fail with EINVAL

dup2
dup2 file to stderr
print something to stderr.
    nothing should show on stdout
    should be printed onto file
write to stderr for same above test


- newfd doesn't exist
- oldfd don't exist
- newfd is invalid


Maybe in a separate program:
opening OPEN_MAX number of files, OPEN_MAX + 1 should fail. (EMFILE)*
   */

	printf("\n**********\n* File Tester\n");

	snprintf(buf, MAX_BUF, "**********\n* write() works for stdout\n");
	write(1, buf, strlen(buf));
	snprintf(buf, MAX_BUF, "**********\n* write() works for stderr\n");
	write(2, buf, strlen(buf));

	printf("**********\n* opening new file \"test.file\"\n");
	fd = open("test.file", O_RDWR | O_CREAT );
	printf("* open() got fd %d\n", fd);
	if (fd < 0) {
		printf("ERROR opening file: %s\n", strerror(errno));
		exit(1);
	}

	printf("* writing test string\n");
	r = write(fd, teststr, strlen(teststr));
	printf("* wrote %d bytes\n", r);
	if (r < 0) {
		printf("ERROR writing file: %s\n", strerror(errno));
		exit(1);
	}

	printf("* writing test string again\n");
	r = write(fd, teststr, strlen(teststr));
	printf("* wrote %d bytes\n", r);
	if (r < 0) {
		printf("ERROR writing file: %s\n", strerror(errno));
		exit(1);
	}
	printf("* closing file\n");
	close(fd);

	printf("**********\n* opening old file \"test.file\"\n");
	fd = open("test.file", O_RDONLY);
	printf("* open() got fd %d\n", fd);
	if (fd < 0) {
		printf("ERROR opening file: %s\n", strerror(errno));
		exit(1);
	}

	printf("* reading entire file into buffer \n");
	i = 0;
	do  {
		printf("* attemping read of %d bytes\n", MAX_BUF -i);
		r = read(fd, &buf[i], MAX_BUF - i);
		printf("* read %d bytes\n", r);
		i += r;
	} while (i < MAX_BUF && r > 0);

	printf("* reading complete\n");
	if (r < 0) {
		printf("ERROR reading file: %s\n", strerror(errno));
		exit(1);
	}
	k = j = 0;
	r = strlen(teststr);
	do {
		if (buf[k] != teststr[j]) {
			printf("ERROR  file contents mismatch\n");
			exit(1);
		}
		k++;
		j = k % r;
	} while (k < i);
	printf("* file content okay\n");

	printf("**********\n* testing lseek\n");
	r = lseek(fd, 5, SEEK_SET);
	if (r < 0) {
		printf("ERROR lseek: %s\n", strerror(errno));
		exit(1);
	}

	printf("* reading 10 bytes of file into buffer \n");
	i = 0;
	do  {
		printf("* attemping read of %d bytes\n", 10 - i );
		r = read(fd, &buf[i], 10 - i);
		printf("* read %d bytes\n", r);
		i += r;
	} while (i < 10 && r > 0);
	printf("* reading complete\n");
	if (r < 0) {
		printf("ERROR reading file: %s\n", strerror(errno));
		exit(1);
	}

	k = 0;
	j = 5;
	r = strlen(teststr);
	do {
		if (buf[k] != teststr[j]) {
			printf("ERROR  file contents mismatch\n");
			exit(1);
		}
		k++;
		j = (k + 5)% r;
	} while (k < 5);

	printf("* file lseek  okay\n");
	printf("* closing file\n");

	printf("Testing dup2\n");
	i = dup2(fd, 2);
	if (i < 0) {
		printf("HELP! %s\n", strerror(errno));
		exit(1);
	}
	snprintf(buf, MAX_BUF, "**********\n* this shouldn't show stderr\n");
	write(2, buf, strlen(buf));
	printf("dup2 worked?\n");

	dup2(1, fd);
	snprintf(buf, MAX_BUF, "**********\n* this show stderr\n");
	write(1, buf, strlen(buf));


	close(fd);

	return 0;
}
