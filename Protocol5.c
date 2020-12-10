#define MAX SEQ 7
typedef enum
{
    frame arrival,
    cksum err,
    timeout,
    network layer ready
} event type;
#include "Protocol.h"

static boolean between(seq_nr a, seq_nr b, seq_nr c)
{ /* Return true if a <= b < c circularly; false otherwise. */
    if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
        return (true);
    else
        return (false);
}

static void send data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[])
{                                                       /* Construct and send a data frame. */
    frame s;                                            /* scratch variable */
    s.info = buffer[frame_nr];                          /* insert packet into frame */
    s.seq = frame_nr;                                   /* insert sequence number into frame */
    s.ack = (frame expected + MAX SEQ) % (MAX SEQ + 1); /* piggyback ack */
    to physical layer(&s);                              /* transmit the frame */
    start timer(frame_nr);                              /* start the timer running */
}

void protocol5(void)
{
    seq_nr next frame_to_send;  /* MAX SEQ > 1; used for outbound stream */
    seq_nr ack expected;        /* oldest frame as yet unacknowledged */
    seq_nr frame expected;      /* next frame expected on inbound stream */
    frame r;                    /* scratch variable */
    packet buffer[MAX SEQ + 1]; /* buffers for the outbound stream */
    seq_nr nbuffered;           /* number of output buffers currently in use */
    seq_nr i;                   /* used to index into the buffer array */
    event type event;
    enable network layer(); /* allow network layer ready events */
    ack expected = 0;       /* next ack expected inbound */
    next frame_to_send = 0; /* next frame going out */
    frame expected = 0;     /* number of frame expected inbound */
    nbuffered = 0;          /* initially no packets are buffered */
    while (true)
    {
wait for event(&event); /* four possibilities: see event type above */
SEC.3.4 SLIDING WINDOW PROTOCOLS 237 switch (event)
{
case network layer ready: /* the network layer has a packet to send */ /* Accept, save, and transmit a new frame. */
    from network layer(&buffer[next frame_to_send]);                   /* fetch new packet */
    nbuffered = nbuffered + 1;                                         /* expand the sender’s window */
    send data(next frame_to_send, frame expected, buffer);             /* transmit the frame */
    inc(next frame_to_send);                                           /* advance sender’s upper window edge */
    break;
case frame arrival:          /* a data or control frame has arrived */
    from physical layer(&r); /* get incoming frame from physical layer */
    if (r.seq == frame expected)
    {
        /* Frames are accepted only in order. */
        to network layer(&r.info); /* pass packet to network layer */
        inc(frame expected);       /* advance lower edge of receiver’s window */
    }                              /* Ack n implies n − 1, n − 2, etc. Check for this. */
    while (between(ack expected, r.ack, next frame_to_send))
    {
        /* Handle piggybacked ack. */
        nbuffered = nbuffered − 1; /* one frame fewer buffered */
        stop timer(ack expected);  /* frame arrived intact; stop timer */
        inc(ack expected);         /* contract sender’s window */
    }
    break;
case cksum err:
    break;                             /* just ignore bad frames */
case timeout:                          /* trouble; retransmit all outstanding frames */
    next frame_to_send = ack expected; /* start retransmitting here */
    for (i = 1; i <= nbuffered; i++)
    {
        send data(next frame_to_send, frame expected, buffer); /* resend frame */
        inc(next frame_to_send);                               /* prepare to send the next one */
    }
}
if (nbuffered < MAX SEQ)
    enable network layer();
else
    disable network layer();
    }
}