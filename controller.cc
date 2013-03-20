#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "controller.hh"
#include "timestamp.hh"

using namespace Network;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), cwnd(1), last_ack_timestamp(timestamp()), ALPHA(0), BETA(0), GAMMA(0), DELTA(0)
{

  char* alpha_str = getenv("ALPHA");
  ALPHA = atof(alpha_str);
  char* beta_str = getenv("BETA");
  BETA = atof(beta_str);
  char* gamma_str = getenv("GAMMA");
  GAMMA = atof(gamma_str);
  char* delta_str = getenv("DELTA");
  DELTA = atof(delta_str);

}

/* Get current window size, in packets */
unsigned int Controller::window_size( void )
{
  uint64_t time_since_last_ack = timestamp() - last_ack_timestamp;
  if (debug_) {
    fprintf( stderr, "time_since_last_ack %lu \n", time_since_last_ack);
  }

  uint64_t TIMEOUT_THRESHOLD = GAMMA; /* in milliseconds */

  if (time_since_last_ack > TIMEOUT_THRESHOLD){
    if (debug_) {
      fprintf( stderr, "cwnd is %f, ", cwnd);
    }
    cwnd = BETA * cwnd;
    if (debug_) {
      fprintf( stderr, "decreasing cwnd to %f\n", cwnd);
    }
  }

  cwnd = std::max(1.0, cwnd);
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
  /* Default: take no action */
  if ( debug_ ) {
    fprintf( stderr, "At time %lu, sent packet %lu.\n",
	     send_timestamp, sequence_number );
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
  last_ack_timestamp = timestamp_ack_received;

  if (debug_) {
    fprintf( stderr, "cwnd is %f, ", cwnd);
  }  
  cwnd += ALPHA / cwnd;
  if (debug_) {
    fprintf( stderr, "growing cwnd to %f\n", cwnd);
  }  

  if ( debug_ ) {
    fprintf( stderr, "At t= %lu, received ACK for p# %lu, cwnd %f",
	     timestamp_ack_received, sequence_number_acked, cwnd );

    fprintf( stderr, " (sent %lu, received %lu by receiver's clock).\n",
	     send_timestamp_acked, recv_timestamp_acked );
  }
}

/* How long to wait if there are no acks before sending one more packet */
unsigned int Controller::timeout_ms( void )
{
  return (int)GAMMA; /* timeout of one second */
}
