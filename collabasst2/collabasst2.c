#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>

#include <assert.h>

#define MAX_BUF 500
char teststr[] = "The quick brown fox jumped over the lazy dog.";
char buf[MAX_BUF];

int do_read(int fd2);

int do_read(int fd2) {
	int i,r;
	for (i = 0; i < MAX_BUF; i++) {
		buf[i] = 0;
	}
	i = 0;
	do  {
		r = read(fd2, &buf[i], MAX_BUF - i);
		i += r;
	} while (i < MAX_BUF && r > 0);

	return i;
}

int
main(int argc, char * argv[])
{
	/* Notes:
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
	printf("------**** File Tester ****--------\n");

        printf("----------- Testing stdout ------------\n");
        printf("\nTEST BY EYE: should say ... \"Output to stdout\"\n");
	snprintf(buf, MAX_BUF, "Output to stdout\n");
	r = write(1, buf, strlen(buf));
        assert(r == 17 && strerror(errno));
        printf("Passed\n----------- Testing stderr -------------\n");
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
        printf("Passed\n----------- Opening file that doesn't exist with write -------------\n");

        fd = open("test.file", O_WRONLY, 0666);
        assert(fd == -1);
        assert(errno == ENOENT && strerror(errno));
        close(fd);

        printf("Passed\n----------- Opening with filename = NULL -------------\n");
        fd = open(NULL, O_RDONLY, 0666);
        assert(fd == -1);
        assert(errno == EFAULT && strerror(errno) );
        close(fd);

        /*

open with write (and create)
write()
- same as given test
- zero buffer length
- null buffer
- invalid fd pointer

close file

*/
        printf("Passed\n----------- Opening file -------------\n");

	fd = open("test.file", O_WRONLY | O_CREAT, 0666);
	if (fd < 0) {
		printf("ERROR opening file: %s\n", strerror(errno));
		exit(1);
	}

        printf("Passed\n----------- Writing to the file -------------\n");
	r = write(fd, teststr, strlen(teststr));
	assert(r == (int)strlen(teststr) && strerror(errno));

        printf("Passed\n----------- Writing to the file with zero buffer length -------------\n");
	// Zero buffer length
        r = write(fd, teststr, 0);
	assert(r == 0 && strerror(errno));

        if (r < 0) {
		printf("ERROR writing file: %s\n", strerror(errno));
		exit(1);
	}



        printf("Passed\n----------- Writing to a file with invalid fd -------------\n");
        // invalid fd pointer
        r = write(23, teststr, strlen(teststr));
        assert(r == -1);


        printf("Passed\n----------- Closing file-------------\n");

        close(fd);

        /*


open same file with write (and O_EXCL)
- get error
*/


       fd = open("test.file", O_CREAT | O_EXCL);
       assert(fd == -1);
       assert(errno == EEXIST);
       close(fd);


        printf("Passed?\n----------- Opening exisiting file with 0_EXCL flag -------------\n");
        fd = open("test.file", O_WRONLY | O_CREAT | O_EXCL, 0666);
        assert(fd == -1);
        assert(errno == EEXIST);


        /*
open file with append
- add to the file and see if the whole thing is there.

close
*/





        printf("Passed\n----------- Appending to test file-------------\n");
        fd = open("test.file", O_WRONLY | O_APPEND, 0666);
        assert(fd != -1 && strerror(errno));
        r = write(fd, "HELLO", 5);
        assert(r == 5);

        printf("Still checking\n----------- reading test file-------------\n");
        int fd2 = open("test.file", O_RDONLY, 0666);
		i = do_read(fd2);

	if (r < 0) {
		printf("ERROR reading file: %s\n", strerror(errno));
		exit(1);
	}
	k = j = 0;
        char check_string[] = "The quick brown fox jumped over the lazy dog.HELLO";
	r = strlen(check_string);
        do {
				//printf("%x %x\n", buf[k], check_string[j]);
                if (buf[k] != check_string[j]) {
			printf("ERROR  file contents mismatch\n");
			exit(1);
		}
		k++;
		j = k % r;
	} while (k < i);


        /*

open with read
read()
- write to it and fail
- null buffer, zero buffer length, invalid fd pointer

close
*/

        printf("Passed read and append\n----------- writing to a file when opened with read -------------\n");
        r = write(fd2, teststr, strlen(teststr));
        assert(r == -1);
        assert(errno == EBADF && strerror(errno));

        /*
open same file with write, create, trunc
- all writing above should be gone
- replace with new text

close

*/
        close(fd);

        printf("Passed\n----------- writing and truncate -------------\n");

        fd = open("test.file", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		printf("ERROR opening file: %s\n", strerror(errno));
		exit(1);
	}
/*		struct stat hi;
		fstat(fd, &hi);
		printf("size = %lld\n", hi.st_size); */
        r = write(fd, "HEE", 3);
        assert(r == 3 && strerror(errno));
        printf("\n----------- writing and truncate -------------\n");
        close(fd2);

        fd2 = open("test.file", O_RDWR, 0666);
        assert(fd2 != -1);
		do_read(fd2);
		//printf("buf = %s\n", buf);
		assert(strcmp(buf, "HEE") == 0);




        /*

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

*/
printf("\n----------- lseek, seek_set -------------\n");
		lseek(fd2, 2, SEEK_SET);
		r = write(fd2, "YOOKMANFSHFJKSDHFKJSHDFKHSDLKHFKJDSHFK", strlen("YOOKMANFSHFJKSDHFKJSHDFKHSDLKHFKJDSHFK"));
		assert(r == strlen("YOOKMANFSHFJKSDHFKJSHDFKHSDLKHFKJDSHFK") && strerror(errno));

		lseek(fd2, 0, SEEK_SET);
		for (i = 0; i < MAX_BUF; i++) {
			buf[i] = 0;
		}
		i = do_read(fd2);
		//printf("%s\n", buf);
		assert(strcmp(buf, "HEYOOKMANFSHFJKSDHFKJSHDFKHSDLKHFKJDSHFK") == 0);

		lseek(fd2, -10, SEEK_END);
		r = write(fd2, "yo", 2);
		assert(r == 2 && strerror(errno));
		lseek(fd2, 0, SEEK_SET);
		i = do_read(fd2);
		assert(strcmp(buf, "HEYOOKMANFSHFJKSDHFKJSHDFKHSDLyoFKJDSHFK") == 0);

		lseek(fd2, -10, SEEK_END);
		lseek(fd2, 2, SEEK_CUR);
		r = write(fd2, "mate", 4);
		assert(r == 4 && strerror(errno));

		lseek(fd2, 0, SEEK_SET);
		i = do_read(fd2);
		assert(strcmp(buf, "HEYOOKMANFSHFJKSDHFKJSHDFKHSDLyomateSHFK") == 0);

		r = lseek(fd2, -1, SEEK_SET);
		assert(r == -1 && errno == EINVAL);



		lseek(fd2, 0, SEEK_SET);
		i = do_read(fd2);
		//printf("%s\n", buf);

		//printf("%s\n", buf);
printf("\n----------- dup2 -------------\n");
printf("fd = %d\n", fd2);
		r = dup2(fd2, 10);
		assert(r != -1);
		r = lseek(10, -10, SEEK_END);
		assert(r != -1);
		r = write(10, "ello", 4);
		assert(r == 4 && strerror(errno));
		lseek(fd2, 0, SEEK_SET);
		i = do_read(fd2);
		//printf("%s\n", buf);
		assert(strcmp(buf, "HEYOOKMANFSHFJKSDHFKJSHDFKHSDLelloteSHFK") == 0);
	printf("fd = %d\n", fd2);
printf("HERE\n");
		r = dup2(23, fd2);
		assert(r == -1 && errno == EBADF);
printf("HERE\n");
		r = dup2(23, 40);
		assert(r == -1 && errno == EBADF);
printf("HERE\n");
		r = dup2(fd2, -1);
		assert(r == -1 && errno == EBADF);
printf("HERE\n");
		printf("Check if test.file has Hello at the end.\n");
printf("fd = %d\n", fd2);
		r = lseek(fd2, 0, SEEK_END);
		assert (r != -1 && strerror(errno));
		r = dup2(fd2, 1);
		assert (r != -1 && strerror(errno));
		printf("Hello\n");


		/*

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


	/*

        printf("Passed\n----------- Writing to the file with NULL buffer -------------\n");
        // NULL buffer
        r = write(fd, NULL, strlen(teststr));
        assert(r == -1);
        assert(errno == EFAULT && strerror(errno));

*/
	return 0;
}
