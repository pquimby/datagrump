#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "controller.hh"
#include "timestamp.hh"

using namespace Network;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), 
    cwnd ( 5), 
    last_ack_timestamp(timestamp()), 
    RTT_min(100),
    interarrival_average(0),
    timestamp_zero(timestamp()),
  ALPHA(5.0),
  BETA(1.25),
  GAMMA(50),
  DELTA(0.25)
{
}

/* Get current window size, in packets */
unsigned int Controller::window_size( void )
{
  int the_window_size = (int)cwnd;

  if ( debug_ ) {
    fprintf( stderr, "At time %lu, return window_size = %d.\n",
	     timestamp(), the_window_size );
  }

  return the_window_size;
}

/* A packet was sent */
void Controller::packet_was_sent( const uint64_t sequence_number,
				  /* of the sent packet */
				  const uint64_t send_timestamp )
                                  /* in milliseconds */
{
  if ( debug_ ) {
    fprintf( stderr, "At time %lu, sent packet %lu. :)\n",
	     send_timestamp-timestamp_zero, sequence_number );
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged packet was sent */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged packet was received */
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{

  uint64_t RTT = timestamp_ack_received - send_timestamp_acked;
  
  /* Update RTT_min */
  if (RTT < RTT_min) {
    RTT_min = RTT;
  }
  fprintf(stderr, "RTT %lu RTT_min %lu\n", RTT, RTT_min);

  int NO_QUEUE_EPSILON = 5; /* in milliseconds */
  int DESIRED_QUEUE_SIZE = ALPHA;

  double interarrival = timestamp_ack_received - last_ack_timestamp;
  if (interarrival < 0.01) {
     interarrival = 1.0; 
  }
  fprintf(stderr, "interarrival %f\n", interarrival);
  double interarrival_average_new = interarrival_average * (1.0-DELTA) + interarrival * (DELTA);
  fprintf(stderr, "interarrival_average_new %f\n", interarrival_average_new);

  if (std::abs(RTT - RTT_min) < NO_QUEUE_EPSILON) {
    fprintf( stderr, "NO QUEUE DETECTED.\n" );
    cwnd += 1/cwnd;
  } else {
    if (interarrival > interarrival_average) {
      interarrival_average = interarrival;
    } else {
      interarrival_average = interarrival_average_new;
    }
    fprintf(stderr, "interarrival_average %f\n", interarrival_average);

    cwnd = BETA * RTT_min / interarrival_average + DESIRED_QUEUE_SIZE;
  }
  fprintf(stderr, "cwnd %f\n", cwnd);

  last_ack_timestamp = timestamp_ack_received;

  if ( debug_ ) {

    fprintf( stderr, "At time %lu, received ACK for packet %lu",
	     timestamp_ack_received-timestamp_zero, sequence_number_acked );

    fprintf( stderr, " (sent %lu, received %lu by receiver's clock).\n",
	     send_timestamp_acked-timestamp_zero, recv_timestamp_acked );
  }

  cwnd = std::max(1.0, cwnd);
}

/* How long to wait if there are no acks before sending one more packet */
unsigned int Controller::timeout_ms( void )
{
  return (int)GAMMA; /* timeout of one second */
}
