      #include <termios.h>
      #include <stdio.h>
      #include <unistd.h>
      #include <fcntl.h>
      #include <signal.h>
      #include <stdlib.h>
      #include <time.h>  



      #define MODEMDEVICE "/dev/ttyS1"
      #define FALSE 0
      #define TRUE 1
        
      volatile int STOP=FALSE;
      struct time_encoder {
	  struct timespec timestamp;
	  char encoder_count[60];
      };
      volatile int count;
      volatile struct time_encoder time_encoder_array[1000];
      volatile int read_USB = -1;
      int fd;
      char read_buffer_USB[50];



      void signal_handler_IO (int status);   /* definition of signal handler */

        
      int main()
      {
     
        struct termios oldtio,newtio;
        struct sigaction saio;           /* definition of signal action */
	int written_USB = -1;

        
        /* open the device to be non-blocking (read will return immediatly) */
        fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd <0) {perror(MODEMDEVICE); exit(-1); }
        
        /* install the signal handler before making the device asynchronous */
        saio.sa_handler = signal_handler_IO;
	sigemptyset(&saio.sa_mask);
        saio.sa_flags = 0;

        sigaction(SIGIO,&saio,NULL);
          
        /* allow the process to receive SIGIO */
        fcntl(fd, F_SETOWN, getpid());
        /* Make the file descriptor asynchronous (the manual page says only 
           O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
        fcntl(fd, F_SETFL, FASYNC);
        
        tcgetattr(fd,&oldtio); /* save current port settings */
        /* set new port settings for canonical input processing */
	  /* set the BSD style raw mode */
	cfmakeraw (&newtio);
	newtio.c_cflag |= CREAD | CLOCAL;



	/* only need 1 stop bit */
	newtio.c_cflag &= ~CSTOPB;

	/* no hardware flowcontrol */
	newtio.c_cflag &= ~CRTSCTS;

	/* set the baud rate */
	cfsetspeed (&newtio,B115200);

	
	/* set the attributes for tuning read() */
	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0 ;
	cfsetspeed (&newtio, B115200);
        tcflush(fd, TCIFLUSH);
	tcsetattr (fd, TCSAFLUSH, &newtio);


	
	written_USB = write (fd, "^ECHOF 1\r", 9);
	if (written_USB < 0){
	    puts ("error writing to serial port");
	    exit (EXIT_FAILURE);
	}
	written_USB = -1;


         
        /* loop while waiting for input */ 
        while (STOP==FALSE) {
	/* giving a speed */
	    written_USB = write (fd, "!P 1 1000\r", 10);
	    if (written_USB < 0){
		perror("write_USB");
		exit (EXIT_FAILURE);
	    }
	    
	    usleep(1000);
	    
	    }
        
        /* restore old port settings */
	tcsetattr(fd,TCSANOW,&oldtio);



	    
	return EXIT_SUCCESS;
      }
        
      void signal_handler_IO (int status)
      {
	  clock_gettime(CLOCK_MONOTONIC, (struct timespec *) &time_encoder_array[count].timestamp);
	  
	  if (read (fd, (void *) &time_encoder_array[count].encoder_count, 49)) {
	      time_encoder_array[count].encoder_count[read_USB] = 0;
	  }
	  else{
	      perror("read USB");
	      exit (EXIT_FAILURE);
	  }
	  
	  count = count + 1;
	  if (count == 999)
	      STOP = TRUE;
      }
    



/* writes the timespec and the array in the signal handler */

/* TODO: get the difference in timespec, store it in a file */
