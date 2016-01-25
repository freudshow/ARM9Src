# include <stdio.h>
# include <stdlib.h>
# include <termio.h>
# include <unistd.h>
# include <fcntl.h>
# include <getopt.h>
# include <time.h>
# include <errno.h>
# include <string.h>

static void Error(const char *Msg) {
	fprintf(stderr, "%s\n", Msg);
	fprintf(stderr, "strerror() is %s\n", strerror(errno));
	exit(1);
}
static void Warning(const char *Msg) {
	fprintf(stderr, "Warning: %s\n", Msg);
}

static int SerialSpeed(const char *SpeedString) {
	int SpeedNumber = atoi(SpeedString);
#   define TestSpeed(Speed) if (SpeedNumber == Speed) return B##Speed
	TestSpeed(1200);
	TestSpeed(2400);
	TestSpeed(4800);
	TestSpeed(9600);
	TestSpeed(19200);
	TestSpeed(38400);
	TestSpeed(57600);
	TestSpeed(115200);
	TestSpeed(230400);
	Error("Bad speed");
	return -1;
}

static void PrintUsage(void) {

	fprintf(stderr, "comtest - interactive program of comm port\n");
	fprintf(stderr, "press [ESC] 3 times to quit\n\n");

	fprintf(stderr,
			"Usage: comtest [-d device] [-t tty] [-s speed] [-7] [-c] [-x] [-o] [-h]\n");
	fprintf(stderr, "         -7 7 bit\n");
	fprintf(stderr, "         -x hex mode\n");
	fprintf(stderr, "         -o output to stdout too\n");
	fprintf(stderr, "         -c stdout output use color\n");
	fprintf(stderr, "         -h print this help\n");
	exit(-1);
}

static inline void WaitFdWriteable(int Fd) {
	fd_set WriteSetFD;
	FD_ZERO(&WriteSetFD);
	FD_SET(Fd, &WriteSetFD);
	if (select(Fd + 1, NULL, &WriteSetFD, NULL, NULL) < 0) {
		Error(strerror(errno));
	}

}

int main(int argc, char **argv) {
	int CommFd, TtyFd;

	struct termios TtyAttr;
	struct termios BackupTtyAttr;

	int DeviceSpeed = B2400;
	int TtySpeed = B2400;
	int ByteBits = CS8;
	const char *DeviceName = "/dev/ttyS1";
	const char *TtyName = "/dev/tty";
	const char *ucBeginLoge = "HI! This is jame ok ";
	int OutputHex = 0;
	int OutputToStdout = 0;
	int UseColor = 0;

	opterr = 0;
	for (;;) {
		int c = getopt(argc, argv, "d:s:t:7xoch");
		if (c == -1)
			break;
		switch (c) {
		case 'd':
			DeviceName = optarg;
			break;
		case 't':
			TtyName = optarg;
			break;
		case 's':
			if (optarg[0] == 'd') {
				DeviceSpeed = SerialSpeed(optarg + 1);
			} else if (optarg[0] == 't') {
				TtySpeed = SerialSpeed(optarg + 1);
			} else
				TtySpeed = DeviceSpeed = SerialSpeed(optarg);
			break;
		case 'o':
			OutputToStdout = 1;
			break;
		case '7':
			ByteBits = CS7;
			break;
		case 'x':
			OutputHex = 1;
			break;
		case 'c':
			UseColor = 1;
			break;
		case '?':
		case 'h':
		default:
			PrintUsage();
		}
	}
	if (optind != argc)
		PrintUsage();

	CommFd = open(DeviceName, O_RDWR, 0);
	if (CommFd < 0)
		Error("Unable to open device");
	if (fcntl(CommFd, F_SETFL, O_NONBLOCK) < 0)
		Error("Unable set to NONBLOCK mode");

	memset(&TtyAttr, 0, sizeof(struct termios));
	TtyAttr.c_iflag = IGNPAR;
	TtyAttr.c_cflag = DeviceSpeed | HUPCL | ByteBits | CREAD | CLOCAL;
	TtyAttr.c_cc[VMIN] = 1;

	if (tcsetattr(CommFd, TCSANOW, &TtyAttr) < 0)
		Warning("Unable to set comm port");

#if 0
	TtyFd = open(TtyName, O_RDWR | O_NDELAY, 0);
	if (TtyFd < 0)
	Error("Unable to open tty");

	TtyAttr.c_cflag = TtySpeed | HUPCL | ByteBits | CREAD | CLOCAL;
	if (tcgetattr(TtyFd, &BackupTtyAttr) < 0)
	Error("Unable to get tty");

	if (tcsetattr(TtyFd, TCSANOW, &TtyAttr) < 0)
	Error("Unable to set tty");
#endif
	write(CommFd, ucBeginLoge, 20);
	for (;;) {
		unsigned char Char = 0;
		fd_set ReadSetFD;

		FD_ZERO(&ReadSetFD);
		FD_SET(CommFd, &ReadSetFD);
		//FD_SET( TtyFd, &ReadSetFD);
#   define max(x,y) ( ((x) >= (y)) ? (x) : (y) )
		if (select(CommFd + 1, &ReadSetFD, NULL, NULL, NULL) < 0) {
			Error(strerror(errno));
		}
#   undef max

		if (FD_ISSET(CommFd, &ReadSetFD)) {
			while (read(CommFd, &Char, 1) == 1) {
				WaitFdWriteable(CommFd);
				if (write(CommFd, &Char, 1) < 0) {
					Error(strerror(errno));
				}
			}
		}
	}

	ExitLabel:

	return 0;
}
