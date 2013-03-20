#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "controller.hh"
#include "timestamp.hh"

using namespace Network;

double _cwnd;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), ALPHA(0), BETA(0), GAMMA(0), DELTA(0)
{
  _cwnd = 1;
}

/* Get current window size, in packets */
unsigned int Controller::window_size( void )
{
  /* Default: fixed window size of one outstanding packet */
  char* alpha_str = getenv("ALPHA");
  ALPHA = atof(alpha_str);
  char* beta_str = getenv("BETA");
  BETA = atof(beta_str);
  char* gamma_str = getenv("GAMMA");
  GAMMA = atof(gamma_str);
  char* delta_str = getenv("DELTA");
  DELTA = atof(delta_str);

  int the_window_size = 1;

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

  double ALPHA = 2;
  double BETA = 1.0/2.0;
  double DROP_THRESHOLD = 150; /* in milliseconds */

  uint64_t RTT = timestamp_ack_received - send_timestamp_acked;
  bool simulate_drop = RTT > DROP_THRESHOLD;

  if (simulate_drop){
    _cwnd = BETA * _cwnd;
  } else {
    _cwnd += ALPHA / _cwnd;
  }

  _cwnd = std::max(1.0, _cwnd);

  if ( debug_ ) {
    fprintf( stderr, "At time %lu, received ACK for packet %lu",
	     timestamp_ack_received, sequence_number_acked );

    fprintf( stderr, " (sent %lu, received %lu by receiver's clock).\n",
	     send_timestamp_acked, recv_timestamp_acked );
  }
}

/* How long to wait if there are no acks before sending one more packet */
unsigned int Controller::timeout_ms( void )
{
  return 1000; /* timeout of one second */
}
