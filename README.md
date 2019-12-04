# so_progetto_paolo_alessio
Paolo -> CrazyLegoMind
Alessio -> alessio123

testing merging branches
Insert TODO file!

General:
- All projects should be hosted on github/gitlab or similars.

- Each student performs commits using his own account.

- We want to see the individual commits.

- The contribution of each wach member in the group to the development
  should be made.  Comment your sections in the source with some
  initials or nickname

  Example:

  //gg: here we clear the list of connections and
  //    do some other stuff
  List_clear(&connections);
  List_pushBack(&connections, stuff_added);



Arduino:
- All projects involvig Arduino should include some sort of serial
  communication with the host.
  The protocol should be binary, and data integrity should be ensured by
  a checksum.

  Meaning: a project of arduino includes a "server" (arduino part),
  and a client (PC part). These are two different programs.
  No cutecom needed for using arduino.

- Use of resources:
  Use at least one interrupt a timer and an I/O port

---- CHOSEN PROJECT ----

1. Oscilloscope [2]:
   Arduino: timed sampling of up to 8 channels on the ADC,
            and streaming on PC

            Possibility of configuring (from PC)
            - which channel(s) are being sampled
            - the sampling frequency

            Possibility of operating in two modes
            - continuous sampling: (each sample is sent
              to the PC as it becomes ready)
            - buffered mode: a set of samples are stored 
              locally and then sent bulk

            Implementation of "trigger"
            (a condition that decides when to start sampling, in buffered mode)

            Serial communication should be asynchronous (use an interrupt
            on the UART)
   
   PC: host program controlling the oscilloscope.
       Output might be done on text files and displayed with gnuplot.
       
