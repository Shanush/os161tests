#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#define MAX_BUF 256
char teststr[] = "This is a test made by Roger. This aims to test the error number handling of the program";
char buf[MAX_BUF];

void check_test(int fd, int expect);
void test_open(void);
void test_close(void);
void test_write(void);
void test_read(void);
void test_lseek(void);
void test_dup2(void);

void check_test(int fd, int expect) {
    if (fd != -1) {
        printf("   failture is not caught\n");
        return;
    }

    if (errno != expect) {
        printf("   errno %d while expect %d\n", errno, expect);
        return;
    }

    printf("   SUCCESS!\n");
    return;
}

int
main(int argc, char * argv[])
{
	(void) argc;
	(void) argv;

	printf("\n**************************\n");
    printf("Errno Handling Test\n");
    printf("***************************\n");

    test_open();
    test_close();
    test_write();
    test_read();
    test_lseek();
    test_dup2();
	return 0;
}

void test_open(void) {

	int fd;
	printf("open: Not a test.........");
	fd = open("fs_errno.txt", O_RDWR | O_CREAT);
    if (fd < 0) {
        printf("   FAILURE!\n");
    } else {
        printf("   SUCCESS!\n");
    }
    close(fd);

    printf("open: ENODEV test........");
    fd = open("kern:", O_RDONLY | O_CREAT);
    check_test(fd, ENODEV);

	printf("open: ENOENT test........");
	fd = open("testcases/file.txt", O_RDONLY | O_CREAT);
    check_test(fd, ENOENT);

	printf("open: ENOENT test........");
	fd = open("file.txt", O_RDONLY);
    check_test(fd, ENOENT);

	printf("open: EISDIR test........");
	fd = open("testbin/", O_WRONLY);
    check_test(fd, EISDIR);

	printf("open: EEXIST test........");
	fd = open("fs_errno.txt", O_RDONLY | O_CREAT | O_EXCL);
    check_test(fd, EEXIST);

	printf("open: EINVAL test........");
	fd = open("fs_errno.txt", -1 );
    check_test(fd, EINVAL);

	printf("open: EFAULT test........");
	fd = open(NULL, O_RDWR | O_CREAT );
    check_test(fd, EFAULT);

	printf("open: EMFILE test........");
    int fda[__OPEN_MAX - 3];
    for (int i = 0; i < __OPEN_MAX - 2; i++) {
        fda[i] = open("fs_errno.txt", O_RDONLY);
        if (fda[i] == -1) {
            printf("   failed %d open %dth file\n", errno, i);
            break;
        }
    }
	fd = open("fs_errno.txt", O_RDWR);
    check_test(fd, EMFILE);

}

void test_close(void) {

	int fd;
	printf("close: EBADF test........");
    for (int i = 3; i < __OPEN_MAX; i++) {
        fd = close(i);
        if (fd == -1) {
            printf("   failed %d close %dth file\n", errno, i);
            break;
        }
    }
	fd = close(3);
    check_test(fd, EBADF);

	printf("close: EBADF test........");
	fd = close(-1);
    check_test(fd, EBADF);
}

void test_write(void) {
	int fd, bufsiz, result;

    bufsiz = strlen(teststr) + 1;
	printf("write: Not a test........");
	fd = open("fs_errno.txt", O_WRONLY);
    result = write(fd, teststr, bufsiz);
    if (result == -1) {
        printf("   failed %d write file\n", errno);
    } else if (fd != 3) {
        printf("   failed fd = %d, expect 3\n", fd);
    } else {
        printf("   SUCCESS!\n");
    }

	printf("write: EBADF test........");
    result = write(-1, teststr, bufsiz);
    check_test(result, EBADF);

	printf("write: EBADF test........");
    result = write(4, teststr, bufsiz);
    check_test(result, EBADF);

    void *broken = NULL;
	printf("write: EFAULT test.......");
    result = write(3, broken, bufsiz);
    check_test(result, EFAULT);

    close(fd);

    fd = open("fs_errno.txt", O_RDONLY);
    printf("write: EBADF test........");
    result = write(3, teststr, bufsiz);
    check_test(result, EBADF);
    close(fd);
}

void test_read(void) {
    int fd, strsiz, result;

    strsiz = strlen(teststr) + 1;
	printf("read: Not a test.........");
	fd = open("fs_errno.txt", O_RDWR);
    result = read(fd, buf, strsiz);
    if (result == -1) {
        printf("   failed %d read file\n", errno);
    } else if (fd != 3) {
        printf("   failed fd = %d, expect 3\n", fd);
    } else if (result != strsiz) {
        printf("   read size %d mismatch %d\n", result, strsiz);
    } else if (strcmp(buf, teststr) != 0) {
        printf("   read content mismatch\n");
    } else {
        printf("   SUCCESS!\n");
    }
    close(fd);

	fd = open("fs_errno.txt", O_RDONLY);
	printf("read: EBADF test.........");
    result = read(-1, buf, 10);
    check_test(result, EBADF);

	printf("read: EBADF test.........");
    result = read(4, buf, 10);
    check_test(result, EBADF);

    void *broken = NULL;
	printf("read: EFAULT test........");
    result = read(3, broken, 10);
    check_test(result, EFAULT);
    close(fd);

    fd = open("fs_errno.txt", O_WRONLY);
    printf("read: EBADF test.........");
    result = read(3, buf, 10);
    check_test(result, EBADF);
    close(fd);
}

void test_lseek(void) {
    int fd, bufsiz, result, console;
    bufsiz = strlen(teststr) + 1;

	printf("lseek: Not a test........");
	fd = open("fs_errno.txt", O_RDWR);
    result = lseek(fd, -10, SEEK_END);
    read(fd, buf, bufsiz);
    if (result == -1) {
        printf("   failed %d seek file\n", errno);
    } else if (fd != 3) {
        printf("   failed fd = %d, expect 3\n", fd);
    } else if (result != 79) {
        printf("   seek size %d mismatch %d\n", result, 79);
    } else if (strcmp(buf, "e program") != 0) {
        printf("   seek content mismatch\n");
    } else {
        printf("   SUCCESS!\n");
    }
    close(fd);

    fd = open("fs_errno.txt", O_RDONLY);
    console = open("con:", O_RDWR);

	printf("lseek: EBADF test........");
    result = lseek(-1, 10, SEEK_SET);
    check_test(result, EBADF);

	printf("lseek: ESPIPE tes........");
    result = lseek(console, 10, SEEK_SET);
    check_test(result, ESPIPE);

	printf("lseek: EINVAL test.......");
    result = lseek(fd, 10, 4);
    check_test(result, EINVAL);

	printf("lseek: EINVAL test.......");
    lseek(fd, 45, SEEK_SET);
    result = lseek(fd, -46, SEEK_CUR);
    check_test(result, EINVAL);

	printf("lseek: EINVAL test.......");
    result = lseek(fd, -bufsiz-1, SEEK_END);
    check_test(result, EINVAL);

    printf("lseek: EOF test..........");
    result = lseek(fd, 1, SEEK_END);
    if (result == -1) {
        printf("   FAILURE!\n");
    } else {
        printf("   SUCCESS!\n");
    }
}

void test_dup2(void) {
    int fd, newfd, bufsiz, result;
    bufsiz = strlen(teststr) + 1;

	printf("dup2: Not a test.........");
	fd = open("fs_errno.txt", O_RDWR);
    newfd = __OPEN_MAX - 1;
    result = dup2(fd, newfd);
    read(fd, buf, 10);
    lseek(fd, -8, SEEK_END);
    close(fd);
    read(newfd, buf+10, bufsiz);
    if (result == -1) {
        printf("   failed %d dup2 file\n", errno);
    } else if (result != newfd) {
        printf("   failed result = %d, expect %d\n", result, newfd);
    } else if (strcmp(buf, "This is a program") != 0) {
        printf("   dup2 cursors are not synchronised\n");
    } else {
        printf("   SUCCESS!\n");
    }
    close(newfd);

	printf("dup2: EBADF test.........");
    result = dup2(fd, -1);
    check_test(result, EBADF);

    // Reallocate STDERR
    fd = open("fs_errno.txt", O_WRONLY);
    printf("dup2: STDERR reallocation");
    result = dup2(fd, 2);
    close(fd);
    if (result == -1) {
        printf("   FAILURE!\n");
    }
	snprintf(buf, MAX_BUF, "Congratulation, you've passed all the test cases!\n ----------------------------------------\nPlease remove this file if you rerun the test cases.\n Thanks for using Roger's pleb test cases\n");
	write(2, buf, strlen(buf));
    close(2);
    printf("   SUCCESS!\n");

    // Reallocate STDOUT -> STDIN
    printf("dup2: STDOUT reallocation");
    result = dup2(1, 0);
    if (result == -1) {
        printf("   FAILURE!\n");
    } else {
        close(1);
        snprintf(buf, 13, "   SUCCESS!\n");
        write(0, buf, 13);
    }
}
